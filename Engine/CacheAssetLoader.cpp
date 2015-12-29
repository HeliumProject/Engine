#include "EnginePch.h"
#include "Engine/CacheAssetLoader.h"
#include "Engine/CachePackageLoader.h"
#include "Engine/Config.h"

using namespace Helium;

/// Constructor.
CacheAssetLoader::CacheAssetLoader()
{
	m_pAssetPackageLoader = new CachePackageLoader;
	HELIUM_ASSERT( m_pAssetPackageLoader );
	HELIUM_VERIFY( m_pAssetPackageLoader->Initialize( Name( TXT("Asset") ) ) );

	HELIUM_VERIFY( m_pAssetPackageLoader->BeginPreload() );

	m_pConfigPackageLoader = new CachePackageLoader;
	HELIUM_ASSERT( m_pConfigPackageLoader );
	HELIUM_VERIFY( m_pConfigPackageLoader->Initialize( Name( TXT("Config") ) ) );

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
bool CacheAssetLoader::InitializeStaticInstance()
{
	if( sm_pInstance )
	{
		return false;
	}

	sm_pInstance = new CacheAssetLoader;
	HELIUM_ASSERT( sm_pInstance );

	return true;
}

/// @copydoc AssetLoader::GetPackageLoader()
PackageLoader* CacheAssetLoader::GetPackageLoader( AssetPath path )
{
	if ( Config::GetInstance().IsAssetPathInConfigContainerPackage( path ) )
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
