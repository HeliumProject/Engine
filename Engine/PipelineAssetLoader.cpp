#include "EnginePch.h"
#include "PipelineAssetLoader.h"
#include "PipelineMessages.h"

#include "Platform/File.h"
#include "Foundation/FilePath.h"
#include "Engine/FileLocations.h"
#include "Engine/Config.h"
#include "Engine/Resource.h"
#include "Engine/PipelinePackageLoader.h"
#include "Engine/PipelineClient.h"
#include "Foundation/DirectoryIterator.h"


using namespace Helium;
using namespace Helium::Pipeline;

PipelineAssetLoader* Helium::PipelineAssetLoader::sm_pPipelineInstance = NULL;

/// Constructor.
PipelineAssetLoader::PipelineAssetLoader()
{
	PipelineClient::InitializeStaticInstance();
}

/// Destructor.
PipelineAssetLoader::~PipelineAssetLoader()
{
	PipelineClient::DestroyStaticInstance();
}

/// Initialize the static object loader instance as an PipelineAssetLoader.
///
/// @return  True if the loader was initialized successfully, false if not or another object loader instance already
///          exists.
bool PipelineAssetLoader::InitializeStaticInstance()
{
	if( sm_pInstance )
	{
		return false;
	}

	sm_pPipelineInstance = new PipelineAssetLoader;
	sm_pInstance = sm_pPipelineInstance;
	HELIUM_ASSERT( sm_pInstance );

	return true;
}

/// @copydoc AssetLoader::GetPackageLoader()
PackageLoader* PipelineAssetLoader::GetPackageLoader( AssetPath path )
{
	return GetPipelinePackageLoader( path );
}

PipelinePackageLoader *PipelineAssetLoader::GetPipelinePackageLoader( AssetPath path )
{
	PipelinePackageLoader* pLoader = m_packageLoaderMap.GetPackageLoader( path );

	return pLoader;
}

/// @copydoc AssetLoader::TickPackageLoaders()
void PipelineAssetLoader::TickPackageLoaders()
{
	PipelineClient::GetStaticInstance()->Tick();
	m_packageLoaderMap.TickPackageLoaders();
}
