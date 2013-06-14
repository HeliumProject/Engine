#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_EDITOR_SUPPORT_EXPORTS
        #define HELIUM_EDITOR_SUPPORT_API HELIUM_API_EXPORT
    #else
        #define HELIUM_EDITOR_SUPPORT_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_EDITOR_SUPPORT_API
#endif
