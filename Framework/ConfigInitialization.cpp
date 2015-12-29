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
    Config* pConfig = Config::GetInstance();
	HELIUM_ASSERT( pConfig );

    AssetLoader* pAssetLoader = AssetLoader::GetInstance();
    HELIUM_ASSERT( pAssetLoader );

    HELIUM_TRACE( TraceLevels::Info, TXT( "Loading configuration settings.\n" ) );

    pConfig->BeginLoad();
    while( !pConfig->TryFinishLoad() )
    {
        pAssetLoader->Tick();
    }

    HELIUM_TRACE( TraceLevels::Debug, TXT( "Configuration settings loaded.\n" ) );

    return true;
}
