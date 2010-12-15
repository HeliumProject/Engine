#pragma once

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.
//#include "Foundation/Profile.h"

#if HELIUM_SHARED
# ifdef FOUNDATION_EXPORTS
#  define FOUNDATION_API __declspec(dllexport)
# else
#  define FOUNDATION_API __declspec(dllimport)
# endif
#else
#define FOUNDATION_API
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