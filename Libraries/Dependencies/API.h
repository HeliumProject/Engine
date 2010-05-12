#pragma once

#include "Profile/Profile.h"

#ifdef DEPENDENCIES_EXPORTS
#  define DEPENDENCIES_API __declspec (dllexport)
#else
#  define DEPENDENCIES_API __declspec (dllimport)
#endif

// profiling for header systems
//#define DEPENDENCIES_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (DEPENDENCIES_PROFILE)
# define DEPENDENCIES_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define DEPENDENCIES_SCOPE_TIMER(__Str)
#endif


// this profiles file info fetching when the dependencies system is being profiled (guess)
//#define FILEINFO_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || ( defined (FILEINFO_PROFILE) || defined (DEPENDENCIES_PROFILE) )
# define FILEINFO_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define FILEINFO_SCOPE_TIMER(__Str)
#endif