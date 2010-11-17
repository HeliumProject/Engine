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
    HELIUM_ASSERT_MSG( byteCount <= MAXDWORD, TXT( "File read operations are limited to DWORD sizes" ) );
    if( byteCount > MAXDWORD )
    {
        // Truncate to a multiple of "size" bytes.
        byteCount = ( MAXDWORD / size ) * size;
    }

    HELIUM_ASSERT( pBuffer || byteCount == 0 );

    DWORD bytesRead = 0;
    HELIUM_VERIFY( ReadFile( m_hFile, pBuffer, static_cast< DWORD >( byteCount ), &bytesRead, NULL ) );

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
    HELIUM_ASSERT_MSG( byteCount <= MAXDWORD, TXT( "File write operations are limited to DWORD sizes" ) );
    if( byteCount > MAXDWORD )
    {
        // Truncate to a multiple of "size" bytes.
        byteCount = ( MAXDWORD / size ) * size;
    }

    HELIUM_ASSERT( pBuffer || byteCount == 0 );

    DWORD bytesWritten = 0;
    HELIUM_VERIFY( WriteFile( m_hFile, pBuffer, static_cast< DWORD >( byteCount ), &bytesWritten, NULL ) );

    return ( bytesWritten / size );
}

/// @copydoc Stream::Flush()
void FileStream::Flush()
{
    HELIUM_ASSERT_MSG( m_hFile != InvalidHandleValue, TXT( "File not open" ) );

    // Only files open for writing need to be flushed.
    if( m_hFile != InvalidHandleValue && ( m_modeFlags & MODE_WRITE ) )
    {
        HELIUM_VERIFY( FlushFileBuffers( m_hFile ) );
    }
}

/// @copydoc Stream::Seek()
int64_t FileStream::Seek( int64_t offset, ESeekOrigin origin )
{
    if( m_hFile == InvalidHandleValue )
    {
        HELIUM_ASSERT_MSG_FALSE( TXT( "File not open" ) );
        return -1;
    }

    if( static_cast< size_t >( origin ) >= static_cast< size_t >( SEEK_ORIGIN_MAX ) )
    {
        HELIUM_ASSERT_MSG_FALSE( TXT( "Invalid seek origin" ) );
        return -1;
    }

    LARGE_INTEGER moveDistance;
    moveDistance.QuadPart = offset;

    DWORD moveMethod =
        ( origin == SEEK_ORIGIN_CURRENT
        ? FILE_CURRENT
        : ( origin == SEEK_ORIGIN_BEGIN ? FILE_BEGIN : FILE_END ) );

    LARGE_INTEGER filePointer;
    filePointer.QuadPart = 0;

    BOOL bResult = SetFilePointerEx( m_hFile, moveDistance, &filePointer, moveMethod );
    HELIUM_ASSERT( bResult );

    return ( bResult ? filePointer.QuadPart : -1 );
}

/// @copydoc Stream::Tell()
int64_t FileStream::Tell() const
{
    if( m_hFile == InvalidHandleValue )
    {
        HELIUM_ASSERT_MSG_FALSE( TXT( "File not open" ) );
        return -1;
    }

    LARGE_INTEGER moveDistance;
    moveDistance.QuadPart = 0;

    LARGE_INTEGER filePointer;
    filePointer.QuadPart = 0;

    BOOL bResult = SetFilePointerEx( m_hFile, moveDistance, &filePointer, FILE_CURRENT );
    HELIUM_ASSERT( bResult );

    return ( bResult ? filePointer.QuadPart : -1 );
}

/// @copydoc Stream::GetSize()
int64_t FileStream::GetSize() const
{
    if( m_hFile == InvalidHandleValue )
    {
        HELIUM_ASSERT_MSG_FALSE( TXT( "File not open" ) );
        return -1;
    }

    LARGE_INTEGER fileSize;
    fileSize.QuadPart = 0;

    BOOL bResult = GetFileSizeEx( m_hFile, &fileSize );
    HELIUM_ASSERT( bResult );

    return ( bResult ? fileSize.QuadPart : -1 );
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

    DWORD desiredAccess = 0;
    if( modeFlags & MODE_READ )
    {
        desiredAccess |= GENERIC_READ;
    }

    if( modeFlags & MODE_WRITE )
    {
        desiredAccess |= GENERIC_WRITE;
    }

    // Allow other files to read if we are not writing to the file.
    DWORD shareMode = 0;
    if( !( modeFlags & MODE_WRITE ) )
    {
        shareMode |= FILE_SHARE_READ;
    }

    DWORD createDisposition = OPEN_EXISTING;
    if( modeFlags & MODE_WRITE )
    {
        createDisposition = ( bTruncate ? CREATE_ALWAYS : OPEN_ALWAYS );
    }

    m_hFile = CreateFile(
        pPath,
        desiredAccess,
        shareMode,
        NULL,
        createDisposition,
        FILE_ATTRIBUTE_NORMAL,
        NULL );

    if ( m_hFile == InvalidHandleValue )
    {
        return false;
    }

    m_modeFlags = modeFlags;
    return true;
}
