#include "PlatformPch.h"
#include "Platform/File.h"

#include "Platform/Assert.h"
#include "Platform/Encoding.h"
#include "Platform/Types.h"

#include <vector>
#include <sys/stat.h>

using namespace Helium;

static uint64_t FromWindowsTime( FILETIME time )
{
	// FILETIME is a 64-bit unsigned integer representing
	// the number of 100-nanosecond intervals since January 1, 1601
	// UNIX timestamp is number of seconds since January 1, 1970
	// 116444736000000000 = 10000000 * 60 * 60 * 24 * 365 * 369 + 89 leap days
	uint64_t ticks = ( (uint64_t)time.dwHighDateTime << 32 ) | time.dwLowDateTime;
	return (ticks - 116444736000000000) / 10000000;
}

static void FromWindowsAttributes( DWORD attrs, uint32_t mode )
{
	mode |= ( attrs & FILE_ATTRIBUTE_READONLY ) ? StatusModes::Read : ( StatusModes::Read | StatusModes::Write );
	mode |= ( attrs & FILE_ATTRIBUTE_DIRECTORY ) ? StatusModes::Directory : StatusModes::None;
	mode |= ( attrs & FILE_ATTRIBUTE_REPARSE_POINT ) ? StatusModes::Link : StatusModes::None;
	mode |= ( attrs & FILE_ATTRIBUTE_DEVICE ) ? StatusModes::Special : StatusModes::None;
	mode |= ( attrs & FILE_ATTRIBUTE_SYSTEM ) ? StatusModes::Special : StatusModes::None;
}

static void FromWindowsFindData( const WIN32_FIND_DATA& windowsFile, DirectoryEntry& ourFile )
{
	ConvertString( windowsFile.cFileName, ourFile.m_Name );

	ourFile.m_Stat.m_Size = ( (uint64_t)windowsFile.nFileSizeHigh << 32 ) | windowsFile.nFileSizeLow;
	ourFile.m_Stat.m_CreatedTime = FromWindowsTime( windowsFile.ftCreationTime );
	ourFile.m_Stat.m_ModifiedTime = FromWindowsTime( windowsFile.ftLastWriteTime );
	ourFile.m_Stat.m_AccessTime = FromWindowsTime( windowsFile.ftLastAccessTime );

	FromWindowsAttributes( windowsFile.dwFileAttributes, ourFile.m_Stat.m_Mode );
}

//
// File contents
//

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

	HELIUM_TCHAR_TO_WIDE( filename, convertedFilename );
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

//
// File stats
//

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
	HELIUM_TCHAR_TO_WIDE( path, convertedPath );

	WIN32_FILE_ATTRIBUTE_DATA fileStatus;
	memset( &fileStatus, 0, sizeof( fileStatus ) );
	bool result = ::GetFileAttributesEx( convertedPath, GetFileExInfoStandard, &fileStatus ) == TRUE;
	if ( result )
	{
		m_Size = ( (uint64_t)fileStatus.nFileSizeHigh << 32 ) | fileStatus.nFileSizeLow;
		m_CreatedTime = FromWindowsTime( fileStatus.ftCreationTime );
		m_ModifiedTime = FromWindowsTime( fileStatus.ftLastWriteTime );
		m_AccessTime = FromWindowsTime( fileStatus.ftLastAccessTime );

		FromWindowsAttributes( fileStatus.dwFileAttributes, m_Mode );
	}

	return result;
}

//
// Directory info
//

DirectoryEntry::DirectoryEntry( const tstring& name )
	: m_Name( name )
{
}

Directory::Directory( const tstring& path )
	: m_Path( path )
	, m_Handle( INVALID_HANDLE_VALUE )
{
}

Directory::~Directory()
{
	Close();
}

bool Directory::IsOpen()
{
	return m_Handle != INVALID_HANDLE_VALUE;
}

bool Directory::FindFirst( DirectoryEntry& entry )
{
	Close();

	tstring path ( m_Path + TXT( "/*" ) );
	HELIUM_TCHAR_TO_WIDE( path.c_str(), convertedPath );

	WIN32_FIND_DATA foundFile;
	m_Handle = ::FindFirstFile( convertedPath, &foundFile );
	if ( m_Handle == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	FromWindowsFindData( foundFile, entry );
	return true;
}

bool Directory::FindNext( DirectoryEntry& entry )
{
	WIN32_FIND_DATA foundFile;
	if ( !::FindNextFile( m_Handle, &foundFile ) )
	{
		return false;
	}

	FromWindowsFindData( foundFile, entry );
	return true;
}

bool Directory::Close()
{
	if ( IsOpen() && ::FindClose( m_Handle ) == 0 )
	{
		return false;
	}

	return true;
}

//
// File system ops
//

const tchar_t Helium::PathSeparator = TXT('\\');

void Helium::GetFullPath( const tchar_t* path, tstring& fullPath )
{
	HELIUM_TCHAR_TO_WIDE( path, convertedPath );
	DWORD fullPathNameCount = ::GetFullPathName( convertedPath, 0, NULL, NULL );
	wchar_t* fullPathName = (wchar_t*)alloca( sizeof(wchar_t) * fullPathNameCount );
	uint32_t result = ::GetFullPathName( convertedPath, MAX_PATH, fullPathName, NULL );

	HELIUM_WIDE_TO_TCHAR( fullPathName, convertedFullPathName );
	fullPath = convertedFullPathName;
}

bool Helium::IsAbsolute( const tchar_t* path )
{
	if ( path && path[0] != '\0' && path[1] != '\0' )
	{
		if ( path[ 1 ] == ':' )
		{
			return true;
		}

		if ( path[ 0 ] == '\\' && path[ 1 ] == '\\' )
		{
			return true;
		}
	}

	return false;
}

static void SplitDirectories( const tstring& path, std::vector< tstring >& output )
{
	tstring::size_type start = 0; 
	tstring::size_type end = 0; 
	while ( ( end = path.find( Helium::PathSeparator, start ) ) != tstring::npos )
	{ 
		output.push_back( path.substr( start, end - start ) ); 
		start = end + 1;
	}
	output.push_back( path.substr( start ) ); 
}

bool Helium::MakePath( const tchar_t* path )
{
	std::vector< tstring > directories;
	SplitDirectories( path, directories );

	struct _stati64 statInfo;
	tstring currentDirectory;
	currentDirectory.reserve( MAX_PATH );
	currentDirectory = directories[ 0 ];
	for( std::vector< tstring >::const_iterator itr = directories.begin() + 1, end = directories.end(); itr != end; ++itr )
	{
		HELIUM_TCHAR_TO_WIDE( currentDirectory.c_str(), convertedCurrentDirectory );

		if ( ( (*currentDirectory.rbegin()) != TXT(':') ) && ( _wstat64( convertedCurrentDirectory, &statInfo ) != 0 ) )
		{
			if ( !CreateDirectory( convertedCurrentDirectory, NULL ) )
			{
				return false;
			}
		}

		currentDirectory += tstring( TXT("\\") ) + *itr;
	}

	return true;
}

bool Helium::Copy( const tchar_t* source, const tchar_t* dest, bool overwrite )
{
	HELIUM_TCHAR_TO_WIDE( source, convertedSource );
	HELIUM_TCHAR_TO_WIDE( dest, convertedDest );
	return ( TRUE == ::CopyFile( convertedSource, convertedDest, overwrite ? FALSE : TRUE ) );
}

bool Helium::Move( const tchar_t* source, const tchar_t* dest )
{
	HELIUM_TCHAR_TO_WIDE( source, convertedSource );
	HELIUM_TCHAR_TO_WIDE( dest, convertedDest );
	return ( TRUE == ::MoveFile( convertedSource, convertedDest ) );
}

bool Helium::Delete( const tchar_t* path )
{
	HELIUM_TCHAR_TO_WIDE( path, convertedPath );
	return ( TRUE == ::DeleteFile( convertedPath ) );
}