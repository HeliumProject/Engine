#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

#ifdef WIN32
# include "Platform/PathWin.h"
#else
# include "Platform/POSIX/Path.h"
#endif

#include <string>

namespace Helium
{
    PLATFORM_API extern const tchar_t PathSeparator;

    PLATFORM_API bool GetFullPath( const tchar_t* path, tstring& fullPath );
    PLATFORM_API bool IsAbsolute( const tchar_t* path );
    PLATFORM_API bool MakePath( const tchar_t* path );
    PLATFORM_API bool Copy( const tchar_t* source, const tchar_t* dest, bool overwrite );
    PLATFORM_API bool Move( const tchar_t* source, const tchar_t* dest );
    PLATFORM_API bool Delete( const tchar_t* path );
    PLATFORM_API bool GetVersionInfo( const tchar_t* path, tstring& versionInfo );
}