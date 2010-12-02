//----------------------------------------------------------------------------------------------------------------------
// TestJobs.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "TestJobsPch.h"
#include "TestJobs/TestJobs.h"

using namespace Lunar;

/// Initialize the default memory heap for the TestJobs module.
///
/// This should be called from the main thread before running any jobs to make sure the allocator is properly
/// initialized, as initialization of the heap is not thread-safe.  Note that this will only initialize a separate
/// heap if module-specific memory heaps are enabled.
void Lunar::InitTestJobsDefaultHeap()
{
    HELIUM_DEFAULT_HEAP;
}
