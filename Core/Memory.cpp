//----------------------------------------------------------------------------------------------------------------------
// Memory.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

// Note that we do not use pre-compiled headers for this file so as to allow including "Core/Memory.h" without the
// inline, module-specific "new"/"delete" overrides.

#include "CorePch.h"
#include "Core/Memory.h"
#include "Core/Threading.h"

namespace Lunar
{
#if L_ENABLE_MEMORY_TRACKING
    volatile size_t PhysicalMemory::sm_bytesAllocated = 0;
#endif

    /// Constructor.
    ThreadLocalStackAllocator::ThreadLocalStackAllocator()
    {
        m_pHeap = &GetMemoryHeap();
        HELIUM_ASSERT( m_pHeap );
    }

    /// Allocate a block of memory.
    ///
    /// @param[in] size  Number of bytes to allocate.
    ///
    /// @return  Base address of the allocation if successful, null if allocation failed.
    void* ThreadLocalStackAllocator::Allocate( size_t size )
    {
        HELIUM_ASSERT( m_pHeap );
        void* pMemory = m_pHeap->Allocate( size );

        return pMemory;
    }

    /// Allocate a block of aligned memory.
    ///
    /// @param[in] alignment  Allocation alignment (must be a power of two).
    /// @param[in] size       Number of bytes to allocate.
    ///
    /// @return  Base address of the allocation if successful, null if allocation failed.
    void* ThreadLocalStackAllocator::AllocateAligned( size_t alignment, size_t size )
    {
        HELIUM_ASSERT( m_pHeap );
        void* pMemory = m_pHeap->AllocateAligned( alignment, size );

        return pMemory;
    }

    /// Free a block of memory previously allocated using Allocate() or AllocateAligned().
    ///
    /// @param[in] pMemory  Base address of the allocation to free.
    void ThreadLocalStackAllocator::Free( void* pMemory )
    {
        HELIUM_ASSERT( m_pHeap );
        m_pHeap->Free( pMemory );
    }

    /// Get the stack-based memory heap for the current thread.
    ///
    /// @return  Reference to the current thread's stack-based heap.
    ///
    /// @see ReleaseMemoryHeap()
    StackMemoryHeap<>& ThreadLocalStackAllocator::GetMemoryHeap()
    {
        ThreadLocalStorage& tls = ThreadLocalStorage::GetInstance();

        size_t tlsIndex = GetMemoryHeapTlsIndex();
        HELIUM_ASSERT( IsValid( tlsIndex ) );

        StackMemoryHeap<>* pHeap = reinterpret_cast< StackMemoryHeap<>* >( tls.Get( tlsIndex ) );
        if( !pHeap )
        {
            // Heap does not already exist for this thread, so create one.
            pHeap = new StackMemoryHeap<>( BLOCK_SIZE );
            HELIUM_ASSERT( pHeap );

            tls.Set( tlsIndex, reinterpret_cast< uintptr_t >( pHeap ) );
        }

        return *pHeap;
    }

    /// Release the stack-based memory heap for the current thread.
    ///
    /// This should be called by any thread using this allocator prior to exiting the thread in order to avoid leaking
    /// memory.
    ///
    /// @see GetMemoryHeap()
    void ThreadLocalStackAllocator::ReleaseMemoryHeap()
    {
        ThreadLocalStorage& tls = ThreadLocalStorage::GetInstance();

        size_t tlsIndex = GetMemoryHeapTlsIndex();
        HELIUM_ASSERT( IsValid( tlsIndex ) );

        StackMemoryHeap<>* pHeap = reinterpret_cast< StackMemoryHeap<>* >( tls.Get( tlsIndex ) );
        if( pHeap )
        {
            delete pHeap;
            tls.Set( tlsIndex, 0 );
        }
    }

    /// Get the TLS index for the current thread's stack heap.
    ///
    /// @return  TLS index for storage of the thread-local stack heap instance.
    size_t ThreadLocalStackAllocator::GetMemoryHeapTlsIndex()
    {
        // Keeping this as a local static variable should help us enforce its construction on the first attempt to
        // allocate memory, regardless of whichever code attempts to dynamically allocate memory first.
        // XXX TMC: I'd imagine a race condition could still occur where multiple threads could use
        // ThreadLocalStackAllocator for the first time at around the same time.  To be safe, the main thread should try
        // to use ThreadLocalStackAllocator before any threads are created in order to prep this value (could do
        // something in the Thread class itself...).
        static size_t tlsIndex = ThreadLocalStorage::GetInstance().Allocate();
        HELIUM_ASSERT( IsValid( tlsIndex ) );

        return tlsIndex;
    }

#if !L_USE_MODULE_HEAPS
    /// Get the default heap used for dynamic allocations within the engine.
    ///
    /// @return  Reference to the default dynamic memory heap.
    DynamicMemoryHeap& GetDefaultHeap()
    {
        static L_DYNAMIC_MEMORY_HEAP( defaultHeap, TXT( "Default" ) );
        return defaultHeap;
    }
#endif

#if L_USE_EXTERNAL_HEAP
    /// Get the fallback heap for allocations made by external libraries.
    ///
    /// @return  Reference for the external allocation fallback heap.
    DynamicMemoryHeap& GetExternalHeap()
    {
        static L_DYNAMIC_MEMORY_HEAP( externalHeap, TXT( "External" ) );
        return externalHeap;
    }
#endif
}
