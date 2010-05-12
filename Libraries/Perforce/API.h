#pragma once

#include "Profile/Profile.h"

#ifdef NOCTURNAL_STATIC
# define PERFORCE_API
#else
# ifdef PERFORCE_EXPORTS
#  define PERFORCE_API __declspec(dllexport)
# else
#  define PERFORCE_API __declspec(dllimport)
# endif
#endif

// profiling for perforce systems
//#define PERFORCE_PROFILE

#if defined( PROFILE_INSTRUMENT_ALL ) || defined ( PERFORCE_PROFILE )
# define PERFORCE_SCOPE_TIMER( __Str ) PROFILE_SCOPE_TIMER( __Str )
#else
# define PERFORCE_SCOPE_TIMER(__Str)
#endif