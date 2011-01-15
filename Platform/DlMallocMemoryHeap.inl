#define APPLY_PREFIX_IMPL_IMPL( PREFIX, FUNCTION ) _##PREFIX##_##FUNCTION
#define APPLY_PREFIX_IMPL( PREFIX, FUNCTION ) APPLY_PREFIX_IMPL_IMPL( PREFIX, FUNCTION )
#define APPLY_PREFIX( FUNCTION ) APPLY_PREFIX_IMPL( MEMORY_HEAP_CLASS_NAME, FUNCTION )

#define dlmalloc_usable_size        APPLY_PREFIX( dlmalloc_usable_size )
#define create_mspace               APPLY_PREFIX( create_mspace )
#define create_mspace_with_base     APPLY_PREFIX( create_mspace_with_base )
#define mspace_track_large_chunks   APPLY_PREFIX( mspace_track_large_chunks )
#define destroy_mspace              APPLY_PREFIX( destroy_mspace )
#define mspace_malloc               APPLY_PREFIX( mspace_malloc )
#define mspace_free                 APPLY_PREFIX( mspace_free )
#define mspace_calloc               APPLY_PREFIX( mspace_calloc )
#define mspace_realloc              APPLY_PREFIX( mspace_realloc )
#define mspace_memalign             APPLY_PREFIX( mspace_memalign )
#define mspace_independent_calloc   APPLY_PREFIX( mspace_independent_calloc )
#define mspace_independent_comalloc APPLY_PREFIX( mspace_independent_comalloc )
#define mspace_trim                 APPLY_PREFIX( mspace_trim )
#define mspace_malloc_stats         APPLY_PREFIX( mspace_malloc_stats )
#define mspace_footprint            APPLY_PREFIX( mspace_footprint )
#define mspace_max_footprint        APPLY_PREFIX( mspace_max_footprint )
#define mspace_mallinfo             APPLY_PREFIX( mspace_mallinfo )
#define mspace_usable_size          APPLY_PREFIX( mspace_usable_size )
#define mspace_mallopt              APPLY_PREFIX( mspace_mallopt )

/// Wrapper for Helium::PhysicalMemory::Allocate() to support return values expected by dlmalloc.
///
/// @param[in] size  Allocation size, in bytes.
///
/// @return  Address of the allocation if successfully allocated, (void*)~0 if not.
static void* PhysicalMemoryAllocate( size_t size )
{
    void* pMemory = Helium::PhysicalMemory::Allocate( size );
    return( pMemory ? pMemory : reinterpret_cast< void* >( ~static_cast< uintptr_t >( 0 ) ) );
}

/// Wrapper for Helium::PhysicalMemory::Free() to support return values expected by dlmalloc.
///
/// @param[in] pMemory  Base address of the region of memory to free.
/// @param[in] size     Size of the region of memory to free.
///
/// @return  Zero if successful, non-zero if an error occurred.
static int PhysicalMemoryFree( void* pMemory, size_t size )
{
    return( Helium::PhysicalMemory::Free( pMemory, size ) ? 0 : -1 );
}

#if HELIUM_ENABLE_TRACE
/// Wrapper for re-routing printf() calls to the logging system.
///
/// @param[in] pFormat  Format string.
/// @param[in] ...      Format arguments.
static void PrintfWrapper( const char* pFormat, ... )
{
#if HELIUM_UNICODE
    char message[ Helium::Trace::DEFAULT_MESSAGE_BUFFER_SIZE ];

    va_list argList;
    va_start( argList, pFormat );
    Helium::StringFormatVa( message, HELIUM_ARRAY_COUNT( message ), pFormat, argList );
    va_end( argList );

    message[ HELIUM_ARRAY_COUNT( message ) - 1 ] = '\0';

    // Do a direct conversion, assuming memory allocator logging is only using 7-bit ASCII text.
    wchar_t messageWide[ Helium::Trace::DEFAULT_MESSAGE_BUFFER_SIZE ];

    const char* pSourceCharacter = &message[ 0 ];
    wchar_t* pDestCharacter = &messageWide[ 0 ];
    for( ; ; )
    {
        uint8_t character = *pSourceCharacter;
        *pDestCharacter = static_cast< wchar_t >( character );

        if( character == '\0' )
        {
            break;
        }

        ++pSourceCharacter;
        ++pDestCharacter;
    }

    HELIUM_TRACE( Helium::TRACE_DEBUG, TXT( "%s" ), messageWide );
#else
    va_list argList;
    va_start( argList, pFormat );
    HELIUM_TRACE_VA( Helium::TRACE_DEBUG, pFormat, argList );
    va_end( argList );
#endif
}
#endif  // HELIUM_ENABLE_TRACE

#define MSPACES 1
#define ONLY_MSPACES 1
#define USE_DL_PREFIX 1
#define MMAP( s ) PhysicalMemoryAllocate( s )
#define MUNMAP( a, s ) PhysicalMemoryFree( a, s )
#define DIRECT_MMAP( s ) PhysicalMemoryAllocate( s )
#define HAVE_MREMAP 0
#define REALLOC_ZERO_BYTES_FREES 1
#define FOOTERS 1

#define CORRUPTION_ERROR_ACTION( m ) HELIUM_ASSERT_MSG_FALSE( TXT( "Memory corruption detected!" ) )
#define USAGE_ERROR_ACTION( m, p ) HELIUM_ASSERT_MSG_FALSE( TXT( "Incorrect realloc()/free() usage detected!" ) )

#if HELIUM_RELEASE || HELIUM_PROFILE
#define INSECURE 1
#else
#define INSECURE 0
#endif

#if HELIUM_DEBUG
#define DEBUG 1
#endif

#ifdef HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
#define FULLSANITYCHECKS
#endif

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4100 ) // 'identifier' : unreferenced formal parameter
#pragma warning( disable : 4127 ) // conditional expression is constant
#pragma warning( disable : 4189 ) // 'identifier' : local variable is initialized but not referenced
#pragma warning( disable : 4267 ) // 'var' : conversion from 'size_t' to 'type', possible loss of data
// XXX TMC: C4505 must be disabled on a per-source basis (disabling it on just the scope of the function definitions
// does not work).
//#pragma warning( disable : 4505 ) // 'function' : unreferenced local function has been removed
#pragma warning( disable : 4706 ) // assignment within conditional expression
#endif

// Re-route printf() calls to the engine's logging system.
#if HELIUM_ENABLE_TRACE
#define printf( FORMAT, ... ) PrintfWrapper( FORMAT, __VA_ARGS__ )
#else
#define printf( FORMAT, ... )
#endif

#if USE_NEDMALLOC
#define EXTSPEC static
#include "nedmalloc/nedmalloc.c"
#undef EXTSPEC
#else
#include "dlmalloc/malloc.c.h"
#endif

#undef printf

#ifdef _MSC_VER
#pragma warning( pop )
#endif

/// Constructor.
///
/// @param[in] capacity  Fixed size (in bytes) of the memory heap to create, or zero to create a growable heap.
Helium::MEMORY_HEAP_CLASS_NAME::MEMORY_HEAP_CLASS_NAME( size_t capacity )
#if !HELIUM_RELEASE && !HELIUM_PROFILE
    : m_pName( NULL )
#endif
#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
    , m_pVerboseTrackingData( NULL )
#endif
{
    ConstructNoName( capacity );
}

#if !HELIUM_RELEASE && !HELIUM_PROFILE
/// Constructor.
///
/// @param[in] pName     Name to associate with the memory heap (for debugging purposes).  Note that the heap holds
///                      onto the given pointer directly, so it must remain valid for the entire lifetime of the
///                      memory heap.  Using a hard-coded string literal is recommended.
/// @param[in] capacity  Fixed size (in bytes) of the memory heap to create, or zero to create a growable heap.
Helium::MEMORY_HEAP_CLASS_NAME::MEMORY_HEAP_CLASS_NAME( const tchar_t* pName, size_t capacity )
    : m_pName( pName )
#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
    , m_pVerboseTrackingData( NULL )
#endif
{
    ConstructNoName( capacity );
}
#endif

/// Destructor.
Helium::MEMORY_HEAP_CLASS_NAME::~MEMORY_HEAP_CLASS_NAME()
{
#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
    // Destroy the memory tracking data first in case it is referencing memory allocated from this allocator.
    bool bLockedTracking = ConditionalVerboseTrackingLock();

    bool bOldDisableBactraceTracking = sm_bDisableBacktraceTracking;
    sm_bDisableBacktraceTracking = true;

    DynamicMemoryHeapVerboseTrackingData* pVerboseTrackingData = m_pVerboseTrackingData;
    m_pVerboseTrackingData = NULL;

    HELIUM_ASSERT( pVerboseTrackingData->allocationBacktraceMap.empty() );
    pVerboseTrackingData->~DynamicMemoryHeapVerboseTrackingData();

    size_t allocationSize = Align( sizeof( DynamicMemoryHeapVerboseTrackingData ), PhysicalMemory::GetPageSize() );
    PhysicalMemory::Free( pVerboseTrackingData, allocationSize );

    sm_bDisableBacktraceTracking = bOldDisableBactraceTracking;

    if( bLockedTracking )
    {
        VerboseTrackingUnlock();
    }
#endif

    {
        ScopeWriteLock writeLock( GetGlobalHeapListLock() );

        MEMORY_HEAP_CLASS_NAME* pPreviousHeap = m_pPreviousHeap;
        MEMORY_HEAP_CLASS_NAME* pNextHeap = m_pNextHeap;
        if( pPreviousHeap )
        {
            pPreviousHeap->m_pNextHeap = pNextHeap;
        }
        else
        {
            sm_pGlobalHeapListHead = pNextHeap;
        }

        if( pNextHeap )
        {
            pNextHeap->m_pPreviousHeap = pPreviousHeap;
        }
    }

#if USE_NEDMALLOC
    nedalloc::neddestroypool( static_cast< nedalloc::nedpool* >( m_pMspace ) );
#else
    destroy_mspace( m_pMspace );
#endif
}

/// Allocate a block of memory.
///
/// @param[in] size  Number of bytes to allocate.
///
/// @return  Base address of the allocation if successful, null pointer if not.
///
/// @see Reallocate(), Free()
void* Helium::MEMORY_HEAP_CLASS_NAME::Allocate( size_t size )
{
#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
    bool bLockedTracking = ConditionalVerboseTrackingLock();
#endif

#if USE_NEDMALLOC
    void* pMemory = nedalloc::nedpmalloc( static_cast< nedalloc::nedpool* >( m_pMspace ), size );
#else
    void* pMemory = mspace_malloc( m_pMspace, size );
#endif

#if HELIUM_ENABLE_MEMORY_TRACKING
    if( pMemory )
    {
        AddAllocation( pMemory );
    }
#endif

#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
    if( bLockedTracking )
    {
        VerboseTrackingUnlock();
    }
#endif

    return pMemory;
}

/// Resize an allocation previously allocated using Allocate() or Reallocate().
///
/// Note that this should not be called on allocations made using AllocateAligned().
///
/// @param[in] pMemory  Base address of an allocation to resize.  If this is null, this will merely behave in the
///                     same fashion as if Allocate() was called directly.
/// @param[in] size     Size to which the allocation should be reallocated, in bytes.  If this is zero, this will
///                     merely behave in the same fashion as if Free() was called directly.
///
/// @return  Base address of the resized allocation if successful, null pointer if the memory could not be
///          reallocated or, in the case size is zero, was freed.  The original address provided should be discarded
///          if a valid pointer was returned or a zero-byte reallocation was requested.
///
/// @see Allocate(), Free()
void* Helium::MEMORY_HEAP_CLASS_NAME::Reallocate( void* pMemory, size_t size )
{
#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
    bool bLockedTracking = false;
#endif

#if HELIUM_ENABLE_MEMORY_TRACKING
    if( pMemory )
    {
#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
        bLockedTracking = ConditionalVerboseTrackingLock();
#endif

        MEMORY_HEAP_CLASS_NAME* pHeap = GetAllocationHeap( pMemory );
        HELIUM_ASSERT( pHeap );
        if( pHeap )
        {
            pHeap->RemoveAllocation( pMemory );
        }
    }
#endif

#if USE_NEDMALLOC
    pMemory = nedalloc::nedprealloc( static_cast< nedalloc::nedpool* >( m_pMspace ), pMemory, size );
#else
    pMemory = mspace_realloc( m_pMspace, pMemory, size );
#endif

#if HELIUM_ENABLE_MEMORY_TRACKING
    if( pMemory )
    {
#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
        if( !bLockedTracking )
        {
            bLockedTracking = ConditionalVerboseTrackingLock();
        }
#endif

        MEMORY_HEAP_CLASS_NAME* pHeap = GetAllocationHeap( pMemory );
        HELIUM_ASSERT( pHeap );
        if( pHeap )
        {
            pHeap->AddAllocation( pMemory );
        }
    }
#endif

#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
    if( bLockedTracking )
    {
        VerboseTrackingUnlock();
    }
#endif

    return pMemory;
}

/// Allocate an aligned block of memory.
///
/// @param[in] alignment  Alignment of the allocation, in bytes.  This must be a power of two.
/// @param[in] size       Number of bytes to allocate.
///
/// @return  Base address of the allocation if successful, null pointer if not.
///
/// @see Free()
void* Helium::MEMORY_HEAP_CLASS_NAME::AllocateAligned( size_t alignment, size_t size )
{
#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
    bool bLockedTracking = ConditionalVerboseTrackingLock();
#endif

#if USE_NEDMALLOC
    void* pMemory = nedalloc::nedpmemalign( static_cast< nedalloc::nedpool* >( m_pMspace ), alignment, size );
#else
    void* pMemory = mspace_memalign( m_pMspace, alignment, size );
#endif

#if HELIUM_ENABLE_MEMORY_TRACKING
    if( pMemory )
    {
        AddAllocation( pMemory );
    }
#endif

#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
    if( bLockedTracking )
    {
        VerboseTrackingUnlock();
    }
#endif

    return pMemory;
}

/// Free a block of memory previously allocated using Allocate(), Reallocate(), or AllocateAligned().
///
/// @param[in] pMemory  Base address of the allocation to free.  If this is a null pointer, no action will be
///                     performed.
///
/// @see Allocate(), Reallocate(), AllocateAligned()
void Helium::MEMORY_HEAP_CLASS_NAME::Free( void* pMemory )
{
#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
    bool bLockedTracking = false;
#endif

#if HELIUM_ENABLE_MEMORY_TRACKING
    if( pMemory )
    {
#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
        bLockedTracking = ConditionalVerboseTrackingLock();
#endif

        MEMORY_HEAP_CLASS_NAME* pHeap = GetAllocationHeap( pMemory );
        HELIUM_ASSERT( pHeap );
        if( pHeap )
        {
            pHeap->RemoveAllocation( pMemory );
        }
    }
#endif

#if USE_NEDMALLOC
    // nedmalloc does not accept null pointers.
    if( pMemory )
    {
        nedalloc::nedpfree( static_cast< nedalloc::nedpool* >( m_pMspace ), pMemory );
    }
#else
    mspace_free( m_pMspace, pMemory );
#endif

#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
    if( bLockedTracking )
    {
        VerboseTrackingUnlock();
    }
#endif
}

/// Get the size of an allocated memory block.
///
/// @param[in] pMemory  Base address of the allocation.
///
/// @return  Allocation size in bytes.
size_t Helium::MEMORY_HEAP_CLASS_NAME::GetMemorySize( void* pMemory )
{
#if USE_NEDMALLOC
    return malloc_usable_size( pMemory );
#else
    return mspace_usable_size( pMemory );
#endif
}

/// Release any thread caches created for the current thread in all existing memory heaps.
///
/// This should always be called from threads in which dynamic allocations may have been performed.
void Helium::MEMORY_HEAP_CLASS_NAME::UnregisterCurrentThreadCache()
{
    // Only applicable when using nedmalloc.
#if USE_NEDMALLOC
    ScopeReadLock readLock( GetGlobalHeapListLock() );

    for( MEMORY_HEAP_CLASS_NAME* pHeap = sm_pGlobalHeapListHead; pHeap != NULL; pHeap = pHeap->m_pNextHeap )
    {
        void* pMspace = pHeap->m_pMspace;
        HELIUM_ASSERT( pMspace );
        nedalloc::neddisablethreadcache( static_cast< nedalloc::nedpool* >( pMspace ) );
    }
#endif
}

/// Initialize this object, assuming all existing fields other than the name are in an uninitialized state.
///
/// @param[in] capacity  Fixed size (in bytes) of the memory heap to create, or zero to create a growable heap.
void Helium::MEMORY_HEAP_CLASS_NAME::ConstructNoName( size_t capacity )
{
#if USE_NEDMALLOC
    // XXX TMC TODO: Add support for the target number of threads (either determined programatically and/or through
    // a parameter).
    m_pMspace = nedalloc::nedcreatepool( capacity, 0 );
    HELIUM_ASSERT( m_pMspace );
#else
    m_pMspace = create_mspace( capacity, 1 );
    HELIUM_ASSERT( m_pMspace );

    // "extp" is unused, so we'll use it to point back to the heap instance.
    static_cast< mstate >( m_pMspace )->extp = this;
#endif

    {
        ScopeWriteLock writeLock( GetGlobalHeapListLock() );

        m_pPreviousHeap = NULL;

        MEMORY_HEAP_CLASS_NAME* pNextHeap = sm_pGlobalHeapListHead;
        m_pNextHeap = pNextHeap;
        if( pNextHeap )
        {
            pNextHeap->m_pPreviousHeap = this;
        }

        sm_pGlobalHeapListHead = this;
    }

#if HELIUM_ENABLE_MEMORY_TRACKING
    m_allocationCount = 0;
    m_bytesActual = 0;
#endif

#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
    bool bLockedTracking = ConditionalVerboseTrackingLock();

    bool bOldDisableBactraceTracking = sm_bDisableBacktraceTracking;
    sm_bDisableBacktraceTracking = true;

    size_t allocationSize = Align( sizeof( DynamicMemoryHeapVerboseTrackingData ), PhysicalMemory::GetPageSize() );
    DynamicMemoryHeapVerboseTrackingData* pVerboseTrackingData =
        static_cast< DynamicMemoryHeapVerboseTrackingData* >( PhysicalMemory::Allocate( allocationSize ) );
    HELIUM_ASSERT( pVerboseTrackingData );
    new( pVerboseTrackingData ) DynamicMemoryHeapVerboseTrackingData;

    m_pVerboseTrackingData = pVerboseTrackingData;

    sm_bDisableBacktraceTracking = bOldDisableBactraceTracking;

    if( bLockedTracking )
    {
        VerboseTrackingUnlock();
    }
#endif
}

#if HELIUM_ENABLE_MEMORY_TRACKING
/// Update memory usage stats for a new allocation.
///
/// @param[in] pMemory  Base address of the newly allocated memory.
///
/// @see RemoveAllocation()
void Helium::MEMORY_HEAP_CLASS_NAME::AddAllocation( void* pMemory )
{
    if( pMemory )
    {
#if USE_NEDMALLOC
        size_t byteCount = nedalloc::nedblksize( NULL, pMemory );
#else
        size_t byteCount = dlmalloc_usable_size( pMemory );
#endif

        size_t lastAllocationCount = m_allocationCount;
        size_t currentAllocationCount;
        do
        {
            currentAllocationCount = lastAllocationCount;
            lastAllocationCount = reinterpret_cast< size_t >( AtomicCompareExchangeUnsafe(
                reinterpret_cast< void* volatile& >( m_allocationCount ),
                reinterpret_cast< void* >( currentAllocationCount + 1 ),
                reinterpret_cast< void* >( currentAllocationCount ) ) );
        } while( currentAllocationCount != lastAllocationCount );

        size_t lastBytesActual = m_bytesActual;
        size_t currentBytesActual;
        do
        {
            currentBytesActual = lastBytesActual;
            lastAllocationCount = reinterpret_cast< size_t >( AtomicCompareExchangeUnsafe(
                reinterpret_cast< void* volatile& >( m_bytesActual ),
                reinterpret_cast< void* >( currentBytesActual + byteCount ),
                reinterpret_cast< void* >( currentBytesActual ) ) );
        } while( currentBytesActual != lastBytesActual );

#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
        if( !sm_bDisableBacktraceTracking )
        {
            sm_bDisableBacktraceTracking = true;

            AllocationBacktrace backtrace;
            MemoryZero( backtrace.pAddresses, sizeof( backtrace.pAddresses ) );
            Helium::GetStackTrace( backtrace.pAddresses, HELIUM_ARRAY_COUNT( backtrace.pAddresses ), 2 );
            m_pVerboseTrackingData->allocationBacktraceMap[ pMemory ] = backtrace;

            sm_bDisableBacktraceTracking = false;
        }
#endif
    }
}

/// Update memory usage for an allocation about to be freed.
///
/// @param[in] pMemory  Base address of the allocation about to be freed.
///
/// @see AddAllocation()
void Helium::MEMORY_HEAP_CLASS_NAME::RemoveAllocation( void* pMemory )
{
    if( pMemory )
    {
#if USE_NEDMALLOC
        size_t byteCount = nedalloc::nedblksize( NULL, pMemory );
#else
        size_t byteCount = dlmalloc_usable_size( pMemory );
#endif

        size_t lastAllocationCount = m_allocationCount;
        size_t currentAllocationCount;
        do
        {
            currentAllocationCount = lastAllocationCount;
            lastAllocationCount = reinterpret_cast< size_t >( AtomicCompareExchangeUnsafe(
                reinterpret_cast< void* volatile& >( m_allocationCount ),
                reinterpret_cast< void* >( currentAllocationCount - 1 ),
                reinterpret_cast< void* >( currentAllocationCount ) ) );
        } while( currentAllocationCount != lastAllocationCount );

        size_t lastBytesActual = m_bytesActual;
        size_t currentBytesActual;
        do
        {
            currentBytesActual = lastBytesActual;
            lastAllocationCount = reinterpret_cast< size_t >( AtomicCompareExchangeUnsafe(
                reinterpret_cast< void* volatile& >( m_bytesActual ),
                reinterpret_cast< void* >( currentBytesActual - byteCount ),
                reinterpret_cast< void* >( currentBytesActual ) ) );
        } while( currentBytesActual != lastBytesActual );

#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
        if( !sm_bDisableBacktraceTracking )
        {
            sm_bDisableBacktraceTracking = true;
            m_pVerboseTrackingData->allocationBacktraceMap.erase( pMemory );
            sm_bDisableBacktraceTracking = false;
        }
#endif
    }
}
#endif  // HELIUM_ENABLE_MEMORY_TRACKING

#if HELIUM_ENABLE_MEMORY_TRACKING
/// Search for the heap to which the given allocation belongs.
///
/// @param[in] pMemory  Allocation address.
///
/// @return  Pointer to the heap to which the allocation belongs.
Helium::MEMORY_HEAP_CLASS_NAME* Helium::MEMORY_HEAP_CLASS_NAME::GetAllocationHeap( void* pMemory )
{
    HELIUM_ASSERT( pMemory );

#if USE_NEDMALLOC
    // Not yet implemented.
    return NULL;
#elif FOOTERS
    mchunkptr pChunk = mem2chunk( pMemory );
    HELIUM_ASSERT( pChunk );

    mstate pMstate = get_mstate_for( pChunk );
    HELIUM_ASSERT( pMstate );

    MEMORY_HEAP_CLASS_NAME* pHeap = static_cast< MEMORY_HEAP_CLASS_NAME* >( pMstate->extp );
    HELIUM_ASSERT( pHeap );

    return pHeap;
#else
    ScopeReadLock readLock( GetGlobalHeapListLock() );

    for( MEMORY_HEAP_CLASS_NAME* pHeap = sm_pGlobalHeapListHead; pHeap != NULL; pHeap = pHeap->m_pNextHeap )
    {
        mstate* pMspace = pHeap->m_pMspace;
        HELIUM_ASSERT( pMspace );

        for( msegmentptr pSegment = &pMspace->seg; pSegment != NULL; pSegment = pSegment->next )
        {
            const char* pBase = pSegment->base;
            if( static_cast< const char* >( pMemory ) >= pBase &&
                static_cast< const char* >( pMemory ) < pBase + pSegment->size )
            {
                return pHeap;
            }
        }
    }

    return NULL;
#endif
}
#endif  // HELIUM_ENABLE_MEMORY_TRACKING
