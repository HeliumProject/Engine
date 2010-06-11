#pragma once

#include "Platform/Assert.h"

#ifdef INSPECTCONTENT_DLL
# ifdef INSPECTCONTENT_EXPORTS
#  define INSPECTCONTENT_API __declspec(dllexport)
# else
#  define INSPECTCONTENT_API __declspec(dllimport)
# endif
#else
# define INSPECTCONTENT_API
#endif

//#define INSPECTCONTENT_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined(INSPECTCONTENT_PROFILE)
# define INSPECTCONTENT_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define INSPECTCONTENT_SCOPE_TIMER(__Str)
#endif
