#pragma once

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
# ifdef FOUNDATION_EXPORTS
#  define FOUNDATION_API HELIUM_API_EXPORT
# else
#  define FOUNDATION_API HELIUM_API_IMPORT
# endif
#else
#define FOUNDATION_API
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