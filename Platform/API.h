#pragma once

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
# ifdef HELIUM_PLATFORM_EXPORTS
#  define HELIUM_PLATFORM_API HELIUM_API_EXPORT
# else
#  define HELIUM_PLATFORM_API HELIUM_API_IMPORT
# endif
#else
# define HELIUM_PLATFORM_API
#endif
