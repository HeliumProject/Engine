#pragma once

#include "Profile/Profile.h"

#ifdef NOCTURNAL_STATIC
# define FILESYSTEM_API
#else
# ifdef FILESYSTEM_EXPORTS
#  define FILESYSTEM_API __declspec(dllexport)
# else
#  define FILESYSTEM_API __declspec(dllimport)
# endif
#endif

// profiling for FileSystem
//#define FILESYSTEM_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (FILESYSTEM_PROFILE)
# define FILESYSTEM_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define FILESYSTEM_SCOPE_TIMER(__Str)
#endif