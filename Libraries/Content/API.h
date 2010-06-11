#pragma once

#include "Platform/Compiler.h"
#include "Foundation/Profile.h"

#ifdef CONTENTDLL_EXPORTS
# define CONTENT_API __declspec(dllexport)
#else
# define CONTENT_API __declspec(dllimport)
#endif

// profiling for header systems
//#define CONTENT_PROFILE

#if defined( PROFILE_INSTRUMENT_ALL ) || defined ( CONTENT_PROFILE )
# define CONTENT_SCOPE_TIMER( __Str ) PROFILE_SCOPE_TIMER( __Str )
#else
# define CONTENT_SCOPE_TIMER(__Str)
#endif