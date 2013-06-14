#pragma once

#include "Framework/Framework.h"

namespace Helium
{
    /// Base class for pre-initialization of multi-threaded module memory heaps.
    ///
    /// Modules that cannot be safely guaranteed to perform dynamic memory allocations from the main thread prior to
    /// heavy multi-threaded memory usage should have their memory heaps pre-initialized.  Modules that provide Asset
    /// types are typically safe, as type registration will ensure memory heaps are pre-initialized.  Job-only modules,
    /// on the other hand, are susceptible to memory heap corruption if heap pre-initialization is not performed.
    ///
    /// This implementation performs heap pre-initialization for the following modules:
    /// - EngineJobs
    /// - GraphicsJobs
    class HELIUM_FRAMEWORK_API MemoryHeapPreInitialization
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~MemoryHeapPreInitialization();
        //@}

        /// @name Heap Pre-initialization
        //@{
        virtual void PreInitialize() = 0;
        //@}
    };
}
