#include "PlatformPch.h"
#include "Platform/File.h"

#include "Platform/Assert.h"
#include "Platform/Encoding.h"
#include "Platform/Types.h"

using namespace Helium;

File::File()
	: m_Handle( INVALID_HANDLE_VALUE )
{
}

File::~File()
{
	Close();
}

bool File::IsOpen() const
{
	return m_Handle != INVALID_HANDLE_VALUE;
}

bool File::Open( const tchar_t* filename, FileMode mode, bool truncate )
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

	HELIUM_CONVERT_TO_NATIVE( filename, convertedFilename );
    m_Handle = ::CreateFile( convertedFilename, desiredAccess, shareMode, NULL, createDisposition, FILE_ATTRIBUTE_NORMAL, NULL );
	return m_Handle != INVALID_HANDLE_VALUE;
}

bool File::Close()
{
    return 1 == ::CloseHandle( m_Handle );
}

bool File::Read( void* buffer, size_t numberOfBytesToRead, size_t* numberOfBytesRead )
{
    HELIUM_ASSERT_MSG( numberOfBytesToRead <= MAXDWORD, TXT( "File read operations are limited to DWORD sizes" ) );
    if( numberOfBytesToRead > MAXDWORD )
    {
        return false;
    }

    HELIUM_ASSERT( buffer || numberOfBytesToRead == 0 );

    DWORD tempBytesRead;
    bool result = 1 == ::ReadFile( m_Handle, buffer, static_cast< DWORD >( numberOfBytesToRead ), &tempBytesRead, NULL );
    if ( result && numberOfBytesRead )
    {
        *numberOfBytesRead = tempBytesRead;
    }
    return result;
}

bool File::Write( const void* buffer, size_t numberOfBytesToWrite, size_t* numberOfBytesWritten )
{
    HELIUM_ASSERT_MSG( numberOfBytesToWrite <= MAXDWORD, TXT( "File write operations are limited to DWORD sizes" ) );
    if( numberOfBytesToWrite > MAXDWORD )
    {
        return false;
    }

    HELIUM_ASSERT( buffer || numberOfBytesToWrite == 0 );

    DWORD tempBytesWritten;
    bool result = 1 == ::WriteFile( m_Handle, buffer, static_cast< DWORD >( numberOfBytesToWrite ), &tempBytesWritten, NULL );
    if ( result && numberOfBytesWritten )
    {
        *numberOfBytesWritten = tempBytesWritten;
    }
    return result;
}

bool File::Flush()
{
    return 1 == ::FlushFileBuffers( m_Handle );
}

int64_t File::Seek( int64_t offset, SeekOrigin origin )
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

    BOOL bResult = ::SetFilePointerEx( m_Handle, moveDistance, &filePointer, moveMethod );
    HELIUM_ASSERT( bResult );

    return ( bResult ? filePointer.QuadPart : -1 );
}

int64_t File::Tell() const
{
    LARGE_INTEGER moveDistance;
    moveDistance.QuadPart = 0;

    LARGE_INTEGER filePointer;
    filePointer.QuadPart = 0;

    BOOL bResult = ::SetFilePointerEx( m_Handle, moveDistance, &filePointer, FILE_CURRENT );
    HELIUM_ASSERT( bResult );

    return ( bResult ? filePointer.QuadPart : -1 );
}

int64_t File::GetSize() const
{
    LARGE_INTEGER fileSize;
    fileSize.QuadPart = 0;

    BOOL bResult = ::GetFileSizeEx( m_Handle, &fileSize );
    HELIUM_ASSERT( bResult );

    return ( bResult ? fileSize.QuadPart : -1 );
}