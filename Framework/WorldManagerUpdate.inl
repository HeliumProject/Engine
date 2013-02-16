//----------------------------------------------------------------------------------------------------------------------
// WorldManagerUpdate.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "Framework/World.h"
#include "Framework/Slice.h"

namespace Helium
{
    /// Maximum number of child jobs that can be spawned at once.
    static const size_t CHILD_JOB_COUNT_MAX = 1024;

    /// Run the WorldManagerUpdate job.
    ///
    /// @param[in] pContext  Context in which this job is running.
    //template< typename EntityUpdateJobType >
    //void WorldManagerUpdate< EntityUpdateJobType >::Run( JobContext* pContext )

    template< typename WorldUpdateJobType, typename EntityUpdateJobType >
    void RunTask(JobContext* pContext, WorldManagerUpdateParameters &m_parameters, WorldUpdateJobType *job)
    {
        HELIUM_ASSERT( pContext );

#if HELIUM_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
        WorldManager& rWorldManager = WorldManager::GetStaticInstance();
#endif

        JobContext::Spawner< CHILD_JOB_COUNT_MAX > spawner( pContext );

        size_t childJobCount = 0;

        size_t startSliceIndex = m_parameters.startSliceIndex;
        size_t startEntityIndex = m_parameters.startEntityIndex;

        const WorldPtr* pspWorlds = m_parameters.pspWorlds;
        size_t worldCount = m_parameters.worldCount;
        HELIUM_ASSERT( pspWorlds || worldCount == 0 );
        for( size_t worldIndex = 0; worldIndex < worldCount; ++worldIndex )
        {
            World* pWorld = pspWorlds[ worldIndex ];
            HELIUM_ASSERT( pWorld );
            size_t sliceCount = pWorld->GetSliceCount();
            for( size_t sliceIndex = startSliceIndex; sliceIndex < sliceCount; ++sliceIndex )
            {
                Slice* pSlice = pWorld->GetSlice( sliceIndex );
                HELIUM_ASSERT( pSlice );
                size_t entityCount = pSlice->GetEntityCount();
                for( size_t entityIndex = startEntityIndex; entityIndex < entityCount; ++entityIndex )
                {
                    EntityDefinition* pEntity = pSlice->GetEntity( entityIndex );
                    HELIUM_ASSERT( pEntity );
#if 0
#if HELIUM_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
                    rWorldManager.SetCurrentThreadUpdateEntity( pEntity );
#endif
                    bool bUpdate = pEntity->NeedsAsynchronousUpdate();
#if HELIUM_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
                    rWorldManager.SetCurrentThreadUpdateEntity( NULL );
#endif
#endif
                    bool bUpdate = true;

                    if( bUpdate )
                    {
                        JobContext* pChildContext = spawner.Allocate();
                        HELIUM_ASSERT( pChildContext );
                        EntityUpdateJobType* pChildJob = pChildContext->Create< EntityUpdateJobType >();
                        HELIUM_ASSERT( pChildJob );
                        EntityUpdateJobType::Parameters& rParameters = pChildJob->GetParameters();
                        rParameters.pEntity = pEntity;

                        // If we've allocated the maximum number of entity update jobs for this job, continue spawning
                        // entity updates in a child job.
                        ++childJobCount;
                        HELIUM_ASSERT( childJobCount < CHILD_JOB_COUNT_MAX );
                        if( childJobCount >= CHILD_JOB_COUNT_MAX + 1 )
                        {
                            // This should probably be AllocateContinuation()
                            JobContext* pContinueContext = spawner.Allocate();
                            HELIUM_ASSERT( pContinueContext );
                            WorldUpdateJobType* pContinueJob = pContinueContext->Create< WorldUpdateJobType >();
                            HELIUM_ASSERT( pContinueJob );
                            WorldUpdateJobType::Parameters& rContinueParameters = pContinueJob->GetParameters();
                            rContinueParameters.pspWorlds = pspWorlds + worldIndex;
                            rContinueParameters.worldCount = worldCount - worldIndex;
                            rContinueParameters.startSliceIndex = sliceIndex;
                            rContinueParameters.startEntityIndex = entityIndex + 1;

                            JobManager& rJobManager = JobManager::GetStaticInstance();
                            rJobManager.ReleaseJob( job );

                            return;
                        }
                    }
                }

                startEntityIndex = 0;
            }

            startSliceIndex = 0;
        }

        JobManager& rJobManager = JobManager::GetStaticInstance();
        rJobManager.ReleaseJob( job );
    }

    template <>
    void JobBase<WorldManagerUpdateParameters_PreUpdate>::Run( JobContext* pContext )
    {
        RunTask<WorldManagerPreUpdate, EntityPreUpdate>(pContext, m_parameters, this);
    }
    
    template <>
    void JobBase<WorldManagerUpdateParameters_PostUpdate>::Run( JobContext* pContext )
    {
        RunTask<WorldManagerPostUpdate, EntityPreUpdate>(pContext, m_parameters, this);
    }
}
