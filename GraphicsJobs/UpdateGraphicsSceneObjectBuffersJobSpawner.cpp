//----------------------------------------------------------------------------------------------------------------------
// UpdateGraphicsSceneObjectBuffersJobSpawner.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsJobsPch.h"
#include "GraphicsJobs/GraphicsJobsInterface.h"

#include "Engine/JobContext.h"

/// Maximum number of child jobs to spawn at once.
static const uint_fast32_t SCENE_OBJECT_CHILD_JOB_MAX = 128;
/// Maximum number of graphics scene objects to update in each child job.
static const uint_fast32_t SCENE_OBJECT_CHILD_JOB_OBJECT_COUNT_MAX = 100;


using namespace Lunar;

/// Spawn jobs to update the constant buffer data for all graphics scene objects.
///
/// @param[in] pContext  Context in which this job is running.
void UpdateGraphicsSceneObjectBuffersJobSpawner::Run( JobContext* pContext )
{
    HELIUM_ASSERT( pContext );

    const GraphicsSceneObject* pSceneObjects = m_parameters.pSceneObjects;
    float32_t* const* ppConstantBufferData = m_parameters.ppConstantBufferData;

    uint_fast32_t sceneObjectCount = m_parameters.sceneObjectCount;

    uint_fast32_t jobCount = ( sceneObjectCount + SCENE_OBJECT_CHILD_JOB_OBJECT_COUNT_MAX - 1 ) /
        SCENE_OBJECT_CHILD_JOB_OBJECT_COUNT_MAX;
    if( jobCount > SCENE_OBJECT_CHILD_JOB_MAX )
    {
        jobCount = SCENE_OBJECT_CHILD_JOB_MAX;
    }

    {
        JobContext::Spawner< SCENE_OBJECT_CHILD_JOB_MAX > childSpawner( pContext );;

        for( uint_fast32_t jobIndex = 0; jobIndex < jobCount; ++jobIndex )
        {
            JobContext* pChildContext = childSpawner.Allocate();
            HELIUM_ASSERT( pChildContext );
            UpdateGraphicsSceneObjectBuffersJob* pJob =
                pChildContext->Create< UpdateGraphicsSceneObjectBuffersJob >();
            HELIUM_ASSERT( pJob );

            uint_fast32_t jobObjectCount = Min( sceneObjectCount, SCENE_OBJECT_CHILD_JOB_OBJECT_COUNT_MAX );
            HELIUM_ASSERT( jobObjectCount != 0 );
            sceneObjectCount -= jobObjectCount;

            UpdateGraphicsSceneObjectBuffersJob::Parameters& rParameters = pJob->GetParameters();
            rParameters.sceneObjectCount = static_cast< uint32_t >( jobObjectCount );
            rParameters.pSceneObjects = pSceneObjects;
            rParameters.ppConstantBufferData = ppConstantBufferData;

            pSceneObjects += jobObjectCount;
            ppConstantBufferData += jobObjectCount;
        }

        if( sceneObjectCount != 0 )
        {
            JobContext* pContinuationContext = childSpawner.AllocateContinuation();
            HELIUM_ASSERT( pContinuationContext );
            UpdateGraphicsSceneObjectBuffersJobSpawner* pContinuationJob =
                pContinuationContext->Create< UpdateGraphicsSceneObjectBuffersJobSpawner >();
            HELIUM_ASSERT( pContinuationJob );

            UpdateGraphicsSceneObjectBuffersJobSpawner::Parameters& rParameters = pContinuationJob->GetParameters();
            rParameters.sceneObjectCount = sceneObjectCount;
            rParameters.pSceneObjects = pSceneObjects;
            rParameters.ppConstantBufferData = ppConstantBufferData;
        }
    }

    JobManager& rJobManager = JobManager::GetStaticInstance();
    rJobManager.ReleaseJob( this );
}
