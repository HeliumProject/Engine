#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef EXAMPLE_EXAMPLE_MAIN_EXPORTS
        #define EXAMPLE_MAIN_API HELIUM_API_EXPORT
    #else
        #define EXAMPLE_MAIN_API HELIUM_API_IMPORT
    #endif
#else
    #define EXAMPLE_MAIN_API
#endif
