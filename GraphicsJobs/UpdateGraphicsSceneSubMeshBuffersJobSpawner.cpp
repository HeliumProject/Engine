//----------------------------------------------------------------------------------------------------------------------
// UpdateGraphicsSceneSubMeshBuffersJobSpawner.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsJobsPch.h"
#include "GraphicsJobs/GraphicsJobsInterface.h"

#include "Engine/JobContext.h"

/// Maximum number of child jobs to spawn at once.
static const uint_fast32_t SUB_MESH_CHILD_JOB_MAX = 128;
/// Maximum number of sub-meshes to update in each child job.
static const uint_fast32_t SUB_MESH_CHILD_JOB_OBJECT_COUNT_MAX = 100;

using namespace Lunar;

/// Spawn jobs to update the constant buffer data for all graphics scene object sub-meshes.
///
/// @param[in] pContext  Context in which this job is running.
void UpdateGraphicsSceneSubMeshBuffersJobSpawner::Run( JobContext* pContext )
{
    HELIUM_ASSERT( pContext );

    const GraphicsSceneObject::SubMeshData* pSubMeshes = m_parameters.pSubMeshes;
    float32_t* const* ppConstantBufferData = m_parameters.ppConstantBufferData;

    const GraphicsSceneObject* pSceneObjects = m_parameters.pSceneObjects;

    uint_fast32_t subMeshCount = m_parameters.subMeshCount;

    uint_fast32_t jobCount = ( subMeshCount + SUB_MESH_CHILD_JOB_OBJECT_COUNT_MAX - 1 ) /
        SUB_MESH_CHILD_JOB_OBJECT_COUNT_MAX;
    if( jobCount > SUB_MESH_CHILD_JOB_MAX )
    {
        jobCount = SUB_MESH_CHILD_JOB_MAX;
    }

    {
        JobContext::Spawner< SUB_MESH_CHILD_JOB_MAX > childSpawner( pContext );;

        for( uint_fast32_t jobIndex = 0; jobIndex < jobCount; ++jobIndex )
        {
            JobContext* pChildContext = childSpawner.Allocate();
            HELIUM_ASSERT( pChildContext );
            UpdateGraphicsSceneSubMeshBuffersJob* pJob =
                pChildContext->Create< UpdateGraphicsSceneSubMeshBuffersJob >();
            HELIUM_ASSERT( pJob );

            uint_fast32_t jobObjectCount = Min( subMeshCount, SUB_MESH_CHILD_JOB_OBJECT_COUNT_MAX );
            HELIUM_ASSERT( jobObjectCount != 0 );
            subMeshCount -= jobObjectCount;

            UpdateGraphicsSceneSubMeshBuffersJob::Parameters& rParameters = pJob->GetParameters();
            rParameters.subMeshCount = static_cast< uint32_t >( jobObjectCount );
            rParameters.pSubMeshes = pSubMeshes;
            rParameters.pSceneObjects = pSceneObjects;
            rParameters.ppConstantBufferData = ppConstantBufferData;

            pSubMeshes += jobObjectCount;
            ppConstantBufferData += jobObjectCount;
        }

        if( subMeshCount != 0 )
        {
            JobContext* pContinuationContext = childSpawner.AllocateContinuation();
            HELIUM_ASSERT( pContinuationContext );
            UpdateGraphicsSceneSubMeshBuffersJobSpawner* pContinuationJob =
                pContinuationContext->Create< UpdateGraphicsSceneSubMeshBuffersJobSpawner >();
            HELIUM_ASSERT( pContinuationJob );

            UpdateGraphicsSceneSubMeshBuffersJobSpawner::Parameters& rParameters =
                pContinuationJob->GetParameters();
            rParameters.subMeshCount = subMeshCount;
            rParameters.pSubMeshes = pSubMeshes;
            rParameters.pSceneObjects = pSceneObjects;
            rParameters.ppConstantBufferData = ppConstantBufferData;
        }
    }

    JobManager& rJobManager = JobManager::GetStaticInstance();
    rJobManager.ReleaseJob( this );
}
