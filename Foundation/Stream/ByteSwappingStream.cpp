//#include "CorePch.h"
#include "Foundation/Stream/ByteSwappingStream.h"

using namespace Helium;

/// Constructor.
///
/// @param[in] pStream  Stream around which this stream should be wrapped (can be null to leave uninitialized).
ByteSwappingStream::ByteSwappingStream( Stream* pStream )
    : m_pStream( pStream )
{
}

/// Destructor.
ByteSwappingStream::~ByteSwappingStream()
{
    Close();
}

/// Set the stream on which to perform byte swapping.
///
/// Any stream currently assigned will be flushed, but not closed, when changing the stream.
///
/// @param[in] pStream  Stream around which this stream should be wrapped (can be null to leave uninitialized).
void ByteSwappingStream::Open( Stream* pStream )
{
    if( m_pStream )
    {
        m_pStream->Flush();
    }

    m_pStream = pStream;
}

/// @copydoc Stream::Close()
void ByteSwappingStream::Close()
{
    if( m_pStream )
    {
        m_pStream->Close();
        m_pStream = NULL;
    }
}

/// @copydoc Stream::IsOpen()
bool ByteSwappingStream::IsOpen() const
{
    return ( m_pStream && m_pStream->IsOpen() );
}

/// @copydoc Stream::Read()
size_t ByteSwappingStream::Read( void* pBuffer, size_t size, size_t count )
{
    HELIUM_ASSERT( CanRead() );
    if( !CanRead() || count == 0 )
    {
        return 0;
    }

    HELIUM_ASSERT( m_pStream );

    // Reduce the number of individual calls to Stream::Read() if reading only single-byte values.
    if( size == 1 )
    {
        size_t bytesRead = m_pStream->Read( pBuffer, 1, count );

        return bytesRead;
    }

    for( size_t blockIndex = 0; blockIndex < count; ++blockIndex )
    {
        for( size_t byteIndex = 0; byteIndex < size; ++byteIndex )
        {
            size_t bytesRead = m_pStream->Read( static_cast< uint8_t* >( pBuffer ) + size - byteIndex - 1, 1, 1 );
            if( bytesRead != 1 )
            {
                return blockIndex;
            }
        }
    }

    return count;
}

/// @copydoc Stream::Write()
size_t ByteSwappingStream::Write( const void* pBuffer, size_t size, size_t count )
{
    HELIUM_ASSERT( CanWrite() );
    if( !CanWrite() || count == 0 )
    {
        return 0;
    }

    HELIUM_ASSERT( m_pStream );

    // Reduce the number of individual calls to Stream::Write() if writing only single-byte values.
    if( size == 1 )
    {
        size_t bytesWritten = m_pStream->Write( pBuffer, 1, count );

        return bytesWritten;
    }

    for( size_t blockIndex = 0; blockIndex < count; ++blockIndex )
    {
        for( size_t byteIndex = 0; byteIndex < size; ++byteIndex )
        {
            size_t bytesWritten = m_pStream->Write(
                static_cast< const uint8_t* >( pBuffer ) + size - byteIndex - 1,
                1,
                1 );
            if( bytesWritten != 1 )
            {
                return blockIndex;
            }
        }
    }

    return count;
}

/// @copydoc Stream::Flush()
void ByteSwappingStream::Flush()
{
    if( m_pStream )
    {
        m_pStream->Flush();
    }
}

/// @copydoc Stream::Seek()
int64_t ByteSwappingStream::Seek( int64_t offset, ESeekOrigin origin )
{
    HELIUM_ASSERT( CanSeek() );
    if( !CanSeek() )
    {
        return -1;
    }

    HELIUM_ASSERT( m_pStream );
    int64_t newOffset = m_pStream->Seek( offset, origin );

    return newOffset;
}

/// @copydoc Stream::Tell()
int64_t ByteSwappingStream::Tell() const
{
    return ( m_pStream ? m_pStream->Tell() : -1 );
}

/// @copydoc Stream::GetSize()
int64_t ByteSwappingStream::GetSize() const
{
    return ( m_pStream ? m_pStream->GetSize() : -1 );
}

/// @copydoc Stream::CanRead()
bool ByteSwappingStream::CanRead() const
{
    return ( m_pStream && m_pStream->CanRead() );
}

/// @copydoc Stream::CanWrite()
bool ByteSwappingStream::CanWrite() const
{
    return ( m_pStream && m_pStream->CanWrite() );
}

/// @copydoc Stream::CanSeek()
bool ByteSwappingStream::CanSeek() const
{
    return ( m_pStream && m_pStream->CanSeek() );
}