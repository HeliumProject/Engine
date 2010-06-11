#pragma once

#include "Foundation/Profile.h"

#ifdef MAYAEXPORTER_EXPORTS
#define MAYAEXPORTER_EXPORT	__declspec (dllexport)
#else
#define MAYAEXPORTER_EXPORT	__declspec (dllimport)
#endif

// profiling for header systems
#define MAYAEXPORTER_PROFILE

#if defined( PROFILE_INSTRUMENTATION ) && defined ( MAYAEXPORTER_PROFILE )
# define MAYAEXPORTER_SCOPE_TIMER( __Str ) PROFILE_SCOPE_TIMER( __Str )
#else
# define MAYAEXPORTER_SCOPE_TIMER(__Str)
#endif