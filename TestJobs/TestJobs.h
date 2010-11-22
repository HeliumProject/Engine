//----------------------------------------------------------------------------------------------------------------------
// TestJobs.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_TEST_JOBS_TEST_JOBS_H
#define LUNAR_TEST_JOBS_TEST_JOBS_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef LUNAR_TEST_JOBS_EXPORTS
        #define LUNAR_TEST_JOBS_API HELIUM_API_EXPORT
    #else
        #define LUNAR_TEST_JOBS_API HELIUM_API_IMPORT
    #endif
#else
    #define LUNAR_TEST_JOBS_API
#endif

namespace Lunar
{
    LUNAR_TEST_JOBS_API extern void InitTestJobsDefaultHeap();
}

#endif  // LUNAR_TEST_JOBS_TEST_JOBS_H
