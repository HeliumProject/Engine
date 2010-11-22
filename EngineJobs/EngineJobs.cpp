//----------------------------------------------------------------------------------------------------------------------
// EngineJobs.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EngineJobsPch.h"
#include "EngineJobs/EngineJobs.h"

namespace Lunar
{
    /// Initialize the default memory heap for the EngineJobs module.
    ///
    /// This should be called from the main thread before running any jobs to make sure the allocator is properly
    /// initialized, as initialization of the heap is not thread-safe.  Note that this will only initialize a separate
    /// heap if module-specific memory heaps are enabled.
    void InitEngineJobsDefaultHeap()
    {
        HELIUM_DEFAULT_HEAP;
    }
}
