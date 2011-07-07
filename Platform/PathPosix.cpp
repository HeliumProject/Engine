#include "Platform/Path.h"

const tchar Helium::PathSeparator = '/';

bool Helium::GetFullPath( const tchar* path, tstring& fullPath )
{
    return false;
}

bool Helium::IsAbsolute( const tchar* path )
{
    return false;
}

bool Helium::MakePath( const tchar* path )
{
    return false;
}

bool Helium::Copy( const tchar* source, const tchar* dest, bool overwrite )
{
    return false;
}

bool Helium::Move( const tchar* source, const tchar* dest )
{
    return false;
}

bool Helium::Delete( const tchar* path )
{
    return false;
}

bool Helium::GetVersionInfo( const tchar* path, tstring& versionInfo )
{
    return false;
}