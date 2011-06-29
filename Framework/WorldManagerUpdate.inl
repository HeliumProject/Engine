//----------------------------------------------------------------------------------------------------------------------
// WorldManagerUpdate.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Maximum number of child jobs that can be spawned at once.
    static const size_t CHILD_JOB_COUNT_MAX = 1024;

    /// Run the WorldManagerUpdate job.
    ///
    /// @param[in] pContext  Context in which this job is running.
    template< typename EntityUpdateJobType >
    void WorldManagerUpdate< EntityUpdateJobType >::Run( JobContext* pContext )
    {
        HELIUM_ASSERT( pContext );

#if L_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
        WorldManager& rWorldManager = WorldManager::GetStaticInstance();
#endif

        JobContext::Spawner< CHILD_JOB_COUNT_MAX > spawner( pContext );

        size_t childJobCount = 0;

        size_t startLayerIndex = m_parameters.startLayerIndex;
        size_t startEntityIndex = m_parameters.startEntityIndex;

        const WorldPtr* pspWorlds = m_parameters.pspWorlds;
        size_t worldCount = m_parameters.worldCount;
        HELIUM_ASSERT( pspWorlds || worldCount == 0 );
        for( size_t worldIndex = 0; worldIndex < worldCount; ++worldIndex )
        {
            World* pWorld = pspWorlds[ worldIndex ];
            HELIUM_ASSERT( pWorld );
            size_t layerCount = pWorld->GetLayerCount();
            for( size_t layerIndex = startLayerIndex; layerIndex < layerCount; ++layerIndex )
            {
                Layer* pLayer = pWorld->GetLayer( layerIndex );
                HELIUM_ASSERT( pLayer );
                size_t entityCount = pLayer->GetEntityCount();
                for( size_t entityIndex = startEntityIndex; entityIndex < entityCount; ++entityIndex )
                {
                    Entity* pEntity = pLayer->GetEntity( entityIndex );
                    HELIUM_ASSERT( pEntity );
#if L_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
                    rWorldManager.SetCurrentThreadUpdateEntity( pEntity );
#endif
                    bool bUpdate = pEntity->NeedsAsynchronousUpdate();
#if L_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
                    rWorldManager.SetCurrentThreadUpdateEntity( NULL );
#endif
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
                            JobContext* pContinueContext = spawner.Allocate();
                            HELIUM_ASSERT( pContinueContext );
                            WorldManagerUpdate* pContinueJob = pContinueContext->Create< WorldManagerUpdate >();
                            HELIUM_ASSERT( pContinueJob );
                            WorldManagerUpdate::Parameters& rContinueParameters = pContinueJob->GetParameters();
                            rContinueParameters.pspWorlds = pspWorlds + worldIndex;
                            rContinueParameters.worldCount = worldCount - worldIndex;
                            rContinueParameters.startLayerIndex = layerIndex;
                            rContinueParameters.startEntityIndex = entityIndex + 1;

                            JobManager& rJobManager = JobManager::GetStaticInstance();
                            rJobManager.ReleaseJob( this );

                            return;
                        }
                    }
                }

                startEntityIndex = 0;
            }

            startLayerIndex = 0;
        }

        JobManager& rJobManager = JobManager::GetStaticInstance();
        rJobManager.ReleaseJob( this );
    }
}
