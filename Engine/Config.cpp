#include "EnginePch.h"
#include "Engine/Config.h"

#include "Engine/Asset.h"
#include "Engine/AssetLoader.h"
#include "Engine/PackageLoader.h"
#include "Engine/FileLocations.h"
#include "Persist/ArchiveJson.h"

using namespace Helium;

Config* Config::sm_pInstance = NULL;

/// Constructor.
Config::Config()
	: m_bLoadingConfigPackage( false )
{
	HELIUM_VERIFY( m_configContainerPackagePath.Set(
		Name( HELIUM_CONFIG_CONTAINER_PACKAGE ),
		true,
		AssetPath( NULL_NAME ) ) );
	HELIUM_VERIFY( m_defaultConfigPackagePath.Set(
		Name( HELIUM_CONFIG_DEFAULT_PACKAGE_BASE HELIUM_CONFIG_PLATFORM_SUFFIX ),
		true,
		m_configContainerPackagePath ) );
	
	if ( !FileLocations::GetUserDirectory( m_userDataDirectory ) )
	{
		HELIUM_TRACE( TraceLevels::Warning, TXT( "ConfigPc: No user data directory could be determined.\n" ) );
	}

}

/// Destructor.
Config::~Config()
{
}

Helium::FilePath Helium::Config::GetUserConfigObjectFilePath( Name name )
{
	String cacheFilePath( m_userDataDirectory.Data() );
	cacheFilePath += m_defaultConfigPackagePath.ToFilePathString();
	cacheFilePath += Helium::s_InternalPathSeparator;
	cacheFilePath += *name;
	cacheFilePath += ".";
	cacheFilePath += Persist::ArchiveExtensions[ Persist::ArchiveTypes::Json ];

	return FilePath( *cacheFilePath );
}

/// Begin async loading of configuration settings.
///
/// @see TryFinishLoad()
void Config::BeginLoad()
{
	// Check if loading is already in progress.
	if( !m_assetLoadIds.IsEmpty() )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			TXT( "Config::BeginLoad(): Called while configuration loading is already in progress.\n" ) );

		return;
	}

	// Clear out all existing object references.
	m_spDefaultConfigPackage.Release();

	m_defaultConfigAssets.Clear();
	m_configObjects.Clear();

	// Initiate pre-loading of the default and user configuration packages.
	AssetLoader* pLoader = AssetLoader::GetStaticInstance();
	HELIUM_ASSERT( pLoader );

	m_assetLoadIds.Clear();
	size_t loadId;

	loadId = pLoader->BeginLoadObject( m_defaultConfigPackagePath );
	HELIUM_ASSERT( IsValid( loadId ) );
	m_assetLoadIds.Add( loadId );

	m_bLoadingConfigPackage = true;
}

/// Poll for whether asynchronous config loading has completed.
///
/// @return  True if loading has completed, false if not.
///
/// @see BeginLoad()
bool Config::TryFinishLoad()
{
	AssetLoader* pLoader = AssetLoader::GetStaticInstance();
	HELIUM_ASSERT( pLoader );

	if( m_bLoadingConfigPackage )
	{
		HELIUM_ASSERT( m_assetLoadIds.GetSize() == 1 );

		size_t defaultConfigLoadId = m_assetLoadIds[ 0 ];
		if( IsValid( defaultConfigLoadId ) )
		{
			HELIUM_ASSERT( !m_spDefaultConfigPackage );

			AssetPtr spPackage;
			if( !pLoader->TryFinishLoad( defaultConfigLoadId, spPackage ) )
			{
				return false;
			}

			m_spDefaultConfigPackage = Reflect::AssertCast< Package >( spPackage.Get() );
			HELIUM_ASSERT( m_spDefaultConfigPackage );

			SetInvalid( m_assetLoadIds[ 0 ] );
		}

		m_assetLoadIds.Resize( 0 );
		m_bLoadingConfigPackage = false;

		// Begin loading all objects in each configuration package.
		AssetPath packagePath;
		PackageLoader* pPackageLoader;
		size_t objectCount;

		packagePath = m_spDefaultConfigPackage->GetPath();
		pPackageLoader = m_spDefaultConfigPackage->GetLoader();
		HELIUM_ASSERT( pPackageLoader );
		objectCount = pPackageLoader->GetObjectCount();
		for( size_t objectIndex = 0; objectIndex < objectCount; ++objectIndex )
		{
			AssetPath objectPath = pPackageLoader->GetAssetPath( objectIndex );
			if( !objectPath.IsPackage() && objectPath.GetParent() == packagePath )
			{
				size_t loadId = pLoader->BeginLoadObject( objectPath );
				HELIUM_ASSERT( IsValid( loadId ) );

				m_assetLoadIds.Add( loadId );
			}
		}
	}

	AssetPath defaultConfigPackagePath = m_spDefaultConfigPackage->GetPath();

	AssetPtr spAsset;
	size_t objectIndex = m_assetLoadIds.GetSize();
	while( objectIndex != 0 )
	{
		--objectIndex;

		size_t loadId = m_assetLoadIds[ objectIndex ];
		HELIUM_ASSERT( IsValid( loadId ) );
		if( !pLoader->TryFinishLoad( loadId, spAsset ) )
		{
			return false;
		}

		if( spAsset )
		{
			ConfigAssetPtr spConfigAsset = Reflect::SafeCast<ConfigAsset>(spAsset.Get());

			if ( spConfigAsset )
			{
				HELIUM_TRACE( TraceLevels::Info, TXT( "Loaded configuration object \"%s\".\n" ), *spAsset->GetPath().ToString() );
				m_defaultConfigAssets.Add( spConfigAsset );
			}
			else
			{
				HELIUM_TRACE( TraceLevels::Info, TXT( "Configuration assets are expected to be of type ConfigAsset \"%s\".\n" ), *spAsset->GetPath().ToString() );
			}
		}

		m_assetLoadIds.Remove( objectIndex );
	}

	m_configObjects.Resize( m_defaultConfigAssets.GetSize() );

	//TODO: Do this asynchronously
	for ( size_t index = 0; index < m_defaultConfigAssets.GetSize(); ++index )
	{
		const Name &name = m_defaultConfigAssets[ index ]->GetName();
		FilePath path = GetUserConfigObjectFilePath( name );

		if (path.Exists())
		{
			Reflect::ObjectPtr configObject = Persist::ArchiveReader::ReadFromFile(path);

			if (configObject)
			{
				m_configObjects[ index ] = configObject;
			}
			else
			{
				HELIUM_TRACE( TraceLevels::Info, TXT( "User config object failed to load for \"%s\". It will be replaced with default settings.\n" ), *name );
			}
		}
		else
		{
			HELIUM_TRACE( TraceLevels::Info, TXT( "User config object does not exist for \"%s\". This is normal for first runs.\n" ), *name );
		}
	}

	// List of async object load IDs should be empty, but call Clear() on it to regain allocated memory as well.
	HELIUM_ASSERT( m_assetLoadIds.IsEmpty() );
	m_assetLoadIds.Clear();

	// Now that all configuration objects have now been loaded, make sure that a user configuration object
	// corresponds to each default configuration object.
	size_t defaultConfigObjectCount = m_defaultConfigAssets.GetSize();
	for( size_t defaultObjectIndex = 0; defaultObjectIndex < defaultConfigObjectCount; ++defaultObjectIndex )
	{
		ConfigAsset* pDefaultConfigAsset = m_defaultConfigAssets[ defaultObjectIndex ];
		HELIUM_ASSERT( pDefaultConfigAsset );

		Name objectName = pDefaultConfigAsset->GetName();

		if( !m_configObjects[ defaultObjectIndex ] )
		{
			const Reflect::Object *pDefaultConfigObject = pDefaultConfigAsset->GetConfigObject();
			HELIUM_ASSERT( pDefaultConfigObject );

			const Reflect::ObjectPtr clone = const_cast< Reflect::Object *>( pDefaultConfigObject )->Clone();

			HELIUM_ASSERT( clone );
			if( clone )
			{
				HELIUM_TRACE(
					TraceLevels::Info,
					TXT( "Config: Created user configuration object \"%s\".\n" ),
					*objectName );

				m_configObjects[ defaultObjectIndex ] = clone;
			}
			else
			{
				HELIUM_TRACE(
					TraceLevels::Error,
					TXT( "Config: Failed to create user configuration object \"%s\".\n" ),
					*objectName );
			}
		}
	}

	return true;
}

/// Get the singleton Config instance, creating it if necessary.
///
/// @return  Reference to the Config instance.
///
/// @see DestroyStaticInstance()
Config& Config::GetStaticInstance()
{
	if( !sm_pInstance )
	{
		sm_pInstance = new Config;
		HELIUM_ASSERT( sm_pInstance );
	}

	return *sm_pInstance;
}

/// Destroy the singleton Config instance.
///
/// @see GetStaticInstance()
void Config::DestroyStaticInstance()
{
	delete sm_pInstance;
	sm_pInstance = NULL;
}
