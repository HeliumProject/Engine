#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef EXAMPLE_GAME_EXPORTS
        #define EXAMPLE_GAME_API HELIUM_API_EXPORT
    #else
        #define EXAMPLE_GAME_API HELIUM_API_IMPORT
    #endif
#else
    #define EXAMPLE_GAME_API
#endif

#define EXAMPLE_GAME_MAX_WORLDS (1)
#define EXAMPLE_GAME_MAX_PLAYERS (4)
