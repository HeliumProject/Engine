#pragma once

#include "Common/Compiler.h"

#ifdef SMARTBUFFERDLL_EXPORTS
# define SMARTBUFFER_API __declspec(dllexport)
#else
# define SMARTBUFFER_API __declspec(dllimport)
#endif

#include "Profile/Profile.h"

//#define SMARTBUFFER_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined(SMARTBUFFER_PROFILE)
#define SMARTBUFFER_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
#define SMARTBUFFER_SCOPE_TIMER(__Str)
#endif