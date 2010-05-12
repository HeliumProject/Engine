#pragma once

#include "Profile/Profile.h"

#ifdef NSLDLL_EXPORTS
#  define NSL_API __declspec(dllexport)
#else
#  define NSL_API __declspec(dllimport)
#endif

// profiling for NSL
//#define NSL_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (NSL_PROFILE)
# define NSL_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define NSL_SCOPE_TIMER(__Str)
#endif