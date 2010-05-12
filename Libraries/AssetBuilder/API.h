#pragma once

#include "Common/Compiler.h"

#ifdef ASSETBUILDERDLL_EXPORTS
#define ASSETBUILDER_API __declspec(dllexport)
#else
#define ASSETBUILDER_API __declspec(dllimport)
#endif

// profiling for perforce systems
//#define BUILDER_PROFILE

#if defined( PROFILE_INSTRUMENT_ALL ) || defined ( BUILDER_PROFILE )
# define BUILDER_SCOPE_TIMER( __Str ) PROFILE_SCOPE_TIMER( __Str )
#else
# define BUILDER_SCOPE_TIMER(__Str)
#endif