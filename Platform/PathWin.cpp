#include "PlatformPch.h"
#include "Platform/Path.h"
#include "Platform/Types.h"

#include <vector>
#include <sys/stat.h>

const tchar_t Helium::PathSeparator = TXT('\\');

#pragma comment( lib, "Version.lib" )

#if HELIUM_UNICODE
#define _CREATE_DIRECTORY CreateDirectoryW
#else
#define _CREATE_DIRECTORY CreateDirectoryA
#endif

bool Helium::GetFullPath( const tchar_t* path, tstring& fullPath )
{
    tchar_t* full = new tchar_t[ PLATFORM_PATH_MAX ];
    uint32_t result = ::GetFullPathName( path, PLATFORM_PATH_MAX, full, NULL );

    if ( result > PLATFORM_PATH_MAX )
    {
        delete full;
        full = new tchar_t[ result ];
        result = ::GetFullPathName( path, result, full, NULL );
    }

    if ( result == 0 )
    {
        delete full;
        return false;
    }

    fullPath = full;
    delete full;
    return true;
}

bool Helium::IsAbsolute( const tchar_t* path )
{
    if ( path && _tcslen( path ) > 1 )
    {
        if ( path[ 1 ] == ':' )
            return true;

        if ( path[ 0 ] == '\\' && path[ 1 ] == '\\' )
            return true;
    }

    return false;
}

void SplitDirectories( const tstring& path, std::vector< tstring >& output )
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
    currentDirectory.reserve( PLATFORM_PATH_MAX );
    currentDirectory = directories[ 0 ];
    for( std::vector< tstring >::const_iterator itr = directories.begin() + 1, end = directories.end(); itr != end; ++itr )
    {
        if ( ( (*currentDirectory.rbegin()) != TXT(':') ) && ( _tstati64( currentDirectory.c_str(), &statInfo ) != 0 ) )
        {
            if ( !_CREATE_DIRECTORY( currentDirectory.c_str(), NULL ) )
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
    return ( TRUE == ::CopyFile( source, dest, overwrite ? FALSE : TRUE ) );
}

bool Helium::Move( const tchar_t* source, const tchar_t* dest )
{
    return ( TRUE == ::MoveFile( source, dest ) );
}

bool Helium::Delete( const tchar_t* path )
{
    return ( TRUE == ::DeleteFile( path ) );
}

bool GetTranslationId(LPVOID lpData, UINT unBlockSize, WORD wLangId, DWORD &dwId, bool bPrimaryEnough/*= FALSE*/)
{
    LPWORD lpwData;
    for (lpwData = (LPWORD)lpData; (LPBYTE)lpwData < ((LPBYTE)lpData)+unBlockSize; lpwData+=2)
    {
        if (*lpwData == wLangId)
        {
            dwId = *((DWORD*)lpwData);
            return TRUE;
        }
    }

    if (!bPrimaryEnough)
        return FALSE;

    for (lpwData = (LPWORD)lpData; (LPBYTE)lpwData < ((LPBYTE)lpData)+unBlockSize; lpwData+=2)
    {
        if (((*lpwData)&0x00FF) == (wLangId&0x00FF))
        {
            dwId = *((DWORD*)lpwData);
            return TRUE;
        }
    }

    return FALSE;
}

bool Helium::GetVersionInfo( const tchar_t* path, tstring& versionInfo )
{
    DWORD	dwHandle;
    DWORD fileDataSize = GetFileVersionInfoSize( ( LPTSTR ) path, ( LPDWORD ) &dwHandle );

    // file has no version info in this case
    if ( fileDataSize <= 0 )
        return false;

    LPVOID fileData = ( LPVOID ) malloc ( fileDataSize );
    if ( fileData == NULL )
    {
        return false;
    }

    // get the version info
    dwHandle = 0;
    if ( GetFileVersionInfo( ( LPTSTR ) path, dwHandle, fileDataSize, fileData ) )
    {
        // catch default information
        LPVOID lpInfo;
        UINT unInfoLen;
        VerQueryValue( fileData, TXT("\\"), &lpInfo, &unInfoLen );

        // find best matching language and codepage
        VerQueryValue( fileData, TXT("\\VarFile\\Translation"), &lpInfo, &unInfoLen );

        DWORD dwLangCode = 0;
        if ( !GetTranslationId(lpInfo, unInfoLen, GetUserDefaultLangID(), dwLangCode, FALSE ) )
        {
            if ( !GetTranslationId(lpInfo, unInfoLen, GetUserDefaultLangID(), dwLangCode, TRUE ) )
            {
                if ( !GetTranslationId(lpInfo, unInfoLen, MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ), dwLangCode, TRUE ) )
                {
                    if ( !GetTranslationId(lpInfo, unInfoLen, MAKELANGID( LANG_ENGLISH, SUBLANG_NEUTRAL ), dwLangCode, TRUE ) )
                    {
                        // use the first one we can get
                        dwLangCode = *( ( DWORD* ) lpInfo );
                    }
                }
            }
        }

        tchar_t key[64];
        _sntprintf(
            key,
            sizeof(key),
            TXT("\\StringFile\\%04X%04X\\FileVersion"),
            ( dwLangCode & 0x0000FFFF ),
            ( dwLangCode & 0xFFFF0000 ) >> 16 );

        if ( VerQueryValue( fileData, ( LPTSTR ) key, &lpInfo, &unInfoLen ) )
        {
            versionInfo = ( LPCTSTR ) lpInfo;
        }
    }

    free( fileData );
    return true;
}