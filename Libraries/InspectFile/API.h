#pragma once

#include "Platform/Assert.h"

#ifdef INSPECTFILE_DLL
# ifdef INSPECTFILE_EXPORTS
#  define INSPECTFILE_API __declspec(dllexport)
# else
#  define INSPECTFILE_API __declspec(dllimport)
# endif
#else
# define INSPECTFILE_API
#endif

//#define INSPECTFILE_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined(INSPECTFILE_PROFILE)
# define INSPECTFILE_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define INSPECTFILE_SCOPE_TIMER(__Str)
#endif
