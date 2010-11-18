#include "Foundation/Stream/FileStream.h"

using namespace Helium;

/// Constructor.
FileStream::FileStream()
: m_modeFlags( 0 )
, m_hFile( InvalidHandleValue )
{
}

/// Destructor.
FileStream::~FileStream()
{
    Close();
}

/// @copydoc Stream::Close()
void FileStream::Close()
{
    if( m_hFile != InvalidHandleValue )
    {
        CloseHandle( m_hFile );
        m_hFile = InvalidHandleValue;
    }
}

/// @copydoc Stream::IsOpen()
bool FileStream::IsOpen() const
{
    return ( m_hFile != InvalidHandleValue );
}

/// @copydoc Stream::Read()
size_t FileStream::Read( void* pBuffer, size_t size, size_t count )
{
    HELIUM_ASSERT_MSG( m_hFile != InvalidHandleValue, TXT( "File not open" ) );
    HELIUM_ASSERT_MSG( m_modeFlags & MODE_READ, TXT( "File not open for reading" ) );
    if( m_hFile == InvalidHandleValue || !( m_modeFlags & MODE_READ ) )
    {
        return 0;
    }

    size_t byteCount = size * count;
    uint32_t bytesRead = 0;
    HELIUM_VERIFY( ReadFile( m_hFile, pBuffer, byteCount, &bytesRead ) );

    return ( bytesRead / size );
}

/// @copydoc Stream::Write()
size_t FileStream::Write( const void* pBuffer, size_t size, size_t count )
{
    HELIUM_ASSERT_MSG( m_hFile != InvalidHandleValue, TXT( "File not open" ) );
    HELIUM_ASSERT_MSG( m_modeFlags & MODE_WRITE, TXT( "File not open for writing" ) );
    if( m_hFile == InvalidHandleValue || !( m_modeFlags & MODE_WRITE ) )
    {
        return 0;
    }

    size_t byteCount = size * count;
    uint32_t bytesWritten = 0;
    HELIUM_VERIFY( WriteFile( m_hFile, pBuffer, byteCount, &bytesWritten ) );

    return ( bytesWritten / size );
}

/// @copydoc Stream::Flush()
void FileStream::Flush()
{
    HELIUM_ASSERT_MSG( m_hFile != InvalidHandleValue, TXT( "File not open" ) );

    // Only files open for writing need to be flushed.
    if( m_hFile != InvalidHandleValue && ( m_modeFlags & MODE_WRITE ) )
    {
        HELIUM_VERIFY( FlushFile( m_hFile ) );
    }
}

/// @copydoc Stream::Seek()
int64_t FileStream::Seek( int64_t offset, SeekOrigin origin )
{
    if( m_hFile == InvalidHandleValue )
    {
        HELIUM_ASSERT_MSG_FALSE( TXT( "File not open" ) );
        return -1;
    }

    return Helium::Seek( m_hFile, offset, origin );
}

/// @copydoc Stream::Tell()
int64_t FileStream::Tell() const
{
    if( m_hFile == InvalidHandleValue )
    {
        HELIUM_ASSERT_MSG_FALSE( TXT( "File not open" ) );
        return -1;
    }

    return Helium::Tell( m_hFile );
}

/// @copydoc Stream::GetSize()
int64_t FileStream::GetSize() const
{
    if( m_hFile == InvalidHandleValue )
    {
        HELIUM_ASSERT_MSG_FALSE( TXT( "File not open" ) );
        return -1;
    }

    return Helium::GetSize( m_hFile );
}

/// @copydoc FileStream::OpenActual()
bool FileStream::Open( const tchar_t* pPath, uint32_t modeFlags, bool bTruncate )
{
    HELIUM_ASSERT( pPath );

    // Verify that at least one mode flag is given.
    if( !( modeFlags & ( MODE_READ | MODE_WRITE ) ) )
    {
        HELIUM_ASSERT_MSG_FALSE( TXT( "At least one FileStream::EMode flag must be set" ) );
        return false;
    }

    // Close any currently open file.
    Close();

    HELIUM_ASSERT( m_hFile == InvalidHandleValue );

    m_hFile = Helium::CreateFile( pPath, (Helium::FileMode)modeFlags, bTruncate );

    if ( m_hFile == InvalidHandleValue )
    {
        return false;
    }

    m_modeFlags = modeFlags;
    return true;
}
