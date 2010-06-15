#pragma once

//#include "Foundation/Profile.h"

#ifdef NOCTURNAL_STATIC
#define FOUNDATION_API
#else
# ifdef FOUNDATION_EXPORTS
#  define FOUNDATION_API __declspec(dllexport)
# else
#  define FOUNDATION_API __declspec(dllimport)
# endif
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