#include "EnginePch.h"
#include "Engine/JobPool.h"

#include "Platform/Atomic.h"

using namespace Helium;

/// Constructor.
JobPool::JobPool()
{
}

/// Destructor.
JobPool::~JobPool()
{
    // Free all pooled job memory.
    DefaultAllocator allocator;

    AllocationPoolMap::Accessor poolAccessor;
    for( m_allocationPools.First( poolAccessor ); poolAccessor.IsValid(); ++poolAccessor )
    {
        AllocationHeader* pHeader = poolAccessor->Second();
        while( pHeader )
        {
            AllocationHeader* pNext = pHeader->pNext;
            allocator.Free( pHeader );
            pHeader = pNext;
        }
    }
}

/// Acquire an unused job allocation of the specified size from this pool.
///
/// Unlike Acquire(), this will only return uninitialized memory.  It is the caller's responsibility to check the
/// returned pointer and initialize the allocation using the placement "new" operator for the job type before using
/// the job object.
///
/// @param[in] size  Job allocation size.
///
/// @return  Pointer to the allocation if one could be located, null if no allocations for the specified size could
///          be located.  Note that the allocation must be initialized using the placement "new" operator for the
///          job type before using it.
///
/// @see ReleaseUninitialized(), Acquire(), Release()
void* JobPool::AcquireUninitialized( size_t size )
{
    // Make sure the job size is at least large enough for the header data.
    if( size < sizeof( AllocationHeader ) )
    {
        size = sizeof( AllocationHeader );
    }

    // Locate an existing entry in the allocation pool lookup.
    AllocationPoolMap::ConstAccessor poolAccessor;
    if( !m_allocationPools.Find( poolAccessor, size ) )
    {
        return NULL;
    }

    // Pop the allocation off the head of the pool.
    AllocationHeader* pTestHeader;
    AllocationHeader* pHeader = poolAccessor->Second();
    do
    {
        pTestHeader = pHeader;
        if( !pTestHeader )
        {
            return NULL;
        }

        AllocationHeader* pNext = pTestHeader->pNext;
        HELIUM_ASSERT( pNext != pTestHeader );

        pHeader = AtomicCompareExchangeRelease(
            const_cast< AllocationHeader*& >( poolAccessor->Second() ),
            pNext,
            pTestHeader );
    } while( pHeader != pTestHeader );

    return pHeader;
}

/// Release a job allocation to the pool of the appropriate size.
///
/// Unlike Release(), this does not call the destructor on the job object.  It is the caller's responsibility to
/// call the job object's destructor prior to calling this function.
///
/// @param[in] pJob  Job allocation to release.  Note that job instances should have their destructors explicitly
///                  called before passing to this function.
/// @param[in] size  Job allocation size.
///
/// @see AcquireUninitialized(), Release(), Acquire()
void JobPool::ReleaseUninitialized( void* pJob, size_t size )
{
    HELIUM_ASSERT( pJob );

    // Make sure the job size is at least large enough for the header data.
    if( size < sizeof( AllocationHeader ) )
    {
        size = sizeof( AllocationHeader );
    }

    // Initialize the job allocation header.
    AllocationHeader* pHeader = static_cast< AllocationHeader* >( pJob );
    pHeader->pNext = NULL;

    // Update the appropriate allocation pool, initializing a new pool if a pool for the specified size does not yet
    // exist.
    KeyValue< size_t, AllocationHeader* > poolEntry( size, pHeader );

    AllocationPoolMap::ConstAccessor poolAccessor;
    if( !m_allocationPools.Insert( poolAccessor, poolEntry ) )
    {
        AllocationHeader* pTestNext;
        AllocationHeader* pNext = poolAccessor->Second();
        do
        {
            pTestNext = pNext;
            pHeader->pNext = pTestNext;

            pNext = AtomicCompareExchangeRelease(
                const_cast< AllocationHeader*& >( poolAccessor->Second() ),
                pHeader,
                pTestNext );
        } while( pNext != pTestNext );
    }
}

/// Allocate uninitialized memory for a new job instance outside the pool that is compatible with pool usage.
///
/// When jobs are added to a pool using Release() or ReleaseUninitialized(), the memory for the job is reused for
/// data relevant to storage within the pool.  This imposes a minimum size requirement for the memory allocated for
/// a job.  Using this function to allocate a job ensures that the size of the block of memory allocated meets this
/// requirement.
///
/// Note that this should only be used when no existing allocation can be located in any previously existing pool.
/// The Acquire() or AcquireUninitialized() method should first be used on potential pools to attempt to reuse an
/// existing allocation before falling back on this function.
///
/// Job memory stored in a pool will be released when the pool is destroyed, so no equivalent DeleteJob() function
/// is necessary, although code that works with jobs should make sure to properly release the job back to a pool
/// when it is no longer needed prior to shutdown.
///
/// @param[in] size  Size needed for the job instance.
///
/// @return  Pointer to newly allocated memory for a job of the specified size.
///
/// @see NewJob(), Acquire(), Release(), AcquireUninitialized(), ReleaseUninitialized()
void* JobPool::NewJobUninitialized( size_t size )
{
    if( size < sizeof( AllocationHeader ) )
    {
        size = sizeof( AllocationHeader );
    }

    void* pJob = DefaultAllocator().AllocateAligned( HELIUM_SIMD_ALIGNMENT, size );

    return pJob;
}
