#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_COMPONENTS_EXPORTS
        #define HELIUM_COMPONENTS_API HELIUM_API_EXPORT
    #else
        #define HELIUM_COMPONENTS_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_COMPONENTS_API
#endif
