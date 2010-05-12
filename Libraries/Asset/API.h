#pragma once

#include "Common/Compiler.h"

#ifdef ASSETDLL_EXPORTS
#define ASSET_API __declspec(dllexport)
#else
#define ASSET_API __declspec(dllimport)
#endif


// profiling for header systems
//#define ASSETTRACKER_PROFILE

#if defined(PROFILE_INSTRUMENTATION) && defined (ASSETTRACKER_PROFILE)
# define ASSETTRACKER_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define ASSETTRACKER_SCOPE_TIMER(__Str)
#endif


// profiling for header systems
//#define ASSETMANAGER_PROFILE

#if defined(PROFILE_INSTRUMENTATION) && defined (ASSETMANAGER_PROFILE)
# define ASSETMANAGER_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define ASSETMANAGER_SCOPE_TIMER(__Str)
#endif