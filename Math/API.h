#pragma once

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
# ifdef HELIUM_MATH_EXPORTS
#  define HELIUM_MATH_API HELIUM_API_EXPORT
# else
#  define HELIUM_MATH_API HELIUM_API_IMPORT
# endif
#else
#define HELIUM_MATH_API
#endif

// profiling
//#define MATH_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (MATH_PROFILE)
# define MATH_FUNCTION_TIMER() PROFILE_FUNCTION_TIMER()
#else
# define MATH_FUNCTION_TIMER()
#endif

#if defined(PROFILE_INSTRUMENT_ALL) || defined (MATH_PROFILE)
# define MATH_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define MATH_SCOPE_TIMER(__Str)
#endif