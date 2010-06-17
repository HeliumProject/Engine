#pragma once

#include "Platform/Compiler.h"

#ifdef FINDER_DLL
# ifdef FINDER_EXPORTS
#  define FINDER_API __declspec(dllexport)
# else
#  define FINDER_API __declspec(dllimport)
# endif
#else
# define FINDER_API
#endif

// profiling for header systems
//#define FINDER_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (FINDER_PROFILE)
# define FINDER_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define FINDER_SCOPE_TIMER(__Str)
#endif