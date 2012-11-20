#include "FoundationPch.h"
#include "MemoryStream.h"

#include "Platform/Trace.h"
#include "Foundation/Math.h"

using namespace Helium;

/// Constructor.
StaticMemoryStream::StaticMemoryStream()
    : m_pStart( NULL )
    , m_pEnd( NULL )
    , m_pCurrent( NULL )
{
}

/// Constructor.
///
/// @param[in] pData  Start address of the memory buffer to use with this stream.
/// @param[in] size   Size of the memory buffer to use with this stream (ignored if pData is null).
StaticMemoryStream::StaticMemoryStream( void* pData, size_t size )
    : m_pStart( static_cast< uint8_t* >( pData ) )
    , m_pEnd( pData ? static_cast< uint8_t* >( pData ) + size : NULL )
    , m_pCurrent( static_cast< uint8_t* >( pData ) )
{
}

/// Destructor.
StaticMemoryStream::~StaticMemoryStream()
{
}

/// Set the memory buffer to use with this stream.
///
/// @param[in] pData  Start address of the memory buffer to use, or null to clear any currently set buffer settings.
/// @param[in] size   Size of the memory buffer to use with this stream (ignored if pData is null).
void StaticMemoryStream::Open( void* pData, size_t size )
{
    m_pStart = static_cast< uint8_t* >( pData );
    m_pEnd = ( pData ? static_cast< uint8_t* >( pData ) + size : NULL );
    m_pCurrent = static_cast< uint8_t* >( pData );
}

/// @copydoc Stream::Close()
void StaticMemoryStream::Close()
{
    m_pStart = NULL;
    m_pEnd = NULL;
    m_pCurrent = NULL;
}

/// @copydoc Stream::IsOpen()
bool StaticMemoryStream::IsOpen() const
{
    return ( m_pStart != NULL );
}

/// @copydoc Stream::Read()
size_t StaticMemoryStream::Read( void* pBuffer, size_t size, size_t count )
{
    HELIUM_ASSERT( pBuffer );
    HELIUM_ASSERT( IsOpen() );

    size_t byteCount = size * count;

    size_t bytesRemaining = static_cast< size_t >( m_pEnd - m_pCurrent );
    if( byteCount > bytesRemaining )
    {
        byteCount = bytesRemaining;
    }

    MemoryCopy( pBuffer, m_pCurrent, byteCount );
    m_pCurrent += byteCount;

    return ( byteCount / size );
}

/// @copydoc Stream::Write()
size_t StaticMemoryStream::Write( const void* pBuffer, size_t size, size_t count )
{
    HELIUM_ASSERT( pBuffer );
    HELIUM_ASSERT( IsOpen() );

    size_t byteCount = size * count;

    size_t bytesRemaining = static_cast< size_t >( m_pEnd - m_pCurrent );
    if( byteCount > bytesRemaining )
    {
        byteCount = bytesRemaining;
    }

    MemoryCopy( m_pCurrent, pBuffer, byteCount );
    m_pCurrent += byteCount;

    return ( byteCount / size );
}

/// @copydoc Stream::Flush()
void StaticMemoryStream::Flush()
{
    HELIUM_ASSERT( IsOpen() );

    // Nothing needs to be done when flushing this type of stream.
}

/// @copydoc Stream::Seek()
int64_t StaticMemoryStream::Seek( int64_t offset, SeekOrigin origin )
{
    HELIUM_ASSERT( static_cast< size_t >( origin ) < static_cast< size_t >( SeekOrigins::SEEK_ORIGIN_MAX ) );
    HELIUM_ASSERT( IsOpen() );

    size_t referenceOffset;
    switch( origin )
    {
        case SeekOrigins::SEEK_ORIGIN_CURRENT:
        {
            referenceOffset = static_cast< size_t >( m_pCurrent - m_pStart );

            break;
        }

        case SeekOrigins::SEEK_ORIGIN_BEGIN:
        {
            referenceOffset = 0;

            break;
        }

        case SeekOrigins::SEEK_ORIGIN_END:
        {
            referenceOffset = static_cast< size_t >( m_pEnd - m_pStart );

            break;
        }

        default:
        {
            HELIUM_TRACE( TraceLevels::Error, TXT( "DynamicMemoryStream::Seek(): Invalid seek origin specified.\n" ) );

            return static_cast< int64_t >( static_cast< size_t >( m_pCurrent - m_pStart ) );
        }
    }

    if( offset < 0 )
    {
        uint64_t absOffset = static_cast< uint64_t >( -offset );
        HELIUM_ASSERT( absOffset <= static_cast< uint64_t >( referenceOffset ) );
        if( absOffset > static_cast< uint64_t >( referenceOffset ) )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
                TXT( "DynamicMemoryStream::Seek(): Attempted to seek before the start of the memory stream.\n" ) );
        }
        else
        {
            m_pCurrent = m_pStart + referenceOffset - static_cast< size_t >( absOffset );
        }
    }
    else
    {
        uint64_t absOffset = static_cast< uint64_t >( offset );
        HELIUM_ASSERT( absOffset <= static_cast< uint64_t >( static_cast< size_t >( -1 ) - referenceOffset ) );
        if( absOffset > static_cast< uint64_t >( static_cast< size_t >( m_pEnd - m_pStart ) - referenceOffset ) )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
                TXT( "DynamicMemoryStream::Seek(): Attempted to seek past the end of the memory stream.\n" ) );
        }
        else
        {
            m_pCurrent = m_pStart + referenceOffset + static_cast< size_t >( absOffset );
        }
    }

    return static_cast< int64_t >( static_cast< size_t >( m_pCurrent - m_pStart ) );
}

/// @see Stream::Tell()
int64_t StaticMemoryStream::Tell() const
{
    return static_cast< int64_t >( static_cast< size_t >( m_pCurrent - m_pStart ) );
}

/// @see Stream::GetSize()
int64_t StaticMemoryStream::GetSize() const
{
    return static_cast< int64_t >( static_cast< size_t >( m_pEnd - m_pStart ) );
}

/// @see Stream::CanRead()
bool StaticMemoryStream::CanRead() const
{
    return true;
}

/// @see Stream::CanWrite()
bool StaticMemoryStream::CanWrite() const
{
    return true;
}

/// @see Stream::CanSeek()
bool StaticMemoryStream::CanSeek() const
{
    return true;
}

/// Constructor.
///
/// @param[in] pBuffer  Dynamic array to use as this stream's memory buffer.
DynamicMemoryStream::DynamicMemoryStream( DynArray< uint8_t >* pBuffer )
    : m_pBuffer( pBuffer )
    , m_offset( 0 )
{
}

/// Destructor.
DynamicMemoryStream::~DynamicMemoryStream()
{
}

/// Set the buffer to use with this stream.
///
/// Any currently assigned buffer will be unset, and the current stream offset will be reset to zero.
///
/// @param[in] pBuffer  Dynamic array of bytes to use as this stream's memory buffer.
void DynamicMemoryStream::Open( DynArray< uint8_t >* pBuffer )
{
    m_pBuffer = pBuffer;
    m_offset = 0;
}

/// @copydoc Stream::Close()
void DynamicMemoryStream::Close()
{
    m_pBuffer = NULL;
    m_offset = 0;
}

/// @copydoc Stream::IsOpen()
bool DynamicMemoryStream::IsOpen() const
{
    return ( m_pBuffer != NULL );
}

/// @copydoc Stream::Read()
size_t DynamicMemoryStream::Read( void* pBuffer, size_t size, size_t count )
{
    HELIUM_ASSERT( m_pBuffer );
    if( !m_pBuffer )
    {
        return 0;
    }

    size_t bytesRemaining = m_pBuffer->GetSize() - m_offset;
    size_t byteCount = Min( size * count, bytesRemaining );

    MemoryCopy( pBuffer, m_pBuffer->GetData() + m_offset, byteCount );
    m_offset += byteCount;

    return ( byteCount / size );
}

/// @copydoc Stream::Write()
size_t DynamicMemoryStream::Write( const void* pBuffer, size_t size, size_t count )
{
    HELIUM_ASSERT( m_pBuffer );
    if( !m_pBuffer )
    {
        return 0;
    }

    size_t byteCount = size * count;

    size_t bytesRemaining = m_pBuffer->GetSize() - m_offset;
    size_t writeCount = Min( byteCount, bytesRemaining );

    MemoryCopy( m_pBuffer->GetData() + m_offset, pBuffer, writeCount );

    size_t remainingCount = byteCount - writeCount;
    if( remainingCount != 0 )
    {
        m_pBuffer->AddArray( static_cast< const uint8_t* >( pBuffer ) + writeCount, remainingCount );
    }

    m_offset += byteCount;

    return ( byteCount / size );
}

/// @copydoc Stream::Flush()
void DynamicMemoryStream::Flush()
{
    // Nothing needs to be done for this class.
}

/// @copydoc Stream::Seek()
int64_t DynamicMemoryStream::Seek( int64_t offset, SeekOrigin origin )
{
    HELIUM_ASSERT( m_pBuffer );
    if( !m_pBuffer )
    {
        return -1;
    }

    HELIUM_ASSERT( static_cast< size_t >( origin ) < static_cast< size_t >( SeekOrigins::SEEK_ORIGIN_MAX ) );

    size_t referenceOffset;
    switch( origin )
    {
        case SeekOrigins::SEEK_ORIGIN_CURRENT:
        {
            referenceOffset = m_offset;

            break;
        }

        case SeekOrigins::SEEK_ORIGIN_BEGIN:
        {
            referenceOffset = 0;

            break;
        }

        case SeekOrigins::SEEK_ORIGIN_END:
        {
            referenceOffset = m_pBuffer->GetSize();

            break;
        }

        default:
        {
            HELIUM_TRACE( TraceLevels::Error, TXT( "DynamicMemoryStream::Seek(): Invalid seek origin specified.\n" ) );

            return static_cast< int64_t >( m_offset );
        }
    }

    if( offset < 0 )
    {
        uint64_t absOffset = static_cast< uint64_t >( -offset );
        HELIUM_ASSERT( absOffset <= static_cast< uint64_t >( referenceOffset ) );
        if( absOffset > static_cast< uint64_t >( referenceOffset ) )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
                TXT( "DynamicMemoryStream::Seek(): Attempted to seek before the start of the memory stream.\n" ) );
        }
        else
        {
            m_offset = referenceOffset - static_cast< size_t >( absOffset );
        }
    }
    else
    {
        uint64_t absOffset = static_cast< uint64_t >( offset );
        HELIUM_ASSERT( absOffset <= static_cast< uint64_t >( static_cast< size_t >( -1 ) - referenceOffset ) );
        if( absOffset >= static_cast< uint64_t >( static_cast< size_t >( -1 ) - referenceOffset ) )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
                ( TXT( "DynamicMemoryStream::Seek(): Attempted to seek outside the maximum buffer size supported " )
                  TXT( "by the current platform.\n" ) ) );
        }
        else
        {
            m_offset = referenceOffset + static_cast< size_t >( absOffset );

            if( m_offset > m_pBuffer->GetSize() )
            {
                size_t additionalBytes = m_offset - m_pBuffer->GetSize();
                m_pBuffer->Add( 0, additionalBytes );
            }
        }
    }

    return static_cast< int64_t >( m_offset );
}

/// @copydoc Stream::Tell()
int64_t DynamicMemoryStream::Tell() const
{
    return ( m_pBuffer ? static_cast< int64_t >( m_offset ) : -1 );
}

/// @copydoc Stream::GetSize()
int64_t DynamicMemoryStream::GetSize() const
{
    return ( m_pBuffer ? static_cast< int64_t >( m_pBuffer->GetSize() ) : 0 );
}

/// @copydoc Stream::CanRead()
bool DynamicMemoryStream::CanRead() const
{
    return ( m_pBuffer != NULL );
}

/// @copydoc Stream::CanWrite()
bool DynamicMemoryStream::CanWrite() const
{
    return ( m_pBuffer != NULL );
}

/// @copydoc Stream::CanSeek()
bool DynamicMemoryStream::CanSeek() const
{
    return ( m_pBuffer != NULL );
}