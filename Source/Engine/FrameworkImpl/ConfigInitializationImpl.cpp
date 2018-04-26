#include "Precompile.h"
#include "FrameworkImpl/ConfigInitializationImpl.h"

#include "Engine/Config.h"
#include "Engine/AssetLoader.h"
#include "PcSupport/ConfigPc.h"

using namespace Helium;

/// @copydoc ConfigInitialization::Initialize()
void ConfigInitializationImpl::Startup()
{
	Config::Startup();

	Config* pConfig = Config::GetInstance();
	HELIUM_ASSERT( pConfig );

	AssetLoader* pAssetLoader = AssetLoader::GetInstance();
	HELIUM_ASSERT( pAssetLoader );

	HELIUM_TRACE( TraceLevels::Info, "Loading configuration settings.\n" );

	pConfig->BeginLoad();
	while( !pConfig->TryFinishLoad() )
	{
		pAssetLoader->Tick();
	}

	HELIUM_TRACE( TraceLevels::Debug, "Configuration settings loaded.\n" );

#if HELIUM_TOOLS
	HELIUM_TRACE( TraceLevels::Info, "Saving user configuration.\n" );
	ConfigPc::SaveUserConfig();
	HELIUM_TRACE( TraceLevels::Info, "User configuration saved.\n" );
#endif
}
