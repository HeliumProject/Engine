#pragma once

#include "Platform/Types.h"
#include "Platform/Compiler.h"

#ifdef MAYA_EXPORTS
#define MAYA_API __declspec(dllexport)
#else
#define MAYA_API __declspec(dllimport)
#endif

//#define MAYA_PROFILE

#if defined( PROFILE_INSTRUMENT_ALL ) || defined ( MAYA_PROFILE )
# define MAYA_SCOPE_TIMER( __Str ) PROFILE_SCOPE_TIMER( __Str )
#else
# define MAYA_SCOPE_TIMER(__Str)
#endif

// profiling for exporter
//#define EXPORT_PROFILE

#if defined( PROFILE_INSTRUMENT_ALL ) || defined ( EXPORT_PROFILE )
# define EXPORT_SCOPE_TIMER( __Str ) PROFILE_SCOPE_TIMER( __Str )
#else
# define EXPORT_SCOPE_TIMER(__Str)
#endif