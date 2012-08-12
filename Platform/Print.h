#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

#include <stdarg.h>

namespace Helium
{
    HELIUM_PLATFORM_API int Print(const tchar_t* fmt, ...);
    HELIUM_PLATFORM_API int PrintArgs(const tchar_t* fmt, va_list args);

	HELIUM_PLATFORM_API int StringPrint(tchar_t* dest, size_t destCount, const tchar_t* fmt, ...);
	HELIUM_PLATFORM_API int StringPrintArgs(tchar_t* dest, size_t destCount, const tchar_t* fmt, va_list args);
}
