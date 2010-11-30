// Note that we do not use pre-compiled headers for this file so as to allow including "Platform/Memory.h" without the
// inline, module-specific "new"/"delete" overrides.

#include "Platform/Memory.h"

#include "Platform/Thread.h"

// Define the memory heap for the current module and include the "new"/"delete" operator implementations.
HELIUM_DEFINE_DEFAULT_MODULE_HEAP( Platform );

#if HELIUM_SHARED
#include "Platform/NewDelete.h"
#endif

using namespace Helium;

#if HELIUM_ENABLE_MEMORY_TRACKING
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
    ThreadLocalPointer& tls = GetMemoryHeapTls();
    StackMemoryHeap<>* pHeap = static_cast< StackMemoryHeap<>* >( tls.GetPointer() );
    if( !pHeap )
    {
        // Heap does not already exist for this thread, so create one.
        pHeap = new StackMemoryHeap<>( BLOCK_SIZE );
        HELIUM_ASSERT( pHeap );

        tls.SetPointer( pHeap );
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
    ThreadLocalPointer& tls = GetMemoryHeapTls();
    StackMemoryHeap<>* pHeap = static_cast< StackMemoryHeap<>* >( tls.GetPointer() );
    if( pHeap )
    {
        delete pHeap;
        tls.SetPointer( NULL );
    }
}

/// Get the thread-local storage pointer for the current thread's stack heap.
///
/// @return  Thread-local storage pointer for the thread-local stack heap instance.
ThreadLocalPointer& ThreadLocalStackAllocator::GetMemoryHeapTls()
{
    // Keeping this as a local static variable should help us enforce its construction on the first attempt to
    // allocate memory, regardless of whichever code attempts to dynamically allocate memory first.
    // XXX TMC: I'd imagine a race condition could still occur where multiple threads could use
    // ThreadLocalStackAllocator for the first time at around the same time.  To be safe, the main thread should try
    // to use ThreadLocalStackAllocator before any threads are created in order to prep this value (could do
    // something in the Thread class itself...).
    static ThreadLocalPointer memoryHeapTls;

    return memoryHeapTls;
}

#if !HELIUM_USE_MODULE_HEAPS
/// Get the default heap used for dynamic allocations within the engine.
///
/// @return  Reference to the default dynamic memory heap.
DynamicMemoryHeap& GetDefaultHeap()
{
    static DynamicMemoryHeap* pDefaultHeap = NULL;
    if( !pDefaultHeap )
    {
        pDefaultHeap = new( PhysicalMemory::Allocate( sizeof( DynamicMemoryHeap ) ) )
            DynamicMemoryHeap HELIUM_DYNAMIC_MEMORY_HEAP_INIT( TXT( "Default" ) );
    }

    return *pDefaultHeap;
}
#endif

#if HELIUM_USE_EXTERNAL_HEAP
/// Get the fallback heap for allocations made by external libraries.
///
/// @return  Reference for the external allocation fallback heap.
DynamicMemoryHeap& GetExternalHeap()
{
    static DynamicMemoryHeap* pExternalHeap = NULL;
    if( !pExternalHeap )
    {
        pExternalHeap = new( PhysicalMemory::Allocate( sizeof( DynamicMemoryHeap ) ) )
            DynamicMemoryHeap HELIUM_DYNAMIC_MEMORY_HEAP_INIT( TXT( "External" ) );
    }

    return *pExternalHeap;
}
#endif
