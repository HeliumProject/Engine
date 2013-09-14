#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_FRAMEWORK_WIN_EXPORTS
        #define HELIUM_FRAMEWORK_WIN_API HELIUM_API_EXPORT
    #else
        #define HELIUM_FRAMEWORK_WIN_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_FRAMEWORK_WIN_API
#endif
