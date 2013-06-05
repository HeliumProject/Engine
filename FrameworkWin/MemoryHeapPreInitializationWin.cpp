//----------------------------------------------------------------------------------------------------------------------
// MemoryHeapPreInitialization.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkWinPch.h"
#include "FrameworkWin/MemoryHeapPreInitializationWin.h"

#include "EngineJobs/EngineJobs.h"
#include "GraphicsJobs/GraphicsJobs.h"

using namespace Helium;

/// Destructor.
MemoryHeapPreInitializationWin::~MemoryHeapPreInitializationWin()
{
}

/// Pre-initialize dynamic memory heaps from the main thread during start-up.
void MemoryHeapPreInitializationWin::PreInitialize()
{
    InitEngineJobsDefaultHeap();
    InitGraphicsJobsDefaultHeap();
}
