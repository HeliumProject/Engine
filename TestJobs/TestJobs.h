//----------------------------------------------------------------------------------------------------------------------
// TestJobs.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_TEST_JOBS_TEST_JOBS_H
#define HELIUM_TEST_JOBS_TEST_JOBS_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef HELIUM_TEST_JOBS_EXPORTS
        #define HELIUM_TEST_JOBS_API HELIUM_API_EXPORT
    #else
        #define HELIUM_TEST_JOBS_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_TEST_JOBS_API
#endif

namespace Helium
{
    HELIUM_TEST_JOBS_API extern void InitTestJobsDefaultHeap();
}

#endif  // HELIUM_TEST_JOBS_TEST_JOBS_H
