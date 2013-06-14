#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef EMPTY_GAME_EXPORTS
        #define EMPTY_GAME_API HELIUM_API_EXPORT
    #else
        #define EMPTY_GAME_API HELIUM_API_IMPORT
    #endif
#else
    #define EMPTY_GAME_API
#endif
