//----------------------------------------------------------------------------------------------------------------------
// JobContext.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_JOB_CONTEXT_H
#define LUNAR_ENGINE_JOB_CONTEXT_H

#include "Engine/Engine.h"

#include "Platform/Trace.h"
#include "Foundation/Container/DynArray.h"
#include "Engine/JobManager.h"

#include <tbb/task.h>

namespace tbb
{
    class task;
}

namespace Lunar
{
    /// Context associated with a running job instance.
    class LUNAR_ENGINE_API JobContext : NonCopyable
    {
    public:
        /// Job execution callback type.
        typedef void ( JOB_EXECUTE_CALLBACK )( void* pJob, JobContext* pContext );

        /// Job context attachment data.
        class LUNAR_ENGINE_API AttachData
        {
        public:
            /// @name Construction/Destruction
            //@{
            inline AttachData();
            template< typename JobType > explicit AttachData( JobType* pJob );
            //@}

            /// @name Data Access
            //@{
            template< typename JobType > void Set( JobType* pJob );
            inline void Set( void* pData, JOB_EXECUTE_CALLBACK* pExecuteCallback );
            inline void Clear();

            inline void* GetData() const;
            inline JOB_EXECUTE_CALLBACK* GetExecuteCallback() const;
            //@}

        private:
            /// Job instance.
            void* m_pData;
            /// Job execution callback.
            JOB_EXECUTE_CALLBACK* m_pExecuteCallback;
        };

        /// Job spawner.
        ///
        /// Jobs can spawn any number of child jobs and up to one continuation job to be executed when all children have
        /// completed execution (along with any children they spawn).  To reduce dynamic memory allocations at runtime,
        /// this class takes an integer as a template parameter specifying the maximum number of child jobs to spawn.
        /// The spawner can be used to spawn less than the maximum count specified, including zero children.
        ///
        /// Note that continuation jobs must always be allocated before any children jobs are allocated.
        template< size_t ChildCountMax >
        class Spawner : NonCopyable
        {
        public:
            /// @name Construction/Destruction
            //@{
            Spawner( JobContext* pContext = NULL );
            ~Spawner();
            //@}

            /// @name Context Allocation
            //@{
            JobContext* Allocate();
            JobContext* AllocateContinuation();
            //@}

            /// @name Job Spawning
            //@{
            void Commit();
            //@}

        private:
            /// Job context.
            JobContext* m_pContext;
            /// Allocated continuation job context.
            JobContext* m_pContinuationContext;
            /// Allocated child job contexts.
            JobContext* m_childContexts[ ChildCountMax ];
            /// Number of allocated child jobs.
            size_t m_childContextCount;
        };

        /// @name Construction/Destruction
        //@{
        ~JobContext();
        //@}

        /// @name Job Allocation
        //@{
        template< typename JobType > JobType* Create();
        template< typename JobType > void Attach( JobType* pJob );
        inline void Attach( const AttachData& rAttachData );
        //@}

        /// @name Data Access
        //@{
        inline const AttachData& GetAttachData() const;
        //@}

    private:
        /// Job attachment data.
        AttachData m_attachData;

        /// TBB task.
        tbb::task* m_pTask;

        /// Currently active spawner (for checking; never dereferenced).
        void* m_pActiveSpawner;
        /// True if a continuation/child jobs have already been spawned.
        bool m_bAllocatedChildren;

        /// @name Construction/Destruction
        //@{
        JobContext();
        //@}

        /// @name TBB Task Allocation
        //@{
        tbb::task* AllocateChildTask( JobContext* pChildContext );
        tbb::task* AllocateContinuationTask( JobContext* pContinuationContext );
        //@}

        /// @name Static TBB Task Allocation
        //@{
        static tbb::task* AllocateRootTask( JobContext* pContext );
        //@}
    };
}

#include "Engine/JobContext.inl"

#endif  // LUNAR_ENGINE_JOB_CONTEXT_H
