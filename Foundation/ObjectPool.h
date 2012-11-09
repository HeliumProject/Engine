#pragma once

#include "Platform/Locks.h"

#include "Foundation/API.h"

namespace Helium
{
    /// Thread-safe object allocation pool.
    ///
    /// This provides a mechanism for managing a pool from which objects of a single type can be created and destroyed.
    /// Objects are allocated in blocks, and released objects are never destroyed until the pool itself is destroyed.
    /// Synchronization of object requests and releases is handled using a spin lock in order to attempt to minimize the
    /// overhead of allocating an object.  Synchronization with block allocation is performed using a read-write lock;
    /// allocation and release calls always acquire a non-exclusive read-only lock, while an exclusive write lock is
    /// acquired when the pool is empty and a new block needs to be allocated.
    template< typename T, typename Allocator = DefaultAllocator >
    class ObjectPool : NonCopyable
    {
    public:
        /// @name Construction/Destruction
        //@{
        ObjectPool( size_t blockSize, size_t blockCountMax = Invalid< size_t >() );
        ~ObjectPool();
        //@}

        /// @name Allocation Interface
        //@{
        T* Allocate();
        void Release( T* pObject );
        //@}

        /// @name Indexing
        //@{
        size_t GetIndex( T* pObject ) const;
        T* GetObject( size_t index ) const;
        //@}

    private:
        /// Pool block.
        struct Block
        {
            /// Allocated objects.
            T* pObjects;
            /// Next pool block.
            Block* pNext;
        };

        /// Free object list.
        T* volatile * volatile m_ppFreeObjects;
        /// Free object count.
        volatile size_t m_freeObjectCount;
        /// Free object list spin lock.
        SpinLock m_freeObjectSpinLock;

        /// Read-write lock for synchronizing with block allocations.
        mutable ReadWriteLock m_poolBlockAllocationLock;

        /// Allocated block list head.
        Block* volatile m_pHeadBlock;

        /// Number of objects per block.
        size_t m_blockSize;
        /// Number of allocated blocks.
        volatile size_t m_allocatedBlockCount;
        /// Maximum number of blocks that can be allocated.
        size_t m_blockCountMax;

        /// @name Utility Functions
        //@{
        void AllocateBlock();
        //@}
    };
}

#include "Foundation/ObjectPool.inl"
