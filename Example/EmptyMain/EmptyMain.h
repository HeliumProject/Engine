#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef EXAMPLE_EMPTY_MAIN_EXPORTS
        #define EMPTY_MAIN_API HELIUM_API_EXPORT
    #else
        #define EMPTY_MAIN_API HELIUM_API_IMPORT
    #endif
#else
    #define EMPTY_MAIN_API
#endif
