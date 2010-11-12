#pragma once

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
# ifdef PLATFORM_EXPORTS
#  define PLATFORM_API HELIUM_API_EXPORT
# else
#  define PLATFORM_API HELIUM_API_IMPORT
# endif
#else
# define PLATFORM_API
#endif
