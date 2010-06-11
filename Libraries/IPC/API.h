#pragma once

#include "Foundation/Profile.h"

#ifdef WIN32
# ifdef IPC_EXPORTS
#  define IPC_API __declspec(dllexport)
# else
#  define IPC_API __declspec(dllimport)
# endif
#else
# define IPC_API
#endif

//#define IPC_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined(IPC_PROFILE)
# define IPC_SCOPE_TIMER(x) PROFILE_SCOPE_TIMER(x)
#else
# define IPC_SCOPE_TIMER(x)
#endif
