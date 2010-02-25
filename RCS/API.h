#pragma once

#ifdef NOCTURNAL_STATIC
# define RCS_API
#else
# ifdef RCS_EXPORTS
#  define RCS_API __declspec (dllexport)
# else
#  define RCS_API __declspec (dllimport)
# endif
#endif

// profiling for RCS systems
//#define RCS_PROFILE

#if defined( PROFILE_INSTRUMENT_ALL ) || defined ( RCS_PROFILE )
# define RCS_SCOPE_TIMER( __Str ) PROFILE_SCOPE_TIMER( __Str )
#else
# define RCS_SCOPE_TIMER(__Str)
#endif