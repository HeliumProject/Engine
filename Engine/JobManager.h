//----------------------------------------------------------------------------------------------------------------------
// JobManager.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_ENGINE_JOB_MANAGER_H
#define HELIUM_ENGINE_JOB_MANAGER_H

#include "Engine/JobPool.h"

#include "Platform/Thread.h"

#ifndef HELIUM_TRACK_JOB_POOL_HITS
/// Set to non-zero to track stats on which job allocations pull from an already pooled job object and which require a
/// new job allocation.
#define HELIUM_TRACK_JOB_POOL_HITS ( HELIUM_DEBUG )
#endif

namespace Helium
{
    /// Job manager.
    class HELIUM_ENGINE_API JobManager : NonCopyable
    {
    public:
        /// @name Initialization
        //@{
        bool Initialize();
        void Shutdown();
        //@}

        /// @name Job Allocation
        //@{
        template< typename T > T* AllocateJob();
        template< typename T > void ReleaseJob( T* pJob );

        void* AllocateJobUninitialized( size_t size );
        void ReleaseJobUninitialized( void* pJob, size_t size );
        //@}

        /// @name Static Access
        //@{
        static JobManager& GetStaticInstance();
        static void DestroyStaticInstance();
        //@}

    private:
        /// Thread-local job pool list node.
        struct PoolNode
        {
            /// Job pool.
            JobPool pool;
            /// Next pool in the list.
            PoolNode* volatile pNext;
#if HELIUM_TRACK_JOB_POOL_HITS
            /// Hits on jobs pulled from the local thread's pool.
            uint32_t localHits;
            /// Hits on jobs pulled from other threads' pools.
            uint32_t stolenHits;
            /// Misses (new jobs needed to be allocated).
            uint32_t misses;
#endif
        };

        /// List of job pools for each thread.
        PoolNode* volatile m_pHeadPool;
        /// Thread-local storage for job pool data.
        ThreadLocalPointer m_poolTls;

        /// Job manager instance.
        static JobManager* sm_pInstance;

        /// @name Construction/Destruction
        //@{
        JobManager();
        ~JobManager();
        //@}

        /// @name Private Utility Functions
        //@{
        PoolNode* GetThreadLocalPoolNode();
        //@}
    };
}

#include "Engine/JobManager.inl"

#endif  // HELIUM_ENGINE_JOB_MANAGER_H
