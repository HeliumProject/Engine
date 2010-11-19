#include "Platform/File.h"

#include "Platform/Assert.h"
#include "Platform/Windows/Windows.h"

using namespace Helium;

// i hate windows
#undef CreateFile

Handle Helium::CreateFile( const tchar_t* filename, FileMode mode, bool truncate )
{
    DWORD desiredAccess = 0;
    if( mode & FileModes::MODE_READ )
    {
        desiredAccess |= GENERIC_READ;
    }

    if( mode & FileModes::MODE_WRITE )
    {
        desiredAccess |= GENERIC_WRITE;
    }

    // Allow other files to read if we are not writing to the file.
    DWORD shareMode = 0;
    if( !( mode & FileModes::MODE_WRITE ) )
    {
        shareMode |= FILE_SHARE_READ;
    }

    DWORD createDisposition = OPEN_EXISTING;
    if( mode & FileModes::MODE_WRITE )
    {
        createDisposition = ( truncate ? CREATE_ALWAYS : OPEN_ALWAYS );
    }

    Handle handle = InvalidHandleValue;
#ifdef HELIUM_UNICODE
    handle = CreateFileW(
#else
    handle = CreateFileA(
#endif
        filename,
        desiredAccess,
        shareMode,
        NULL,
        createDisposition,
        FILE_ATTRIBUTE_NORMAL,
        NULL );

    return handle;
}

bool Helium::CloseHandle( Handle& handle )
{
    return 1 == ::CloseHandle( (HANDLE)handle );
}

bool Helium::ReadFile( Handle& handle, void* buffer, size_t numberOfBytesToRead, size_t* numberOfBytesRead )
{
    HELIUM_ASSERT_MSG( numberOfBytesToRead <= MAXDWORD, TXT( "File read operations are limited to DWORD sizes" ) );
    if( numberOfBytesToRead > MAXDWORD )
    {
        return false;
    }

    HELIUM_ASSERT( buffer || numberOfBytesToRead == 0 );

    DWORD tempBytesRead;
    bool result = 1 == ::ReadFile( handle, buffer, static_cast< DWORD >( numberOfBytesToRead ), &tempBytesRead, NULL );
    if ( result && numberOfBytesRead )
    {
        *numberOfBytesRead = tempBytesRead;
    }
    return result;
}

bool Helium::WriteFile( Handle& handle, const void* buffer, size_t numberOfBytesToWrite, size_t* numberOfBytesWritten )
{
    HELIUM_ASSERT_MSG( numberOfBytesToWrite <= MAXDWORD, TXT( "File write operations are limited to DWORD sizes" ) );
    if( numberOfBytesToWrite > MAXDWORD )
    {
        return false;
    }

    HELIUM_ASSERT( buffer || numberOfBytesToWrite == 0 );

    DWORD tempBytesWritten;
    bool result = 1 == ::WriteFile( handle, buffer, static_cast< DWORD >( numberOfBytesToWrite ), &tempBytesWritten, NULL );
    if ( result && numberOfBytesWritten )
    {
        *numberOfBytesWritten = tempBytesWritten;
    }
    return result;
}

bool Helium::FlushFile( Handle& handle )
{
    return 1 == ::FlushFileBuffers( handle );
}

int64_t Helium::Seek( Handle& handle, int64_t offset, SeekOrigin origin )
{
    HELIUM_ASSERT_MSG( static_cast< size_t >( origin ) <= static_cast< size_t >( SeekOrigins::SEEK_ORIGIN_MAX ), TXT( "Invalid seek origin" ) );

    LARGE_INTEGER moveDistance;
    moveDistance.QuadPart = offset;

    DWORD moveMethod =
        ( origin == SeekOrigins::SEEK_ORIGIN_CURRENT
        ? FILE_CURRENT
        : ( origin == SeekOrigins::SEEK_ORIGIN_BEGIN ? FILE_BEGIN : FILE_END ) );

    LARGE_INTEGER filePointer;
    filePointer.QuadPart = 0;

    BOOL bResult = ::SetFilePointerEx( handle, moveDistance, &filePointer, moveMethod );
    HELIUM_ASSERT( bResult );

    return ( bResult ? filePointer.QuadPart : -1 );
}

int64_t Helium::Tell( const Handle& handle )
{
    LARGE_INTEGER moveDistance;
    moveDistance.QuadPart = 0;

    LARGE_INTEGER filePointer;
    filePointer.QuadPart = 0;

    BOOL bResult = ::SetFilePointerEx( handle, moveDistance, &filePointer, FILE_CURRENT );
    HELIUM_ASSERT( bResult );

    return ( bResult ? filePointer.QuadPart : -1 );
}

int64_t Helium::GetSize( const Handle& handle )
{
    LARGE_INTEGER fileSize;
    fileSize.QuadPart = 0;

    BOOL bResult = ::GetFileSizeEx( handle, &fileSize );
    HELIUM_ASSERT( bResult );

    return ( bResult ? fileSize.QuadPart : -1 );
}