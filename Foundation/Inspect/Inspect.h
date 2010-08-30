#pragma once

#include "Platform/Assert.h"

// profiling for attribute systems
//#define INSPECT_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined(INSPECT_PROFILE)
# define INSPECT_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define INSPECT_SCOPE_TIMER(__Str)
#endif

// prints stepping of script code compilation
//#define INSPECT_DEBUG_SCRIPT_COMPILE

// prints tracks of Data instances
//#define INSPECT_DEBUG_DATA_TRACKING
