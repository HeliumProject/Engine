#include "FrameworkImplPch.h"
#include "FrameworkImpl/AssetLoaderInitializationImpl.h"

#include "Engine/CacheManager.h"

#if HELIUM_TOOLS
# include "PcSupport/LooseAssetLoader.h"
# include "PcSupport/AssetPreprocessor.h"
# include "PreprocessingPc/PcPreprocessor.h"
#else
# include "Engine/CacheAssetLoader.h"
#endif

using namespace Helium;

/// @copydoc AssetLoaderInitialization::Startup()
void AssetLoaderInitializationImpl::Startup()
{
#if HELIUM_TOOLS
	LooseAssetLoader::Startup();

	AssetPreprocessor* pAssetPreprocessor = AssetPreprocessor::CreateStaticInstance();
	HELIUM_ASSERT( pAssetPreprocessor );
	PlatformPreprocessor* pPlatformPreprocessor = new PcPreprocessor;
	HELIUM_ASSERT( pPlatformPreprocessor );
	pAssetPreprocessor->SetPlatformPreprocessor( Cache::PLATFORM_PC, pPlatformPreprocessor );
#else
	CacheAssetLoader::Startup();
#endif
}

/// @copydoc AssetLoaderInitialization::Shutdown()
void AssetLoaderInitializationImpl::Shutdown()
{
#if HELIUM_TOOLS
	AssetPreprocessor::DestroyStaticInstance();
	LooseAssetLoader::Shutdown();
#else
	CacheAssetLoader::Shutdown();
#endif

	CacheManager::Shutdown();
}
