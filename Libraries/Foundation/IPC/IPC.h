#pragma once

#include "Foundation/Profile.h"

//#define IPC_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined(IPC_PROFILE)
# define IPC_SCOPE_TIMER(x) PROFILE_SCOPE_TIMER(x)
#else
# define IPC_SCOPE_TIMER(x)
#endif
