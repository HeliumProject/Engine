//----------------------------------------------------------------------------------------------------------------------
// GraphicsJobs.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_GRAPHICS_JOBS_GRAPHICS_JOBS_H
#define LUNAR_GRAPHICS_JOBS_GRAPHICS_JOBS_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef LUNAR_GRAPHICS_JOBS_EXPORTS
        #define LUNAR_GRAPHICS_JOBS_API HELIUM_API_EXPORT
    #else
        #define LUNAR_GRAPHICS_JOBS_API HELIUM_API_IMPORT
    #endif
#else
    #define LUNAR_GRAPHICS_JOBS_API
#endif

namespace Lunar
{
    LUNAR_GRAPHICS_JOBS_API extern void InitGraphicsJobsDefaultHeap();
}

#endif  // LUNAR_GRAPHICS_JOBS_GRAPHICS_JOBS_H
