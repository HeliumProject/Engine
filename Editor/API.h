#pragma once

#include "Foundation/Profile.h"

// profiling for SCENE systems
//#define EDITOR_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined (EDITOR_PROFILE)
# define EDITOR_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define EDITOR_SCOPE_TIMER(__Str)
#endif

// profiling for SCENE DRAW systems
//#define EDITOR_PROFILE_RENDER

#if defined(PROFILE_INSTRUMENT_ALL) || defined (EDITOR_PROFILE_RENDER)
# define EDITOR_RENDER_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define EDITOR_RENDER_SCOPE_TIMER(__Str)
#endif