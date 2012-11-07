#include "PlatformPch.h"
#include "Platform/Directory.h"

#include "Platform/Assert.h"
#include "Platform/Error.h"
#include "Platform/Exception.h"
#include "Platform/StatusWin.h"
#include "Platform/Encoding.h"

using namespace Helium;

void CopyFromWindowsStruct( const WIN32_FIND_DATA& windowsFile, DirectoryEntry& ourFile )
{
    ConvertString( windowsFile.cFileName, ourFile.m_Name );

    ourFile.m_Stat.m_Size = ( (uint64_t)windowsFile.nFileSizeHigh << 32 ) | windowsFile.nFileSizeLow;
    ourFile.m_Stat.m_CreatedTime = FileTimeToUnixTime( windowsFile.ftCreationTime );
    ourFile.m_Stat.m_ModifiedTime = FileTimeToUnixTime( windowsFile.ftLastWriteTime );
    ourFile.m_Stat.m_AccessTime = FileTimeToUnixTime( windowsFile.ftLastAccessTime );

	CopyFromWindowsAttributes( windowsFile.dwFileAttributes, ourFile.m_Stat.m_Mode );
}

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
	HELIUM_CONVERT_TO_NATIVE( path.c_str(), convertedPath );

    WIN32_FIND_DATA foundFile;
    m_Handle = ::FindFirstFile( convertedPath, &foundFile );

    if ( m_Handle == INVALID_HANDLE_VALUE )
    {
        DWORD error = GetLastError();
        if ( error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND || error == ERROR_ACCESS_DENIED ) 
        {
            return false;
        }
        else
        {
            throw Exception( TXT( "Error calling ::FindFirstFile: %s" ), GetErrorString( error ).c_str() );
        }
    }

    CopyFromWindowsStruct( foundFile, entry );
    return true;
}

bool Directory::FindNext( DirectoryEntry& entry )
{
    WIN32_FIND_DATA foundFile;
    if ( !::FindNextFile( m_Handle, &foundFile ) )
    {
        DWORD error = GetLastError();
        if ( error != ERROR_NO_MORE_FILES ) 
        {
            throw Exception( TXT( "Error calling ::FindNextFile: %s" ), GetErrorString( error ).c_str() );
        }

        return false;
    }

    CopyFromWindowsStruct( foundFile, entry );
    return true;
}

bool Directory::Close()
{
    if ( ::FindClose( m_Handle ) == 0 )
    {
        DWORD error = GetLastError();
        throw Exception( TXT( "Error calling ::FindClose: %s" ), GetErrorString( error ).c_str() );
        return false;
    }

    return true;
}