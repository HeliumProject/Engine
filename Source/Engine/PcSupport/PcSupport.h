#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_PC_SUPPORT_EXPORTS
        #define HELIUM_PC_SUPPORT_API HELIUM_API_EXPORT
    #else
        #define HELIUM_PC_SUPPORT_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_PC_SUPPORT_API
#endif
