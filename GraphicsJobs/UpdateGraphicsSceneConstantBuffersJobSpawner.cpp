//----------------------------------------------------------------------------------------------------------------------
// UpdateGraphicsSceneConstantBuffersJobSpawner.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsJobsPch.h"
#include "GraphicsJobs/GraphicsJobsInterface.h"

#include "Engine/JobContext.h"

/// Maximum number of jobs to spawn at once for scene instance buffer updates.
#define GRAPHICS_SCENE_INSTANCE_UPDATE_JOB_MAX 128

using namespace Lunar;

/// Spawn jobs to update all instance constant buffers for graphics scene objects and sub-meshes.
///
/// @param[in] pContext  Context in which this job is running.
void UpdateGraphicsSceneConstantBuffersJobSpawner::Run( JobContext* pContext )
{
    HELIUM_ASSERT( pContext );

    {
        JobContext::Spawner< 2 > childSpawner( pContext );

        JobContext* pChildContext = childSpawner.Allocate();
        HELIUM_ASSERT( pChildContext );
        UpdateGraphicsSceneObjectBuffersJobSpawner* pObjectJob =
            pChildContext->Create< UpdateGraphicsSceneObjectBuffersJobSpawner >();
        HELIUM_ASSERT( pObjectJob );

        UpdateGraphicsSceneObjectBuffersJobSpawner::Parameters& rObjectParameters = pObjectJob->GetParameters();
        rObjectParameters.sceneObjectCount = m_parameters.sceneObjectCount;
        rObjectParameters.pSceneObjects = m_parameters.pSceneObjects;
        rObjectParameters.ppConstantBufferData = m_parameters.ppSceneObjectConstantBufferData;

        pChildContext = childSpawner.Allocate();
        HELIUM_ASSERT( pChildContext );
        UpdateGraphicsSceneSubMeshBuffersJobSpawner* pSubMeshJob =
            pChildContext->Create< UpdateGraphicsSceneSubMeshBuffersJobSpawner >();
        HELIUM_ASSERT( pSubMeshJob );

        UpdateGraphicsSceneSubMeshBuffersJobSpawner::Parameters& rSubMeshParameters = pSubMeshJob->GetParameters();
        rSubMeshParameters.subMeshCount = m_parameters.subMeshCount;
        rSubMeshParameters.pSubMeshes = m_parameters.pSubMeshes;
        rSubMeshParameters.pSceneObjects = m_parameters.pSceneObjects;
        rSubMeshParameters.ppConstantBufferData = m_parameters.ppSubMeshConstantBufferData;
    }

    JobManager& rJobManager = JobManager::GetStaticInstance();
    rJobManager.ReleaseJob( this );
}
