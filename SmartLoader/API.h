#pragma once

#include "Common/Compiler.h"

#ifdef SMARTLOADERDLL_EXPORTS
# define SMARTLOADER_API __declspec(dllexport)
#else
# define SMARTLOADER_API __declspec(dllimport)
#endif

#include "Profile/Profile.h"

//#define SMARTLOADER_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined(SMARTLOADER_PROFILE)
#define SMARTLOADER_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
#define SMARTLOADER_SCOPE_TIMER(__Str)
#endif