#include "PlatformPch.h"
#include "Platform/Stat.h"

#include "Platform/String.h"

#include <sys/stat.h>

using namespace Helium;

void CopyFromWindowsAttributes( DWORD attrs, uint32_t mode )
{
    mode |= ( attrs & FILE_ATTRIBUTE_READONLY ) ? FileModeFlags::Read : ( FileModeFlags::Read | FileModeFlags::Write );
    mode |= ( attrs & FILE_ATTRIBUTE_DIRECTORY ) ? FileModeFlags::Directory : FileModeFlags::None;
    mode |= ( attrs & FILE_ATTRIBUTE_REPARSE_POINT ) ? FileModeFlags::Link : FileModeFlags::None;
    mode |= ( attrs & FILE_ATTRIBUTE_DEVICE ) ? FileModeFlags::Special : FileModeFlags::None;
    mode |= ( attrs & FILE_ATTRIBUTE_SYSTEM ) ? FileModeFlags::Special : FileModeFlags::None;
}

// FILETIME is a 64-bit unsigned integer representing
// the number of 100-nanosecond intervals since January 1, 1601
// UNIX timestamp is number of seconds since January 1, 1970
// 116444736000000000 = 10000000 * 60 * 60 * 24 * 365 * 369 + 89 leap days
uint64_t FileTimeToUnixTime( FILETIME time )
{
	uint64_t ticks = ( (uint64_t)time.dwHighDateTime << 32 ) | time.dwLowDateTime;
	return (ticks - 116444736000000000) / 10000000;
}

bool Helium::StatPath( const tchar_t* path, Helium::Stat& stat )
{
	HELIUM_CONVERT_TO_NATIVE( path, convertedPath );

	WIN32_FILE_ATTRIBUTE_DATA fileStatus;
	memset( &fileStatus, 0, sizeof( fileStatus ) );
	bool result = ::GetFileAttributesEx( convertedPath, GetFileExInfoStandard, &fileStatus ) == TRUE;
	if ( result )
	{
		stat.m_Size = ( (uint64_t)fileStatus.nFileSizeHigh << 32 ) | fileStatus.nFileSizeLow;
		stat.m_CreatedTime = FileTimeToUnixTime( fileStatus.ftCreationTime );
		stat.m_ModifiedTime = FileTimeToUnixTime( fileStatus.ftLastWriteTime );
		stat.m_AccessTime = FileTimeToUnixTime( fileStatus.ftLastAccessTime );

		CopyFromWindowsAttributes( fileStatus.dwFileAttributes, stat.m_Mode );
	}

	return result;
}

