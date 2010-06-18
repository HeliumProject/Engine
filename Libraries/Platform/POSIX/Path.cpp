#include "Platform/Path.h"

const char Platform::PathSeparator = '/';

bool Platform::GetFullPath( const char* path, std::string& fullPath )
{
    return false;
}

bool Platform::IsAbsolute( const char* path )
{
    return false;
}

bool Platform::MakePath( const char* path )
{
    return false;
}

bool Platform::Copy( const char* source, const char* dest, bool overwrite )
{
    return false;
}

bool Platform::Move( const char* source, const char* dest )
{
    return false;
}

bool Platform::Delete( const char* path )
{
    return false;
}

bool Platform::GetVersionInfo( const char* path, std::string& versionInfo )
{
    return false;
}