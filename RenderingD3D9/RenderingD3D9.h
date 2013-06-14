#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_RENDERING_D3D9_EXPORTS
        #define HELIUM_RENDERING_D3D9_API HELIUM_API_EXPORT
    #else
        #define HELIUM_RENDERING_D3D9_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_RENDERING_D3D9_API
#endif
