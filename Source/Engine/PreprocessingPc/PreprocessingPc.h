#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_PREPROCESSING_PC_EXPORTS
        #define HELIUM_PREPROCESSING_PC_API HELIUM_API_EXPORT
    #else
        #define HELIUM_PREPROCESSING_PC_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_PREPROCESSING_PC_API
#endif
