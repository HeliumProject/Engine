#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_GRAPHICS_TYPES_EXPORTS
        #define HELIUM_GRAPHICS_TYPES_API HELIUM_API_EXPORT
    #else
        #define HELIUM_GRAPHICS_TYPES_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_GRAPHICS_TYPES_API
#endif

/// Non-zero to enable Granny integration for animations, zero to disable.
#define HELIUM_USE_GRANNY_ANIMATION ( HELIUM_HAVE_GRANNY )
