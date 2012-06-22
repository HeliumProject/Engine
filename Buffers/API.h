#pragma once

#include "Platform/Platform.h"

#include "Foundation/Profile.h"

#if HELIUM_SHARED
# ifdef HELIUM_BUFFERS_EXPORTS
#  define HELIUM_BUFFERS_API HELIUM_API_EXPORT
# else
#  define HELIUM_BUFFERS_API HELIUM_API_IMPORT
# endif
#else
#define HELIUM_BUFFERS_API
#endif

//#define BUFFERS_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined(BUFFERS_PROFILE)
#define BUFFERS_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
#define BUFFERS_SCOPE_TIMER(__Str)
#endif