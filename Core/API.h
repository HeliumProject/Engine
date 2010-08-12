#pragma once

#ifdef CORE_DLL
# ifdef CORE_EXPORTS
#  define CORE_API __declspec (dllexport)
# else
#  define CORE_API __declspec (dllimport)
# endif
#else
# define CORE_API
#endif

// profiling for SCENE systems
//#define CORE_SCENE_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (CORE_SCENE_PROFILE)
# define CORE_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define CORE_SCOPE_TIMER(__Str)
#endif

// profiling for SCENE EVALUATE systems
//#define CORE_SCENE_PROFILE_EVALUATE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (CORE_SCENE_PROFILE_EVALUATE)
# define CORE_EVALUATE_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define CORE_EVALUATE_SCOPE_TIMER(__Str)
#endif

// profiling for SCENE DRAW systems
//#define CORE_SCENE_PROFILE_DRAW

#if defined(PROFILE_INSTRUMENT_ALL) || defined (CORE_SCENE_PROFILE_DRAW)
# define CORE_RENDER_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define CORE_RENDER_SCOPE_TIMER(__Str)
#endif