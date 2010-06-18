#pragma once

#include "Platform/API.h"

#ifdef WIN32
# include "Platform/Windows/Path.h"
#else
# include "Platform/POSIX/Path.h"
#endif

#include <string>

namespace Platform
{
    PLATFORM_API const extern char PathSeparator;

    PLATFORM_API bool GetFullPath( const char* path, std::string& fullPath );
    PLATFORM_API bool IsAbsolute( const char* path );
    PLATFORM_API bool MakePath( const char* path );
    PLATFORM_API bool Copy( const char* source, const char* dest, bool overwrite );
    PLATFORM_API bool Move( const char* source, const char* dest );
    PLATFORM_API bool Delete( const char* path );
    PLATFORM_API bool GetVersionInfo( const char* path, std::string& versionInfo );
}