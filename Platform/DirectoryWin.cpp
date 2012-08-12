#include "PlatformPch.h"
#include "Platform/Directory.h"

#include "Platform/Assert.h"
#include "Platform/Error.h"
#include "Platform/Exception.h"
#include "Platform/StatWin.h"
#include "Platform/String.h"

using namespace Helium;

void CopyFromWindowsStruct( const WIN32_FIND_DATA& windowsFile, FileFindData& ourFile )
{
    ConvertString( windowsFile.cFileName, ourFile.m_Filename );

    ourFile.m_Stat.m_Size = ( (uint64_t)windowsFile.nFileSizeHigh << 32 ) | windowsFile.nFileSizeLow;
    ourFile.m_Stat.m_CreatedTime = FileTimeToUnixTime( windowsFile.ftCreationTime );
    ourFile.m_Stat.m_ModifiedTime = FileTimeToUnixTime( windowsFile.ftLastWriteTime );
    ourFile.m_Stat.m_AccessTime = FileTimeToUnixTime( windowsFile.ftLastAccessTime );

	CopyFromWindowsAttributes( windowsFile.dwFileAttributes, ourFile.m_Stat.m_Mode );
}

bool Helium::FindFirst( DirectoryHandle& handle, FileFindData& data )
{
	tstring path ( handle.m_Path + TXT( "/*" ) );
	HELIUM_CONVERT_TO_WCHAR_T( path.c_str(), convertedPath );

    WIN32_FIND_DATA foundFile;
    handle.m_Handle = ::FindFirstFile( convertedPath, &foundFile );

    if ( handle.m_Handle == INVALID_HANDLE_VALUE )
    {
        DWORD error = GetLastError();
        
        //pmd020611 - Added ERROR_ACCESS_DENIED
        if ( error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND || error == ERROR_ACCESS_DENIED ) 
        {
            return false;
        }
        else
        {
            throw Exception( TXT( "Error calling ::FindFirstFile: %s" ), GetErrorString( error ).c_str() );
        }
    }

    CopyFromWindowsStruct( foundFile, data );
    return true;
}

bool Helium::FindNext( DirectoryHandle& handle, FileFindData& data )
{
    WIN32_FIND_DATA foundFile;
    if ( !::FindNextFile( handle.m_Handle, &foundFile ) )
    {
        DWORD error = GetLastError();
        if ( error != ERROR_NO_MORE_FILES ) 
        {
            throw Exception( TXT( "Error calling ::FindNextFile: %s" ), GetErrorString( error ).c_str() );
        }

        return false;
    }

    CopyFromWindowsStruct( foundFile, data );
    return true;
}

bool Helium::CloseFind( DirectoryHandle& handle )
{
    if ( ::FindClose( handle.m_Handle ) == 0 )
    {
        DWORD error = GetLastError();
        throw Exception( TXT( "Error calling ::FindClose: %s" ), GetErrorString( error ).c_str() );

        return false;
    }
    return true;
}

bool Helium::GetExtendedData( DirectoryHandle& handle, FileFindData& data )
{
#ifdef _DEBUG
    if ( !Helium::StatPath( tstring( handle.m_Path + TXT( "/" ) + data.m_Filename.c_str() ).c_str(), data.m_Stat ) )
    {
        return false;
    }

#endif

    return true;
}