#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

#if HELIUM_OS_WIN
# define snprintf _snprintf
# define vsnprintf _vsnprintf
#endif

namespace Helium
{
    HELIUM_PLATFORM_API void Print(const tchar_t* fmt, ...);
}
