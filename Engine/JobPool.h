//----------------------------------------------------------------------------------------------------------------------
// JobPool.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_ENGINE_JOB_POOL_H
#define HELIUM_ENGINE_JOB_POOL_H

#include "Engine/Engine.h"

#include "Foundation/ConcurrentHashMap.h"

namespace Helium
{
    /// Pool of job objects.
    ///
    /// While each thread will have its own job pool in order to minimize thread contention during job object allocation
    /// and deallocation, pools are also internally thread-safe in order to allow threads to grab available jobs from
    /// other pools.
    class HELIUM_ENGINE_API JobPool : NonCopyable
    {
    public:
        /// @name Construction/Destruction
        //@{
        JobPool();
        ~JobPool();
        //@}

        /// @name Job Allocation
        //@{
        template< typename T > T* Acquire();
        template< typename T > void Release( T* pJob );

        void* AcquireUninitialized( size_t size );
        void ReleaseUninitialized( void* pJob, size_t size );
        //@}

        /// @name Static Allocation Utility Functions
        //@{
        template< typename T > static T* NewJob();
        static void* NewJobUninitialized( size_t size );
        //@}

    private:
        /// Job allocation header.
        struct AllocationHeader
        {
            /// Next allocation in the pool of the same size.
            AllocationHeader* volatile pNext;
        };

        /// Allocation pool hash map type (allocations keyed by job size).
        typedef ConcurrentHashMap< size_t, AllocationHeader* > AllocationPoolMap;

        /// Allocation pool lookup.
        AllocationPoolMap m_allocationPools;
    };
}

#include "Engine/JobPool.inl"

#endif  // HELIUM_ENGINE_JOB_POOL_H
