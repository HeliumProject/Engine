#pragma once

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
# ifdef HELIUM_FOUNDATION_EXPORTS
#  define HELIUM_FOUNDATION_API HELIUM_API_EXPORT
# else
#  define HELIUM_FOUNDATION_API HELIUM_API_IMPORT
# endif
#else
#define HELIUM_FOUNDATION_API
#endif

// profiling
//#define FOUNDATION_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (FOUNDATION_PROFILE)
# define FOUNDATION_FUNCTION_TIMER() PROFILE_FUNCTION_TIMER()
#else
# define FOUNDATION_FUNCTION_TIMER()
#endif

#if defined(PROFILE_INSTRUMENT_ALL) || defined (FOUNDATION_PROFILE)
# define FOUNDATION_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define FOUNDATION_SCOPE_TIMER(__Str)
#endif