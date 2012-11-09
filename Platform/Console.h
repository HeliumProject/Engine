#pragma once

#include <stdarg.h>

#include "Platform/API.h"
#include "Platform/Types.h"

namespace Helium
{
	// Print to STDOUT
    HELIUM_PLATFORM_API int Print(const tchar_t* fmt, ...);
    HELIUM_PLATFORM_API int PrintArgs(const tchar_t* fmt, va_list args);

	// Print to C file handle
    HELIUM_PLATFORM_API int FilePrint(FILE* f, const tchar_t* fmt, ...);
    HELIUM_PLATFORM_API int FilePrintArgs(FILE* f, const tchar_t* fmt, va_list args);

	// Print to string
	HELIUM_PLATFORM_API int StringPrint(tchar_t* dest, size_t destCount, const tchar_t* fmt, ...);
	HELIUM_PLATFORM_API int StringPrintArgs(tchar_t* dest, size_t destCount, const tchar_t* fmt, va_list args);

	// Deduction of target array size
	template <size_t N>	HELIUM_FORCEINLINE int StringPrint( tchar_t (&dest)[N], const tchar_t* fmt, ... );
	template <size_t N>	HELIUM_FORCEINLINE int StringPrintArgs( tchar_t (&dest)[N], const tchar_t* fmt, va_list args );

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

    HELIUM_PLATFORM_API int Print(ConsoleColor color, FILE* stream, const tchar_t* fmt, ...);
    HELIUM_PLATFORM_API int PrintArgs(ConsoleColor color, FILE* stream, const tchar_t* fmt, va_list args);
    HELIUM_PLATFORM_API int PrintString(ConsoleColor color, FILE* stream, const tstring& tstring);
}

#include "Platform/Console.inl"