#pragma once

#include <stdarg.h>

#include "Platform/API.h"
#include "Platform/Types.h"

namespace Helium
{
	// Print to STDOUT
	HELIUM_PLATFORM_API int Print(const char* fmt, ...);
	HELIUM_PLATFORM_API int Print(const wchar_t* fmt, ...);
	HELIUM_PLATFORM_API int PrintArgs(const char* fmt, va_list args);
	HELIUM_PLATFORM_API int PrintArgs(const wchar_t* fmt, va_list args);

	// Print to C file handle
	HELIUM_PLATFORM_API int FilePrint(FILE* f, const char* fmt, ...);
	HELIUM_PLATFORM_API int FilePrint(FILE* f, const wchar_t* fmt, ...);
	HELIUM_PLATFORM_API int FilePrintArgs(FILE* f, const char* fmt, va_list args);
	HELIUM_PLATFORM_API int FilePrintArgs(FILE* f, const wchar_t* fmt, va_list args);

	// Print to string
	HELIUM_PLATFORM_API int StringPrint(char* dest, size_t destCount, const char* fmt, ...);
	HELIUM_PLATFORM_API int StringPrint(wchar_t* dest, size_t destCount, const wchar_t* fmt, ...);
	HELIUM_PLATFORM_API int StringPrintArgs(char* dest, size_t destCount, const char* fmt, va_list args);
	HELIUM_PLATFORM_API int StringPrintArgs(wchar_t* dest, size_t destCount, const wchar_t* fmt, va_list args);

	// Deduction of target array size
	template <size_t N>	HELIUM_FORCEINLINE int StringPrint( char (&dest)[N], const char* fmt, ... );
	template <size_t N>	HELIUM_FORCEINLINE int StringPrint( wchar_t (&dest)[N], const wchar_t* fmt, ... );
	template <size_t N>	HELIUM_FORCEINLINE int StringPrintArgs( char (&dest)[N], const char* fmt, va_list args );
	template <size_t N>	HELIUM_FORCEINLINE int StringPrintArgs( wchar_t (&dest)[N], const wchar_t* fmt, va_list args );

	namespace ConsoleColors
	{
		enum ConsoleColor
		{
			None,
			Red,
			Green,
			Blue,
			Yellow,
			Aqua,
			Purple,
			White,
		};
	}
	typedef ConsoleColors::ConsoleColor ConsoleColor;    

	HELIUM_PLATFORM_API int Print(ConsoleColor color, FILE* stream, const char* fmt, ...);
	HELIUM_PLATFORM_API int Print(ConsoleColor color, FILE* stream, const wchar_t* fmt, ...);
	HELIUM_PLATFORM_API int PrintArgs(ConsoleColor color, FILE* stream, const char* fmt, va_list args);
	HELIUM_PLATFORM_API int PrintArgs(ConsoleColor color, FILE* stream, const wchar_t* fmt, va_list args);
	HELIUM_PLATFORM_API int PrintString(ConsoleColor color, FILE* stream, const std::string& string);
	HELIUM_PLATFORM_API int PrintString(ConsoleColor color, FILE* stream, const std::wstring& string);
}

#include "Platform/Console.inl"