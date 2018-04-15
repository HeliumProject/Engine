#include "Precompile.h"
#include "GraphicsJobs/GraphicsJobsInterface.h"

/// Maximum number of child jobs to spawn at once.
static const uint_fast32_t SUB_MESH_CHILD_JOB_MAX = 128;
/// Maximum number of sub-meshes to update in each child job.
static const uint_fast32_t SUB_MESH_CHILD_JOB_OBJECT_COUNT_MAX = 100;

using namespace Helium;

/// Spawn jobs to update the constant buffer data for all graphics scene object sub-meshes.
///
/// @param[in] pContext  Context in which this job is running.
void UpdateGraphicsSceneSubMeshBuffersJobSpawner::Run()
{
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
        for( uint_fast32_t jobIndex = 0; jobIndex < jobCount; ++jobIndex )
        {
			// These were all in parallel but TBB was removed
            uint_fast32_t jobObjectCount = Min( subMeshCount, SUB_MESH_CHILD_JOB_OBJECT_COUNT_MAX );
            HELIUM_ASSERT( jobObjectCount != 0 );
            subMeshCount -= jobObjectCount;

			UpdateGraphicsSceneSubMeshBuffersJob job;
            UpdateGraphicsSceneSubMeshBuffersJob::Parameters& rParameters = job.GetParameters();
            rParameters.subMeshCount = static_cast< uint32_t >( jobObjectCount );
            rParameters.pSubMeshes = pSubMeshes;
            rParameters.pSceneObjects = pSceneObjects;
            rParameters.ppConstantBufferData = ppConstantBufferData;
			job.Run();

            pSubMeshes += jobObjectCount;
            ppConstantBufferData += jobObjectCount;
        }

        if( subMeshCount != 0 )
        {
			// NOTE: This was a continuation job from above but is now inline since TBB was removed
			UpdateGraphicsSceneSubMeshBuffersJobSpawner job;
            UpdateGraphicsSceneSubMeshBuffersJobSpawner::Parameters& rParameters =
                job.GetParameters();
            rParameters.subMeshCount = subMeshCount;
            rParameters.pSubMeshes = pSubMeshes;
            rParameters.pSceneObjects = pSceneObjects;
            rParameters.ppConstantBufferData = ppConstantBufferData;
			job.Run();
        }
    }
}
