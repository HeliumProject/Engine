#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

#ifdef WIN32
# include "Platform/Windows/Path.h"
#else
# include "Platform/POSIX/Path.h"
#endif

#include <string>

namespace Helium
{
    PLATFORM_API extern const tchar PathSeparator;

    PLATFORM_API bool GetFullPath( const tchar* path, tstring& fullPath );
    PLATFORM_API bool IsAbsolute( const tchar* path );
    PLATFORM_API bool MakePath( const tchar* path );
    PLATFORM_API bool Copy( const tchar* source, const tchar* dest, bool overwrite );
    PLATFORM_API bool Move( const tchar* source, const tchar* dest );
    PLATFORM_API bool Delete( const tchar* path );
    PLATFORM_API bool GetVersionInfo( const tchar* path, tstring& versionInfo );
}