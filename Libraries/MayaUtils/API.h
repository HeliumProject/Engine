#pragma once

#include "Platform/Compiler.h"

#ifdef MAYAUTILSDLL_EXPORTS
#define MAYAUTILS_API __declspec(dllexport)
#else
#define MAYAUTILS_API __declspec(dllimport)
#endif

//#define MAYAUTILS_PROFILE

#if defined( PROFILE_INSTRUMENT_ALL ) || defined ( MAYAUTILS_PROFILE )
# define MAYAUTILS_SCOPE_TIMER( __Str ) PROFILE_SCOPE_TIMER( __Str )
#else
# define MAYAUTILS_SCOPE_TIMER(__Str)
#endif