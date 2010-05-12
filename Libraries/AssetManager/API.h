#pragma once

#include "Common/Compiler.h"

#ifdef ASSETMANAGER_DLL
# ifdef ASSETMANAGER_EXPORTS
#  define ASSETMANAGER_API __declspec(dllexport)
# else
#  define ASSETMANAGER_API __declspec(dllimport)
# endif
#else
# define ASSETMANAGER_API
#endif

// profiling for header systems
//#define ASSETMANAGER_PROFILE

#if defined(PROFILE_INSTRUMENTATION) && defined (ASSETMANAGER_PROFILE)
# define ASSETMANAGER_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define ASSETMANAGER_SCOPE_TIMER(__Str)
#endif