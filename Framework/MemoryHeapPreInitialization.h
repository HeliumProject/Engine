//----------------------------------------------------------------------------------------------------------------------
// MemoryHeapPreInitialization.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_FRAMEWORK_MEMORY_HEAP_PRE_INITIALIZATION_H
#define LUNAR_FRAMEWORK_MEMORY_HEAP_PRE_INITIALIZATION_H

#include "Framework/Framework.h"

namespace Lunar
{
    /// Base class for pre-initialization of multi-threaded module memory heaps.
    ///
    /// Modules that cannot be safely guaranteed to perform dynamic memory allocations from the main thread prior to
    /// heavy multi-threaded memory usage should have their memory heaps pre-initialized.  Modules that provide GameObject
    /// types are typically safe, as type registration will ensure memory heaps are pre-initialized.  Job-only modules,
    /// on the other hand, are susceptible to memory heap corruption if heap pre-initialization is not performed.
    ///
    /// This implementation performs heap pre-initialization for the following modules:
    /// - EngineJobs
    /// - GraphicsJobs
    class LUNAR_FRAMEWORK_API MemoryHeapPreInitialization
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~MemoryHeapPreInitialization();
        //@}

        /// @name Heap Pre-initialization
        //@{
        virtual void PreInitialize();
        //@}
    };
}

#endif  // LUNAR_FRAMEWORK_MEMORY_HEAP_PRE_INITIALIZATION_H
