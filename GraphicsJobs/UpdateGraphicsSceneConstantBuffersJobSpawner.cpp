#include "Precompile.h"
#include "GraphicsJobs/GraphicsJobsInterface.h"

/// Maximum number of jobs to spawn at once for scene instance buffer updates.
#define GRAPHICS_SCENE_INSTANCE_UPDATE_JOB_MAX 128

using namespace Helium;

/// Spawn jobs to update all instance constant buffers for graphics scene objects and sub-meshes.
///
/// @param[in] pContext  Context in which this job is running.
void UpdateGraphicsSceneConstantBuffersJobSpawner::Run()
{
	{
		// NOTE: These were running in parallel, but now synchronous since we're removing tbb
		UpdateGraphicsSceneObjectBuffersJobSpawner objectJob;
		UpdateGraphicsSceneObjectBuffersJobSpawner::Parameters& rObjectParameters = objectJob.GetParameters();
		rObjectParameters.sceneObjectCount = m_parameters.sceneObjectCount;
		rObjectParameters.pSceneObjects = m_parameters.pSceneObjects;
		rObjectParameters.ppConstantBufferData = m_parameters.ppSceneObjectConstantBufferData;
		objectJob.Run();

		UpdateGraphicsSceneSubMeshBuffersJobSpawner subMeshJob;
		UpdateGraphicsSceneSubMeshBuffersJobSpawner::Parameters& rSubMeshParameters = subMeshJob.GetParameters();
		rSubMeshParameters.subMeshCount = m_parameters.subMeshCount;
		rSubMeshParameters.pSubMeshes = m_parameters.pSubMeshes;
		rSubMeshParameters.pSceneObjects = m_parameters.pSceneObjects;
		rSubMeshParameters.ppConstantBufferData = m_parameters.ppSubMeshConstantBufferData;
		subMeshJob.Run();
	}
}
