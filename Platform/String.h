#pragma once

#include <string>
#include <stdlib.h>

#include "Platform/API.h"
#include "Platform/Types.h"

namespace Helium
{
    inline bool ConvertString( const std::string& src, std::string& dest )
    {
        dest = src;
        return true;
    }

#ifndef __GNUC__
    inline bool ConvertString( const std::wstring& src, std::wstring& dest )
    {
        dest = src;
        return true;
    }

    HELIUM_PLATFORM_API bool ConvertString( const std::string& src, std::wstring& dest );
    HELIUM_PLATFORM_API bool ConvertString( const std::wstring& src, std::string& dest );
#endif

    HELIUM_PLATFORM_API tstring GetEncoding();
}
