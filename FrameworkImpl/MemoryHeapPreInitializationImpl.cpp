#include "FrameworkImplPch.h"
#include "FrameworkImpl/MemoryHeapPreInitializationImpl.h"

#include "EngineJobs/EngineJobs.h"
#include "GraphicsJobs/GraphicsJobs.h"

using namespace Helium;

/// Destructor.
MemoryHeapPreInitializationImpl::~MemoryHeapPreInitializationImpl()
{
}

/// Pre-initialize dynamic memory heaps from the main thread during start-up.
void MemoryHeapPreInitializationImpl::PreInitialize()
{
    InitEngineJobsDefaultHeap();
    InitGraphicsJobsDefaultHeap();
}
