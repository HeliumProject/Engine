#pragma once

#include "Profile/Profile.h"

#ifdef CODEGEN_EXPORTS
# define CODEGEN_API __declspec(dllexport)
#else
# define CODEGEN_API __declspec(dllimport)
#endif

// profiling for this module
//#define CODEGEN_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (CODEGEN_PROFILE)
# define CODEGEN_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define CODEGEN_SCOPE_TIMER(__Str)
#endif