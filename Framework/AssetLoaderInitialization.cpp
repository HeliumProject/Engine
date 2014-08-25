#include "FrameworkPch.h"
#include "Framework/AssetLoaderInitialization.h"

#include "Engine/CacheManager.h"
#include "Engine/AssetLoader.h"

using namespace Helium;

/// Destructor.
AssetLoaderInitialization::~AssetLoaderInitialization()
{
}

/// @fn AssetLoader* AssetLoaderInitialization::Initialize()
/// Create and initialize a new AssetLoader instance.
///
/// @return  Pointer to the AssetLoader instance if initialized successfully, null if creation and initialization
///          failed.
///
/// @see Shutdown()

/// Shut down and destroy the AssetLoader and any related types.
///
/// @see Initialize()
void AssetLoaderInitialization::Shutdown()
{
    AssetLoader::DestroyStaticInstance();
    CacheManager::Cleanup();
}
