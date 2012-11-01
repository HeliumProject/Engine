#pragma once

#include <string>
#include <stdlib.h>

#include "Platform/API.h"
#include "Platform/Types.h"

namespace Helium
{
    inline bool ConvertString( const char* src, char* dest, size_t destCount )
    {
		size_t srcCount = strlen( src );
        memcpy( dest, src, ( srcCount < destCount ? srcCount : destCount ) * sizeof( *src ) );
        return true;
    }

	inline bool ConvertString( const std::string& src, std::string& dest )
    {
        dest = src;
        return true;
    }

    inline bool ConvertString( const wchar_t* src, wchar_t* dest, size_t destCount )
    {
		size_t srcCount = wcslen( src );
        memcpy( dest, src, ( srcCount < destCount ? srcCount : destCount ) * sizeof( *src ) );
        return true;
    }

    inline bool ConvertString( const std::wstring& src, std::wstring& dest )
    {
        dest = src;
        return true;
    }

    HELIUM_PLATFORM_API bool ConvertString( const char* src, wchar_t* dest, size_t destCount );
    HELIUM_PLATFORM_API bool ConvertString( const wchar_t* src, char* dest, size_t destCount );

	size_t GetConvertedStringLength( const char* src );
	size_t GetConvertedStringLength( const wchar_t* src );

    HELIUM_PLATFORM_API bool ConvertString( const std::string& src, std::wstring& dest );
    HELIUM_PLATFORM_API bool ConvertString( const std::wstring& src, std::string& dest );
}

#if HELIUM_WCHAR_T
# define HELIUM_CONVERT_TO_TCHAR( chars, convertedChars ) \
	const wchar_t* convertedChars = chars;
# define HELIUM_CONVERT_TO_NATIVE( chars, convertedChars ) \
	const wchar_t* convertedChars = chars;
#else
# define HELIUM_CONVERT_TO_TCHAR( chars, convertedChars ) \
	size_t convertedChars##Count = GetConvertedStringLength( chars ); \
	char* convertedChars = (char*)alloca( convertedChars##Count * sizeof( char ) ); \
	ConvertString( chars, convertedChars, convertedChars##Count );
# define HELIUM_CONVERT_TO_NATIVE( chars, convertedChars ) \
	size_t convertedChars##Count = GetConvertedStringLength( chars ); \
	wchar_t* convertedChars = (wchar_t*)alloca( convertedChars##Count * sizeof( wchar_t ) ); \
	ConvertString( chars, convertedChars, convertedChars##Count );
#endif