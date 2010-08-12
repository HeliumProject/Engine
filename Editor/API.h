#pragma once

#include "Foundation/Profile.h"

//
// Core
//

#ifdef EDITOR_CORE_DLL
# ifdef EDITOR_CORE_EXPORTS
#  define EDITOR_CORE_API __declspec(dllexport)
# else
#  define EDITOR_CORE_API __declspec(dllimport)
# endif
#else
# define EDITOR_CORE_API
#endif

// profiling for CORE systems
//#define EDITOR_CORE_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (EDITOR_CORE_PROFILE)
# define EDITOR_CORE_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define EDITOR_CORE_SCOPE_TIMER(__Str)
#endif

//
// Editor
//

// profiling for SCENE systems
//#define EDITOR_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (EDITOR_PROFILE)
# define EDITOR_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define EDITOR_SCOPE_TIMER(__Str)
#endif

// profiling for SCENE DRAW systems
//#define EDITOR_PROFILE_RENDER

#if defined(PROFILE_INSTRUMENT_ALL) || defined (EDITOR_PROFILE_RENDER)
# define EDITOR_RENDER_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define EDITOR_RENDER_SCOPE_TIMER(__Str)
#endif

//
// Task
//

#ifdef EDITOR_TASK_DLL
# ifdef EDITOR_TASK_EXPORTS
#  define EDITOR_TASK_API __declspec(dllexport)
# else
#  define EDITOR_TASK_API __declspec(dllimport)
# endif
#else
# define EDITOR_TASK_API
#endif
