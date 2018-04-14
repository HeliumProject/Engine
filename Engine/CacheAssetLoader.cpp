#include "EnginePch.h"
#include "Engine/CacheAssetLoader.h"
#include "Engine/CachePackageLoader.h"
#include "Engine/Config.h"

using namespace Helium;

static uint32_t g_InitCount = 0;

/// Constructor.
CacheAssetLoader::CacheAssetLoader()
{
	m_pAssetPackageLoader = new CachePackageLoader;
	HELIUM_ASSERT( m_pAssetPackageLoader );
	HELIUM_VERIFY( m_pAssetPackageLoader->Initialize( Name( "Asset" ) ) );

	HELIUM_VERIFY( m_pAssetPackageLoader->BeginPreload() );

	m_pConfigPackageLoader = new CachePackageLoader;
	HELIUM_ASSERT( m_pConfigPackageLoader );
	HELIUM_VERIFY( m_pConfigPackageLoader->Initialize( Name( "Config" ) ) );

	HELIUM_VERIFY( m_pConfigPackageLoader->BeginPreload() );
}

/// Destructor.
CacheAssetLoader::~CacheAssetLoader()
{
	delete m_pAssetPackageLoader;
	m_pAssetPackageLoader = NULL;

	delete m_pConfigPackageLoader;
	m_pConfigPackageLoader = NULL;
}

/// Initialize the static object loader instance as a CacheAssetLoader.
///
/// @return  True if the loader was initialized successfully, false if not or another object loader instance already
///          exists.
void CacheAssetLoader::Startup()
{
	if ( ++g_InitCount == 1 )
	{
		AssetLoader::Startup();

		HELIUM_ASSERT( !sm_pInstance )
		sm_pInstance = new CacheAssetLoader;
		HELIUM_ASSERT( sm_pInstance );
	}
}

/// Destroy the global object loader instance if one exists.
///
/// @see GetInstance()
void CacheAssetLoader::Shutdown()
{
	if ( --g_InitCount == 0 )
	{
		HELIUM_ASSERT( sm_pInstance );
		delete sm_pInstance;
		sm_pInstance = NULL;

		AssetLoader::Shutdown();
	}
}

/// @copydoc AssetLoader::GetPackageLoader()
PackageLoader* CacheAssetLoader::GetPackageLoader( AssetPath path )
{
	if ( HELIUM_VERIFY( Config::GetInstance() ) && Config::GetInstance()->IsAssetPathInConfigContainerPackage( path ) )
	{
		return m_pConfigPackageLoader;
	}

	return m_pAssetPackageLoader;
}

/// @copydoc AssetLoader::TickPackageLoaders()
void CacheAssetLoader::TickPackageLoaders()
{
	m_pAssetPackageLoader->Tick();
	m_pConfigPackageLoader->Tick();
}
