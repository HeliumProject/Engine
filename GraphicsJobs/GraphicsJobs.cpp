//----------------------------------------------------------------------------------------------------------------------
// GraphicsJobs.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsJobsPch.h"
#include "GraphicsJobs/GraphicsJobs.h"

using namespace Helium;

/// Initialize the default memory heap for the GraphicsJobs module.
///
/// This should be called from the main thread before running any jobs to make sure the allocator is properly
/// initialized, as initialization of the heap is not thread-safe.  Note that this will only initialize a separate
/// heap if module-specific memory heaps are enabled.
void Helium::InitGraphicsJobsDefaultHeap()
{
    HELIUM_DEFAULT_HEAP;
}
