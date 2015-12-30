#include "FrameworkImplPch.h"
#include "FrameworkImpl/MemoryHeapPreInitializationImpl.h"

#include "EngineJobs/EngineJobs.h"
#include "GraphicsJobs/GraphicsJobs.h"

using namespace Helium;

/// Pre-initialize dynamic memory heaps from the main thread during start-up.
void MemoryHeapPreInitializationImpl::Startup()
{
	/// Modules that cannot be safely guaranteed to perform dynamic memory allocations from the main thread prior to
	/// heavy multi-threaded memory usage should have their memory heaps pre-initialized.  Modules that provide Asset
	/// types are typically safe, as type registration will ensure memory heaps are pre-initialized.  Job-only modules,
	/// on the other hand, are susceptible to memory heap corruption if heap pre-initialization is not performed.
	///
	InitEngineJobsDefaultHeap();
	InitGraphicsJobsDefaultHeap();
}
