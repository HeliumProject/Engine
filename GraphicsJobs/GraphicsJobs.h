//----------------------------------------------------------------------------------------------------------------------
// GraphicsJobs.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_GRAPHICS_JOBS_GRAPHICS_JOBS_H
#define HELIUM_GRAPHICS_JOBS_GRAPHICS_JOBS_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

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

#endif  // HELIUM_GRAPHICS_JOBS_GRAPHICS_JOBS_H
