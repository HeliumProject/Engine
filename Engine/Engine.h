#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_ENGINE_EXPORTS
        #define HELIUM_ENGINE_API HELIUM_API_EXPORT
    #else
        #define HELIUM_ENGINE_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_ENGINE_API
#endif
