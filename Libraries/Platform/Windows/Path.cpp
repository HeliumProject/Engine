#include "Platform/Path.h"
#include "Platform/Types.h"
#include "Platform/Windows/Windows.h"

#include <sys/stat.h>

const static char Platform::PathSeparator = '\\';

bool Platform::GetFullPath( const char* path, std::string& fullPath )
{
    char* full = new char[ PLATFORM_PATH_MAX ];
    u32 result = ::GetFullPathName( path, PLATFORM_PATH_MAX, full, NULL );

    if ( result > PLATFORM_PATH_MAX )
    {
        delete full;
        full = new char[ result ];
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

bool Platform::IsAbsolute( const char* path )
{
    if ( strlen( path ) > 1 )
    {
        if ( path[ 1 ] == ':' )
            return true;

        if ( path[ 0 ] == '\\' && path[ 1 ] == '\\' )
            return true;
    }

    return false;
}

void SplitDirectories( const std::string& path, V_string& output )
{
    std::string::size_type start = 0; 
    std::string::size_type end = 0; 
    while ( ( end = path.find( Platform::PathSeparator, start ) ) != std::string::npos )
    { 
        output.push_back( path.substr( start, end - start ) ); 
        start = end + 1;
    }
    output.push_back( path.substr( start ) ); 
}


bool Platform::MakePath( const char* path )
{
    V_string directories;
    SplitDirectories( path, directories );

    struct stat statInfo;
    std::string currentDirectory;
    currentDirectory.reserve( PLATFORM_PATH_MAX );
    currentDirectory = directories[ 0 ];
    for( V_string::const_iterator itr = directories.begin() + 1, end = directories.end(); itr != end; ++itr )
    {
        if ( ( (*currentDirectory.rbegin()) != ':' ) && ( stat( currentDirectory.c_str(), &statInfo ) != 0 ) )
        {
            if ( !CreateDirectory( currentDirectory.c_str(), NULL ) )
            {
                return false;
            }
        }

        currentDirectory += std::string( "\\" ) + *itr;
    }
   
    return true;
}

bool Platform::Copy( const char* source, const char* dest, bool overwrite )
{
    return ( TRUE == ::CopyFile( source, dest, overwrite ? FALSE : TRUE ) );
}

bool Platform::Move( const char* source, const char* dest )
{
    return ( TRUE == ::MoveFile( source, dest ) );
}

bool Platform::Delete( const char* path )
{
    return ( TRUE == ::DeleteFile( path ) );
}