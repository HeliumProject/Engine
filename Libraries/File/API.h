#pragma once

#include "Common/Compiler.h"

#ifdef FILE_EXPORTS
# define FILE_API __declspec(dllexport)
#else
# define FILE_API __declspec(dllimport)
#endif

// profiling for header systems
//#define FILE_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (FILE_PROFILE)
# define FILE_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define FILE_SCOPE_TIMER(__Str)
#endif