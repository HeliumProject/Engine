#pragma once

#include "Platform/Platform.h"

#include "Foundation/Profile.h"

//#define SMARTBUFFER_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined(SMARTBUFFER_PROFILE)
#define SMARTBUFFER_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
#define SMARTBUFFER_SCOPE_TIMER(__Str)
#endif