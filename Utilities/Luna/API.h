#pragma once

#include "Profile/Profile.h"

//
// Core
//

#ifdef LUNA_CORE_DLL
# ifdef LUNA_CORE_EXPORTS
#  define LUNA_CORE_API __declspec(dllexport)
# else
#  define LUNA_CORE_API __declspec(dllimport)
# endif
#else
# define LUNA_CORE_API
#endif

// profiling for CORE systems
//#define LUNA_CORE_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (LUNA_CORE_PROFILE)
# define LUNA_CORE_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define LUNA_CORE_SCOPE_TIMER(__Str)
#endif

//
// Editor
//

#define LUNA_EDITOR_API

//
// Live
//

#ifdef LUNA_LIVE_DLL
# ifdef LUNA_LIVE_EXPORTS
#  define LUNA_LIVE_API __declspec(dllexport)
# else
#  define LUNA_LIVE_API __declspec(dllimport)
# endif
#else
# define LUNA_LIVE_API
#endif

// profiling for LIVE systems
//#define LUNA_LIVE_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (LUNA_LIVE_PROFILE)
# define LUNA_LIVE_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define LUNA_LIVE_SCOPE_TIMER(__Str)
#endif

//
// Character
//

#ifdef LUNA_CHARACTER_DLL
# ifdef LUNA_CHARACTER_EXPORTS
#  define LUNA_CHARACTER_API __declspec(dllexport)
# else
#  define LUNA_CHARACTER_API __declspec(dllimport)
# endif
#else
# define LUNA_CHARACTER_API
#endif

// profiling for CHARACTER systems
//#define LUNA_CHARACTER_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (LUNA_CHARACTER_PROFILE)
# define LUNA_CHARACTER_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define LUNA_CHARACTER_SCOPE_TIMER(__Str)
#endif

//
// AnimationEvent
//

#ifdef LUNA_ANIMATION_EVENTS_DLL
# ifdef LUNA_ANIMATION_EVENTS_EXPORTS
#  define LUNA_ANIMATION_EVENTS_API __declspec(dllexport)
# else
#  define LUNA_ANIMATION_EVENTS_API __declspec(dllimport)
# endif
#else
# define LUNA_ANIMATION_EVENTS_API
#endif

// profiling for ANIMATION_EVENT systems
//#define LUNA_ANIMATION_EVENTS_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (LUNA_ANIMATION_EVENTS_PROFILE)
# define LUNA_ANIMATION_EVENTS_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define LUNA_ANIMATION_EVENTS_SCOPE_TIMER(__Str)
#endif

//
// CinematicEvent
//

#ifdef LUNA_CINEMATIC_EVENTS_DLL
# ifdef LUNA_CINEMATIC_EVENTS_EXPORTS
#  define LUNA_CINEMATIC_EVENTS_API __declspec(dllexport)
# else
#  define LUNA_CINEMATIC_EVENTS_API __declspec(dllimport)
# endif
#else
# define LUNA_CINEMATIC_EVENTS_API
#endif

// profiling for CINEMATIC_EVENT systems
//#define LUNA_CINEMATIC_EVENT_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (LUNA_CINEMATIC_EVENT_PROFILE)
# define LUNA_CINEMATIC_EVENT_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define LUNA_CINEMATIC_EVENT_SCOPE_TIMER(__Str)
#endif

//
// Scene
//

#ifdef LUNA_SCENE_DLL
# ifdef LUNA_SCENE_EXPORTS
#  define LUNA_SCENE_API __declspec(dllexport)
# else
#  define LUNA_SCENE_API __declspec(dllimport)
# endif
#else
# define LUNA_SCENE_API
#endif

// profiling for SCENE systems
//#define LUNA_SCENE_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (LUNA_SCENE_PROFILE)
# define LUNA_SCENE_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define LUNA_SCENE_SCOPE_TIMER(__Str)
#endif

// profiling for SCENE EVALUATE systems
//#define LUNA_SCENE_PROFILE_EVALUATE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (LUNA_SCENE_PROFILE_EVALUATE)
# define LUNA_SCENE_EVALUATE_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define LUNA_SCENE_EVALUATE_SCOPE_TIMER(__Str)
#endif

// profiling for SCENE DRAW systems
//#define LUNA_SCENE_PROFILE_DRAW

#if defined(PROFILE_INSTRUMENT_ALL) || defined (LUNA_SCENE_PROFILE_DRAW)
# define LUNA_SCENE_DRAW_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define LUNA_SCENE_DRAW_SCOPE_TIMER(__Str)
#endif

//
// Task
//

#ifdef LUNA_TASK_DLL
# ifdef LUNA_TASK_EXPORTS
#  define LUNA_TASK_API __declspec(dllexport)
# else
#  define LUNA_TASK_API __declspec(dllimport)
# endif
#else
# define LUNA_TASK_API
#endif

// profiling for TASK systems
//#define LUNA_TASK_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (LUNA_TASK_PROFILE)
# define LUNA_TASK_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define LUNA_TASK_SCOPE_TIMER(__Str)
#endif
