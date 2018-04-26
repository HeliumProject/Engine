#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_GRAPHICS_EXPORTS
        #define HELIUM_GRAPHICS_API HELIUM_API_EXPORT
    #else
        #define HELIUM_GRAPHICS_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_GRAPHICS_API
#endif

#if HELIUM_TOOLS || (!HELIUM_RELEASE && !HELIUM_PROFILE)
#   define GRAPHICS_SCENE_BUFFERED_DRAWER 1
#else
#   define GRAPHICS_SCENE_BUFFERED_DRAWER 0
#endif
