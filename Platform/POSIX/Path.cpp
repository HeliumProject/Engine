#include "Platform/Path.h"

const tchar Platform::PathSeparator = '/';

bool Platform::GetFullPath( const tchar* path, tstring& fullPath )
{
    return false;
}

bool Platform::IsAbsolute( const tchar* path )
{
    return false;
}

bool Platform::MakePath( const tchar* path )
{
    return false;
}

bool Platform::Copy( const tchar* source, const tchar* dest, bool overwrite )
{
    return false;
}

bool Platform::Move( const tchar* source, const tchar* dest )
{
    return false;
}

bool Platform::Delete( const tchar* path )
{
    return false;
}

bool Platform::GetVersionInfo( const tchar* path, tstring& versionInfo )
{
    return false;
}