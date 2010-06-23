#pragma once

#include <string>
#include <stdlib.h>

#include "Platform/API.h"

namespace Platform
{
    inline bool ConvertChar( char src, char& dest )
    {
        dest = src;
        return true;
    }

    inline bool ConvertChar( wchar_t src, wchar_t& dest )
    {
        dest = src;
        return true;
    }

    PLATFORM_API bool ConvertChar( char src, wchar_t& dest );
    PLATFORM_API bool ConvertChar( wchar_t src, char& dest );

    inline bool ConvertString( const std::wstring& src, std::wstring& dest )
    {
        dest = src;
        return true;
    }

    inline bool ConvertString( const std::string& src, std::string& dest )
    {
        dest = src;
        return true;
    }

    PLATFORM_API bool ConvertString( const std::string& src, std::wstring& dest );
    PLATFORM_API bool ConvertString( const std::wstring& src, std::string& dest );
}