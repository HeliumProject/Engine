#include "Precompile.h"
#include "GraphicsJobs/GraphicsJobsInterface.h"

/// Maximum number of child jobs to spawn at once.
static const uint_fast32_t SCENE_OBJECT_CHILD_JOB_MAX = 128;
/// Maximum number of graphics scene objects to update in each child job.
static const uint_fast32_t SCENE_OBJECT_CHILD_JOB_OBJECT_COUNT_MAX = 100;


using namespace Helium;

/// Spawn jobs to update the constant buffer data for all graphics scene objects.
void UpdateGraphicsSceneObjectBuffersJobSpawner::Run()
{

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
        for( uint_fast32_t jobIndex = 0; jobIndex < jobCount; ++jobIndex )
        {
            uint_fast32_t jobObjectCount = Min( sceneObjectCount, SCENE_OBJECT_CHILD_JOB_OBJECT_COUNT_MAX );
            HELIUM_ASSERT( jobObjectCount != 0 );
            sceneObjectCount -= jobObjectCount;

			// NOTE: These were run in parallel but now synchronous since TBB has been removed
			UpdateGraphicsSceneObjectBuffersJob job;
            UpdateGraphicsSceneObjectBuffersJob::Parameters& rParameters = job.GetParameters();
            rParameters.sceneObjectCount = static_cast< uint32_t >( jobObjectCount );
            rParameters.pSceneObjects = pSceneObjects;
            rParameters.ppConstantBufferData = ppConstantBufferData;
			job.Run();

            pSceneObjects += jobObjectCount;
            ppConstantBufferData += jobObjectCount;
        }

		// This was a continuation task but now just exectues inline here since TBB was removed
        if( sceneObjectCount != 0 )
        {
			UpdateGraphicsSceneObjectBuffersJobSpawner job;
            UpdateGraphicsSceneObjectBuffersJobSpawner::Parameters& rParameters = job.GetParameters();
            rParameters.sceneObjectCount = sceneObjectCount;
            rParameters.pSceneObjects = pSceneObjects;
            rParameters.ppConstantBufferData = ppConstantBufferData;
			job.Run();
        }
    }
}
