#include "FoundationPch.h"
#include "Foundation/Stream/BufferedStream.h"

#include "Platform/Memory.h"
#include "Foundation/Math.h"

using namespace Helium;

/// Constructor.
///
/// Creates a buffered stream, wrapped around a given stream, with a specific buffer size.  Note that the stream and
/// buffer size can be modified later with Open().
///
/// @param[in] pStream     Stream around which the buffered stream should be wrapped (can be null to leave
///                        uninitialized).
/// @param[in] bufferSize  Size of the buffer, in bytes.  The buffer will be allocated even if the stream is null.
///                        If this is zero, no buffering will be performed.
BufferedStream::BufferedStream( Stream* pStream, size_t bufferSize )
    : m_pStream( pStream )
    , m_pBuffer( NULL )
    , m_bufferSize( bufferSize )
    , m_bufferStart( -1 )
    , m_bufferedByteCount( 0 )
    , m_bufferOffset( 0 )
    , m_bReadData( false )
{
    // Allocate the stream buffer.
    if( bufferSize != 0 )
    {
        m_pBuffer = DefaultAllocator().Allocate( bufferSize );
        HELIUM_ASSERT( m_pBuffer );
    }

    // Initialize the buffer seek offset if seeking is supported.
    if( pStream && CanSeek() )
    {
        m_bufferStart = pStream->Tell();
    }
}

/// Destructor.
BufferedStream::~BufferedStream()
{
    Close();

    DefaultAllocator().Free( m_pBuffer );
}

/// Assign a stream to this buffered stream and update the buffer size.
///
/// Any stream currently assigned will be flushed, but not closed, when changing the stream or buffer size.
///
/// @param[in] pStream     Stream around which the buffered stream should be wrapped (can be null to leave
///                        uninitialized).
/// @param[in] bufferSize  Size of the buffer, in bytes.  The buffer will be allocated even if the stream is null.
///                        If this is zero, no buffering will be performed.
void BufferedStream::Open( Stream* pStream, size_t bufferSize )
{
    // Check if the stream or buffer are changing.
    if( pStream == m_pStream && bufferSize == m_bufferSize )
    {
        return;
    }

    // Flush the current stream.
    Flush();

    // Assign the new stream.
    if( pStream != m_pStream )
    {
        m_pStream = pStream;

        m_bufferStart = -1;
        if( pStream && CanSeek() )
        {
            m_bufferStart = pStream->Tell();
        }
    }

    // Resize the buffer if necessary.
    if( bufferSize != m_bufferSize )
    {
        m_pBuffer = DefaultAllocator().Reallocate( m_pBuffer, bufferSize );
        HELIUM_ASSERT( m_pBuffer || bufferSize == 0 );

        m_bufferSize = 0;
    }
}

/// Close this stream.
///
/// This will flush this stream, call Close() on any underlying stream, and unset the stream.  Any memory allocated
/// for the buffer will be left intact.
void BufferedStream::Close()
{
    Flush();

    if( m_pStream )
    {
        m_pStream->Close();
        m_pStream = NULL;

        m_bufferStart = -1;
    }
}

/// @copydoc Stream::IsOpen()
bool BufferedStream::IsOpen() const
{
    return( m_pStream && m_pStream->IsOpen() );
}

/// @copydoc Stream::Read()
size_t BufferedStream::Read( void* pBuffer, size_t size, size_t count )
{
    HELIUM_ASSERT( pBuffer || count == 0 );

    // Make sure we can read from the stream.
    HELIUM_ASSERT( CanRead() );
    if( !CanRead() )
    {
        return 0;
    }

    HELIUM_ASSERT( m_pStream );

    bool bCanSeek = CanSeek();

    // Pass through if we have no buffer.
    if( m_bufferSize == 0 )
    {
        if( bCanSeek )
        {
            int64_t fileOffset = m_pStream->Tell() - m_bufferStart;
            if( fileOffset != 0 )
            {
                m_pStream->Seek( fileOffset, SeekOrigins::SEEK_ORIGIN_CURRENT );
            }
        }

        size_t blocksRead = m_pStream->Read( pBuffer, size, count );

        if( bCanSeek )
        {
            m_bufferStart = m_pStream->Tell();
        }

        return blocksRead;
    }

    // If the buffer currently contains written data, flush it and switch the buffer mode.
    if( !m_bReadData )
    {
        Flush();
        m_bReadData = true;
    }

    size_t byteCount = size * count;

    size_t remainingByteCount = byteCount;
    while( remainingByteCount != 0 )
    {
        // Re-fill the buffer if necessary.
        if( m_bufferOffset >= m_bufferedByteCount )
        {
            if( bCanSeek )
            {
                m_bufferStart += m_bufferedByteCount;
                int64_t fileOffset = m_pStream->Tell() - m_bufferStart;
                if( fileOffset != 0 )
                {
                    m_bufferStart = m_pStream->Seek( fileOffset, SeekOrigins::SEEK_ORIGIN_CURRENT );
                }
            }

            m_bufferOffset = 0;
            m_bufferedByteCount = m_pStream->Read( m_pBuffer, 1, m_bufferSize );
            if( m_bufferedByteCount == 0 )
            {
                // End of file, so finish reading.
                return ( ( byteCount - remainingByteCount ) / size );
            }
        }

        // Transfer buffer contents over to the output buffer.
        size_t transferCount = Min( remainingByteCount, m_bufferedByteCount - m_bufferOffset );
        MemoryCopy( pBuffer, static_cast< uint8_t* >( m_pBuffer ) + m_bufferOffset, transferCount );
        pBuffer = static_cast< uint8_t* >( pBuffer ) + transferCount;
        m_bufferOffset += transferCount;

        remainingByteCount -= transferCount;
    }

    // All bytes read successfully.
    return count;
}

/// @copydoc Stream::Write()
size_t BufferedStream::Write( const void* pBuffer, size_t size, size_t count )
{
    HELIUM_ASSERT( pBuffer || count == 0 );

    // Make sure we can write from the stream.
    HELIUM_ASSERT( CanWrite() );
    if( !CanWrite() )
    {
        return 0;
    }

    HELIUM_ASSERT( m_pStream );

    // Pass through if we have no buffer.
    if( m_bufferSize == 0 )
    {
        bool bCanSeek = CanSeek();

        if( bCanSeek )
        {
            int64_t fileOffset = m_pStream->Tell() - m_bufferStart;
            if( fileOffset != 0 )
            {
                m_pStream->Seek( fileOffset, SeekOrigins::SEEK_ORIGIN_CURRENT );
            }
        }

        size_t bytesWritten = m_pStream->Write( pBuffer, size, count );

        if( bCanSeek )
        {
            m_bufferStart = m_pStream->Tell();
        }

        return bytesWritten;
    }

    // If the buffer currently contains read data, flush it and switch the buffer mode.
    if( m_bReadData )
    {
        Flush();
        m_bReadData = false;
    }

    size_t remainingByteCount = size * count;
    while( remainingByteCount != 0 )
    {
        // Flush if the buffer is full.
        if( m_bufferOffset >= m_bufferSize )
        {
            Flush();
            HELIUM_ASSERT( m_bufferOffset == 0 );
        }

        // Fill the buffer with data to write.
        size_t transferCount = Min( remainingByteCount, m_bufferSize - m_bufferOffset );
        MemoryCopy( static_cast< uint8_t* >( m_pBuffer ) + m_bufferOffset, pBuffer, transferCount );
        pBuffer = static_cast< const uint8_t* >( pBuffer ) + transferCount;
        m_bufferOffset += transferCount;

        remainingByteCount -= transferCount;
    }

    return count;
}

/// @copydoc Stream::Flush()
void BufferedStream::Flush()
{
    // Don't do anything if we have no open stream.
    if( !m_pStream )
    {
        return;
    }

    bool bCanSeek = CanSeek();

    // Commit any pending data to write.
    if( !m_bReadData && m_bufferOffset != 0 )
    {
        HELIUM_ASSERT( CanWrite() );

        if( bCanSeek )
        {
            int64_t fileOffset = m_pStream->Tell() - m_bufferStart;
            if( fileOffset != 0 )
            {
                m_pStream->Seek( fileOffset, SeekOrigins::SEEK_ORIGIN_CURRENT );
            }
        }

        m_pStream->Write( m_pBuffer, 1, m_bufferOffset );
    }

    // Reset the buffer.
    m_bufferOffset = 0;
    m_bufferedByteCount = 0;

    m_bufferStart = ( bCanSeek ? m_pStream->Tell() : -1 );

    // Flush the underlying stream.
    m_pStream->Flush();
}

/// @copydoc Stream::Seek()
int64_t BufferedStream::Seek( int64_t offset, SeekOrigin origin )
{
    HELIUM_ASSERT( m_pStream );
    HELIUM_ASSERT( CanSeek() );
    if( !m_pStream || !CanSeek() )
    {
        return -1;
    }

    Flush();

    m_bufferStart = m_pStream->Seek( offset, origin );

    return m_bufferStart;
}

/// @copydoc Stream::Tell()
int64_t BufferedStream::Tell() const
{
    return m_bufferStart + static_cast< int64_t >( m_bufferOffset );
}

/// @copydoc Stream::GetSize()
int64_t BufferedStream::GetSize() const
{
    return( m_pStream && CanSeek() ? m_pStream->GetSize() : -1 );
}

/// @copydoc Stream::CanRead()
bool BufferedStream::CanRead() const
{
    return( m_pStream && m_pStream->CanRead() );
}

/// @copydoc Stream::CanWrite()
bool BufferedStream::CanWrite() const
{
    return( m_pStream && m_pStream->CanWrite() );
}

/// @copydoc Stream::CanSeek()
bool BufferedStream::CanSeek() const
{
    return( m_pStream && m_pStream->CanSeek() );
}
