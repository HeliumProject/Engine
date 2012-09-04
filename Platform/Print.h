#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

#include <stdarg.h>

namespace Helium
{
    HELIUM_PLATFORM_API int Print(const tchar_t* fmt, ...);
    HELIUM_PLATFORM_API int PrintArgs(const tchar_t* fmt, va_list args);

    HELIUM_PLATFORM_API int FilePrint(FILE* f, const tchar_t* fmt, ...);
    HELIUM_PLATFORM_API int FilePrintArgs(FILE* f, const tchar_t* fmt, va_list args);

	HELIUM_PLATFORM_API int StringPrint(tchar_t* dest, size_t destCount, const tchar_t* fmt, ...);
	HELIUM_PLATFORM_API int StringPrintArgs(tchar_t* dest, size_t destCount, const tchar_t* fmt, va_list args);

	template <size_t N>
	inline int StringPrint( tchar_t (&dest)[N], const tchar_t* fmt, ... )
	{
		va_list args;
		va_start(args, fmt);
		int result = StringPrint( dest, N, fmt, args );
	    va_end(args);
		return result;
	}

	template <size_t N>
	inline int StringPrintArgs( tchar_t (&dest)[N], const tchar_t* fmt, va_list args )
	{
		return StringPrint( dest, N, fmt, args );
	}
}
