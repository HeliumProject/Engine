#include "FrameworkImplPch.h"
#include "FrameworkImpl/AssetLoaderInitializationImpl.h"

#if HELIUM_TOOLS
# include "PcSupport/LooseAssetLoader.h"
# include "PcSupport/AssetPreprocessor.h"
# include "PreprocessingPc/PcPreprocessor.h"
#else
# include "Engine/CacheAssetLoader.h"
#endif

using namespace Helium;

/// @copydoc AssetLoaderInitialization::Initialize()
AssetLoader* AssetLoaderInitializationImpl::Initialize()
{
#if HELIUM_TOOLS
    if( !LooseAssetLoader::InitializeStaticInstance() )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "AssetLoaderFactoryWin::Create(): Failed to initialize LooseAssetLoader instance.\n" ) );

        return NULL;
    }

    AssetPreprocessor* pAssetPreprocessor = AssetPreprocessor::CreateStaticInstance();
    HELIUM_ASSERT( pAssetPreprocessor );
    PlatformPreprocessor* pPlatformPreprocessor = new PcPreprocessor;
    HELIUM_ASSERT( pPlatformPreprocessor );
    pAssetPreprocessor->SetPlatformPreprocessor( Cache::PLATFORM_PC, pPlatformPreprocessor );
#else
    if( !CacheAssetLoader::InitializeStaticInstance() )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "AssetLoaderFactoryWin::Create() Failed to initialize PcCacheAssetLoader instance.\n" ) );

        return NULL;
    }
#endif

    AssetLoader* pAssetLoader = AssetLoader::GetInstance();
    HELIUM_ASSERT( pAssetLoader );

    return pAssetLoader;
}

/// @copydoc AssetLoaderInitialization::Shutdown()
void AssetLoaderInitializationImpl::Shutdown()
{
#if HELIUM_TOOLS
    AssetPreprocessor::DestroyStaticInstance();
#endif

    AssetLoaderInitialization::Shutdown();
}
