#include "PlatformPch.h"
#include "Platform/Path.h"

#include "Platform/Encoding.h"

#include <vector>
#include <sys/stat.h>

const tchar_t Helium::PathSeparator = TXT('\\');

void Helium::GetFullPath( const tchar_t* path, tstring& fullPath )
{
	HELIUM_CONVERT_TO_NATIVE( path, convertedPath );
	DWORD fullPathNameCount = ::GetFullPathName( convertedPath, 0, NULL, NULL );
    wchar_t* fullPathName = (wchar_t*)alloca( sizeof(wchar_t) * fullPathNameCount );
    uint32_t result = ::GetFullPathName( convertedPath, MAX_PATH, fullPathName, NULL );

	HELIUM_CONVERT_TO_TCHAR( fullPathName, convertedFullPathName );
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
    currentDirectory.reserve( MAX_PATH );
    currentDirectory = directories[ 0 ];
    for( std::vector< tstring >::const_iterator itr = directories.begin() + 1, end = directories.end(); itr != end; ++itr )
    {
		HELIUM_CONVERT_TO_NATIVE( currentDirectory.c_str(), convertedCurrentDirectory );

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
	HELIUM_CONVERT_TO_NATIVE( source, convertedSource );
	HELIUM_CONVERT_TO_NATIVE( dest, convertedDest );
    return ( TRUE == ::CopyFile( convertedSource, convertedDest, overwrite ? FALSE : TRUE ) );
}

bool Helium::Move( const tchar_t* source, const tchar_t* dest )
{
	HELIUM_CONVERT_TO_NATIVE( source, convertedSource );
	HELIUM_CONVERT_TO_NATIVE( dest, convertedDest );
    return ( TRUE == ::MoveFile( convertedSource, convertedDest ) );
}

bool Helium::Delete( const tchar_t* path )
{
	HELIUM_CONVERT_TO_NATIVE( path, convertedPath );
    return ( TRUE == ::DeleteFile( convertedPath ) );
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