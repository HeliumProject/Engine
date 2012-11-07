#include "PlatformPch.h"
#include "Platform/Status.h"

#include "Platform/Encoding.h"

#include <sys/stat.h>

using namespace Helium;

void CopyFromWindowsAttributes( DWORD attrs, uint32_t mode )
{
    mode |= ( attrs & FILE_ATTRIBUTE_READONLY ) ? StatusModes::Read : ( StatusModes::Read | StatusModes::Write );
    mode |= ( attrs & FILE_ATTRIBUTE_DIRECTORY ) ? StatusModes::Directory : StatusModes::None;
    mode |= ( attrs & FILE_ATTRIBUTE_REPARSE_POINT ) ? StatusModes::Link : StatusModes::None;
    mode |= ( attrs & FILE_ATTRIBUTE_DEVICE ) ? StatusModes::Special : StatusModes::None;
    mode |= ( attrs & FILE_ATTRIBUTE_SYSTEM ) ? StatusModes::Special : StatusModes::None;
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

Status::Status()
: m_Mode( 0 )
, m_Size( 0 )
, m_CreatedTime( 0 )
, m_ModifiedTime( 0 )
, m_AccessTime( 0 )
{

}

bool Status::Read( const tchar_t* path )
{
	HELIUM_CONVERT_TO_NATIVE( path, convertedPath );

	WIN32_FILE_ATTRIBUTE_DATA fileStatus;
	memset( &fileStatus, 0, sizeof( fileStatus ) );
	bool result = ::GetFileAttributesEx( convertedPath, GetFileExInfoStandard, &fileStatus ) == TRUE;
	if ( result )
	{
		m_Size = ( (uint64_t)fileStatus.nFileSizeHigh << 32 ) | fileStatus.nFileSizeLow;
		m_CreatedTime = FileTimeToUnixTime( fileStatus.ftCreationTime );
		m_ModifiedTime = FileTimeToUnixTime( fileStatus.ftLastWriteTime );
		m_AccessTime = FileTimeToUnixTime( fileStatus.ftLastAccessTime );

		CopyFromWindowsAttributes( fileStatus.dwFileAttributes, m_Mode );
	}

	return result;
}

