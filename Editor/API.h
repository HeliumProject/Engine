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

#define EDITOR_EDITOR_API

//
// Live
//

#ifdef EDITOR_LIVE_DLL
# ifdef EDITOR_LIVE_EXPORTS
#  define EDITOR_LIVE_API __declspec(dllexport)
# else
#  define EDITOR_LIVE_API __declspec(dllimport)
# endif
#else
# define EDITOR_LIVE_API
#endif

// profiling for LIVE systems
//#define EDITOR_LIVE_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (EDITOR_LIVE_PROFILE)
# define EDITOR_LIVE_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define EDITOR_LIVE_SCOPE_TIMER(__Str)
#endif

//
// Character
//

#ifdef EDITOR_CHARACTER_DLL
# ifdef EDITOR_CHARACTER_EXPORTS
#  define EDITOR_CHARACTER_API __declspec(dllexport)
# else
#  define EDITOR_CHARACTER_API __declspec(dllimport)
# endif
#else
# define EDITOR_CHARACTER_API
#endif

// profiling for CHARACTER systems
//#define EDITOR_CHARACTER_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (EDITOR_CHARACTER_PROFILE)
# define EDITOR_CHARACTER_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define EDITOR_CHARACTER_SCOPE_TIMER(__Str)
#endif

//
// AnimationEvent
//

#ifdef EDITOR_ANIMATION_EVENTS_DLL
# ifdef EDITOR_ANIMATION_EVENTS_EXPORTS
#  define EDITOR_ANIMATION_EVENTS_API __declspec(dllexport)
# else
#  define EDITOR_ANIMATION_EVENTS_API __declspec(dllimport)
# endif
#else
# define EDITOR_ANIMATION_EVENTS_API
#endif

// profiling for ANIMATION_EVENT systems
//#define EDITOR_ANIMATION_EVENTS_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (EDITOR_ANIMATION_EVENTS_PROFILE)
# define EDITOR_ANIMATION_EVENTS_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define EDITOR_ANIMATION_EVENTS_SCOPE_TIMER(__Str)
#endif

//
// CinematicEvent
//

#ifdef EDITOR_CINEMATIC_EVENTS_DLL
# ifdef EDITOR_CINEMATIC_EVENTS_EXPORTS
#  define EDITOR_CINEMATIC_EVENTS_API __declspec(dllexport)
# else
#  define EDITOR_CINEMATIC_EVENTS_API __declspec(dllimport)
# endif
#else
# define EDITOR_CINEMATIC_EVENTS_API
#endif

// profiling for CINEMATIC_EVENT systems
//#define EDITOR_CINEMATIC_EVENT_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (EDITOR_CINEMATIC_EVENT_PROFILE)
# define EDITOR_CINEMATIC_EVENT_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define EDITOR_CINEMATIC_EVENT_SCOPE_TIMER(__Str)
#endif

//
// Scene
//

#ifdef EDITOR_SCENE_DLL
# ifdef EDITOR_SCENE_EXPORTS
#  define EDITOR_SCENE_API __declspec(dllexport)
# else
#  define EDITOR_SCENE_API __declspec(dllimport)
# endif
#else
# define EDITOR_SCENE_API
#endif

// profiling for SCENE systems
//#define EDITOR_SCENE_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (EDITOR_SCENE_PROFILE)
# define EDITOR_SCENE_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define EDITOR_SCENE_SCOPE_TIMER(__Str)
#endif

// profiling for SCENE EVALUATE systems
//#define EDITOR_SCENE_PROFILE_EVALUATE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (EDITOR_SCENE_PROFILE_EVALUATE)
# define EDITOR_SCENE_EVALUATE_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define EDITOR_SCENE_EVALUATE_SCOPE_TIMER(__Str)
#endif

// profiling for SCENE DRAW systems
//#define EDITOR_SCENE_PROFILE_DRAW

#if defined(PROFILE_INSTRUMENT_ALL) || defined (EDITOR_SCENE_PROFILE_DRAW)
# define EDITOR_SCENE_DRAW_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define EDITOR_SCENE_DRAW_SCOPE_TIMER(__Str)
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

// profiling for TASK systems
//#define EDITOR_TASK_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (EDITOR_TASK_PROFILE)
# define EDITOR_TASK_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define EDITOR_TASK_SCOPE_TIMER(__Str)
#endif
