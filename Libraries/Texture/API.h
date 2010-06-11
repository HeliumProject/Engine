#pragma once

#include "Foundation/Profile.h"

#ifdef TEXTURE_EXPORTS
#define TEXTURE_API __declspec(dllexport)
#else
#define TEXTURE_API __declspec(dllimport)
#endif

// profiling for controlfreak systems
//#define TEXTURE_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined(TEXTURE_PROFILE)
# define TEXTURE_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define TEXTURE_SCOPE_TIMER(__Str)
#endif
