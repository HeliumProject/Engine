#pragma once

#include "Foundation/Profile.h"

#ifdef NOCTURNAL_STATIC
#define MATH_API
#else
# ifdef MATHDLL_EXPORTS
#  define MATH_API __declspec(dllexport)
# else
#  define MATH_API __declspec(dllimport)
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