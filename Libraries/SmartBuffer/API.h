#pragma once

#include "Platform/Compiler.h"

#ifdef SMARTBUFFER_DLL
# ifdef SMARTBUFFER_EXPORTS
#  define SMARTBUFFER_API __declspec(dllexport)
# else
#  define SMARTBUFFER_API __declspec(dllimport)
# endif
# else
# define SMARTBUFFER_API
#endif

#include "Foundation/Profile.h"

//#define SMARTBUFFER_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined(SMARTBUFFER_PROFILE)
#define SMARTBUFFER_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
#define SMARTBUFFER_SCOPE_TIMER(__Str)
#endif