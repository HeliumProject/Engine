#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_RENDERING_GL_EXPORTS
        #define HELIUM_RENDERING_GL_API HELIUM_API_EXPORT
    #else
        #define HELIUM_RENDERING_GL_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_RENDERING_GL_API
#endif
