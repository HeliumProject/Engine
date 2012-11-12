#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

namespace Helium
{
	HELIUM_PLATFORM_API inline bool ConvertString( const char* src, char* dest, size_t destCount );
	HELIUM_PLATFORM_API inline bool ConvertString( const std::string& src, std::string& dest );
	HELIUM_PLATFORM_API inline bool ConvertString( const wchar_t* src, wchar_t* dest, size_t destCount );
	HELIUM_PLATFORM_API inline bool ConvertString( const std::wstring& src, std::wstring& dest );

	HELIUM_PLATFORM_API bool ConvertString( const char* src, wchar_t* dest, size_t destCount );
	HELIUM_PLATFORM_API bool ConvertString( const wchar_t* src, char* dest, size_t destCount );

	HELIUM_PLATFORM_API size_t GetConvertedStringLength( const char* src );
	HELIUM_PLATFORM_API size_t GetConvertedStringLength( const wchar_t* src );

	HELIUM_PLATFORM_API bool ConvertString( const std::string& src, std::wstring& dest );
	HELIUM_PLATFORM_API bool ConvertString( const std::wstring& src, std::string& dest );
}

#define HELIUM_CONVERT_TO_CHAR( chars, convertedChars ) \
	char* convertedChars = NULL; \
	if ( chars ) \
	{ \
		size_t convertedChars##Count = Helium::GetConvertedStringLength( chars ); \
		convertedChars = (char*)alloca( convertedChars##Count * sizeof( char ) ); \
		Helium::ConvertString( chars, convertedChars, convertedChars##Count ); \
	}
#define HELIUM_CONVERT_TO_WIDE( chars, convertedChars ) \
	wchar_t* convertedChars = NULL; \
	if ( chars ) \
	{ \
		size_t convertedChars##Count = Helium::GetConvertedStringLength( chars ); \
		convertedChars = (wchar_t*)alloca( convertedChars##Count * sizeof( wchar_t ) ); \
		Helium::ConvertString( chars, convertedChars, convertedChars##Count ); \
	}

#if HELIUM_WCHAR_T
# define HELIUM_TCHAR_TO_CHAR( chars, convertedChars ) HELIUM_CONVERT_TO_CHAR( chars, convertedChars )
# define HELIUM_CHAR_TO_TCHAR( chars, convertedChars ) HELIUM_CONVERT_TO_WIDE( chars, convertedChars )
# define HELIUM_TCHAR_TO_WIDE( chars, convertedChars ) const wchar_t* convertedChars = chars;
# define HELIUM_WIDE_TO_TCHAR( chars, convertedChars ) const wchar_t* convertedChars = chars;
#else
# define HELIUM_TCHAR_TO_CHAR( chars, convertedChars ) const wchar_t* convertedChars = chars;
# define HELIUM_CHAR_TO_TCHAR( chars, convertedChars ) const wchar_t* convertedChars = chars;
# define HELIUM_TCHAR_TO_WIDE( chars, convertedChars ) HELIUM_CONVERT_TO_WIDE( chars, convertedChars )
# define HELIUM_WIDE_TO_TCHAR( chars, convertedChars ) HELIUM_CONVERT_TO_CHAR( chars, convertedChars )
#endif

#include "Platform/Encoding.inl"