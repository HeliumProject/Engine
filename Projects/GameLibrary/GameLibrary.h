#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef GAME_LIBRARY_EXPORTS
        #define GAME_LIBRARY_API HELIUM_API_EXPORT
    #else
        #define GAME_LIBRARY_API HELIUM_API_IMPORT
    #endif
#else
    #define GAME_LIBRARY_API
#endif

#define EXAMPLE_GAME_MAX_WORLDS (1)
#define EXAMPLE_GAME_MAX_PLAYERS (4)
