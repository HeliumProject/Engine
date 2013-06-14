#include "FrameworkPch.h"
#include "Framework/ConfigInitialization.h"

#include "Engine/Config.h"
#include "Engine/AssetLoader.h"

using namespace Helium;

/// Destructor.
ConfigInitialization::~ConfigInitialization()
{
}

/// Initialize application configuration settings.
///
/// @return  True if initialization was successful, false if not.
bool ConfigInitialization::Initialize()
{
    Config& rConfig = Config::GetStaticInstance();

    AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
    HELIUM_ASSERT( pAssetLoader );

    HELIUM_TRACE( TraceLevels::Info, TXT( "Loading configuration settings.\n" ) );

    rConfig.BeginLoad();
    while( !rConfig.TryFinishLoad() )
    {
        pAssetLoader->Tick();
    }

    HELIUM_TRACE( TraceLevels::Debug, TXT( "Configuration settings loaded.\n" ) );

    return true;
}
