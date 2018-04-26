#pragma once

#include "FrameworkImpl/FrameworkImpl.h"
#include "Framework/MemoryHeapPreInitialization.h"

namespace Helium
{
    /// Base class for pre-initialization of multi-threaded module memory heaps.
    ///
    /// This implementation performs heap pre-initialization for the following modules:
    /// - EngineJobs
    /// - GraphicsJobs
    class HELIUM_FRAMEWORK_IMPL_API MemoryHeapPreInitializationImpl : public MemoryHeapPreInitialization
    {
    public:
        /// @name Heap Pre-initialization
        //@{
        virtual void Startup();
        //@}
    };
}
