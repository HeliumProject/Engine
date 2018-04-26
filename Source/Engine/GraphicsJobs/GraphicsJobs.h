#pragma once

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_GRAPHICS_JOBS_EXPORTS
        #define HELIUM_GRAPHICS_JOBS_API HELIUM_API_EXPORT
    #else
        #define HELIUM_GRAPHICS_JOBS_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_GRAPHICS_JOBS_API
#endif

namespace Helium
{
    HELIUM_GRAPHICS_JOBS_API extern void InitGraphicsJobsDefaultHeap();
}
