#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_RENDERING_EXPORTS
        #define HELIUM_RENDERING_API HELIUM_API_EXPORT
    #else
        #define HELIUM_RENDERING_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_RENDERING_API
#endif
