//----------------------------------------------------------------------------------------------------------------------
// MemoryHeapPreInitialization.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/MemoryHeapPreInitialization.h"

#include "EngineJobs/EngineJobs.h"
#include "GraphicsJobs/GraphicsJobs.h"

namespace Lunar
{
    /// Destructor.
    MemoryHeapPreInitialization::~MemoryHeapPreInitialization()
    {
    }

    /// Pre-initialize dynamic memory heaps from the main thread during start-up.
    void MemoryHeapPreInitialization::PreInitialize()
    {
        InitEngineJobsDefaultHeap();
        InitGraphicsJobsDefaultHeap();
    }
}
