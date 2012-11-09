//----------------------------------------------------------------------------------------------------------------------
// EngineJobs.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_ENGINE_JOBS_ENGINE_JOBS_H
#define HELIUM_ENGINE_JOBS_ENGINE_JOBS_H

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_ENGINE_JOBS_EXPORTS
        #define HELIUM_ENGINE_JOBS_API HELIUM_API_EXPORT
    #else
        #define HELIUM_ENGINE_JOBS_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_ENGINE_JOBS_API
#endif

namespace Helium
{
    HELIUM_ENGINE_JOBS_API extern void InitEngineJobsDefaultHeap();
}

#endif  // HELIUM_ENGINE_JOBS_ENGINE_JOBS_H
