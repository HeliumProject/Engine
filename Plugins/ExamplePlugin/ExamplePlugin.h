#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef EXAMPLE_PLUGIN_EXPORTS
        #define EXAMPLE_PLUGIN_API HELIUM_API_EXPORT
    #else
        #define EXAMPLE_PLUGIN_API HELIUM_API_IMPORT
    #endif
#else
    #define EXAMPLE_PLUGIN_API
#endif
