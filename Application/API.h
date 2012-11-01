#pragma once

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
# ifdef HELIUM_APPLICATION_EXPORTS
#  define HELIUM_APPLICATION_API HELIUM_API_EXPORT
# else
#  define HELIUM_APPLICATION_API HELIUM_API_IMPORT
# endif
#else
# define HELIUM_APPLICATION_API
#endif

// profiling
//#define FOUNDATION_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (FOUNDATION_PROFILE)
# define APPLICATION_FUNCTION_TIMER() PROFILE_FUNCTION_TIMER()
#else
# define APPLICATION_FUNCTION_TIMER()
#endif

#if defined(PROFILE_INSTRUMENT_ALL) || defined (FOUNDATION_PROFILE)
# define APPLICATION_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define APPLICATION_SCOPE_TIMER(__Str)
#endif
