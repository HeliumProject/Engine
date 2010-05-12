#pragma once

#ifdef EVENTSYSTEM_EXPORTS
#  define EVENTSYSTEM_API __declspec(dllexport)
#else
#  define EVENTSYSTEM_API __declspec(dllimport)
#endif

// profiling for header systems
//#define EVENTSYSTEM_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (EVENTSYSTEM_PROFILE)
# define EVENTSYSTEM_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define EVENTSYSTEM_SCOPE_TIMER(__Str)
#endif