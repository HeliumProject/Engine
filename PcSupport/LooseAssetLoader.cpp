#include "Precompile.h"
#include "LooseAssetLoader.h"

#include "Platform/File.h"
#include "Foundation/FilePath.h"
#include "Engine/FileLocations.h"
#include "Engine/Config.h"
#include "Engine/Resource.h"
#include "PcSupport/AssetPreprocessor.h"
#include "PcSupport/LoosePackageLoader.h"
#include "Foundation/DirectoryIterator.h"
#include "LooseAssetFileWatcher.h"

using namespace Helium;

static uint32_t g_InitCount = 0;

#define USE_LOOSE_ASSET_FILE_WATCHER (0)

#if USE_LOOSE_ASSET_FILE_WATCHER
LooseAssetFileWatcher g_FileWatcher;
#endif

/// Constructor.
LooseAssetLoader::LooseAssetLoader()
{
#if USE_LOOSE_ASSET_FILE_WATCHER
	g_FileWatcher.StartThread();
#endif
}

/// Destructor.
LooseAssetLoader::~LooseAssetLoader()
{
#if USE_LOOSE_ASSET_FILE_WATCHER
	g_FileWatcher.StopThread();
#endif
}

/// Initialize the static object loader instance as an LooseAssetLoader.
///
/// @return  True if the loader was initialized successfully, false if not or another object loader instance already
///          exists.
void LooseAssetLoader::Startup()
{
	if ( ++g_InitCount == 1 )
	{
		AssetLoader::Startup();

		HELIUM_ASSERT( !sm_pInstance );
		sm_pInstance = new LooseAssetLoader;
		HELIUM_ASSERT( sm_pInstance );
	}
}

/// Destroy the global object loader instance if one exists.
///
/// @see GetInstance()
void LooseAssetLoader::Shutdown()
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
PackageLoader* LooseAssetLoader::GetPackageLoader( AssetPath path )
{
	LoosePackageLoader* pLoader = m_packageLoaderMap.GetPackageLoader( path );

	return pLoader;
}

/// @copydoc AssetLoader::TickPackageLoaders()
void LooseAssetLoader::TickPackageLoaders()
{
	m_packageLoaderMap.TickPackageLoaders();
}

/// @copydoc AssetLoader::OnLoadComplete()
void LooseAssetLoader::OnLoadComplete( const AssetPath &path, Asset* pAsset, PackageLoader* /*pPackageLoader*/ )
{
	if ( pAsset )
	{
		CacheObject( pAsset, true );
	}
}

/// @copydoc AssetLoader::OnPrecacheReady()
void LooseAssetLoader::OnPrecacheReady( Asset* pAsset, PackageLoader* pPackageLoader )
{
	HELIUM_ASSERT( pAsset );
	HELIUM_ASSERT( pPackageLoader );

	// The default template object for a given type never has its resource data preprocessed, so there's no need to
	// precache default template objects.
	if ( pAsset->IsDefaultTemplate() )
	{
		return;
	}

	// Retrieve the object preprocessor if it exists.
	AssetPreprocessor* pAssetPreprocessor = AssetPreprocessor::GetInstance();
	if ( !pAssetPreprocessor )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			"LooseAssetLoader::OnPrecacheReady(): Missing AssetPreprocessor to use for resource preprocessing.\n" );

		return;
	}

	// We only need to do precache handling for resources, so skip non-resource types.
	Resource* pResource = Reflect::SafeCast< Resource >( pAsset );
	if ( !pResource )
	{
		return;
	}

	// Attempt to load the resource data.
	pAssetPreprocessor->LoadResourceData( pAsset->GetPath(), pResource );
}

/// @copydoc AssetLoader::CacheObject()
bool LooseAssetLoader::CacheObject( Asset* pAsset, bool bEvictPlatformPreprocessedResourceData )
{
	HELIUM_ASSERT( pAsset );
	const AssetPath &path = pAsset->GetPath();

	HELIUM_TRACE(
		TraceLevels::Info,
		"LooseAssetLoader::CacheObject(): Caching asset %s.\n",
		*path.ToString() );

	// Don't cache broken objects or packages.
	if ( pAsset->GetAnyFlagSet( Asset::FLAG_BROKEN ) || pAsset->IsPackage() )
	{
		return false;
	}

	// Make sure we have an object preprocessor instance with which to cache the object.
	AssetPreprocessor* pAssetPreprocessor = AssetPreprocessor::GetInstance();
	if ( !pAssetPreprocessor )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			"LooseAssetLoader::CacheObject(): Missing AssetPreprocessor to use for caching.\n" );

		return false;
	}

	Config* pConfig = Config::GetInstance();
	HELIUM_ASSERT( pConfig );

	int64_t objectTimestamp = AssetLoader::GetAssetFileTimestamp( path );

	if ( !pAsset->IsDefaultTemplate() )
	{
		Resource* pResource = Reflect::SafeCast< Resource >( pAsset );
		if ( pResource )
		{
			AssetPath baseResourcePath = path;
			HELIUM_ASSERT( !baseResourcePath.IsPackage() );
			for ( ;; )
			{
				AssetPath parentPath = baseResourcePath.GetParent();
				if ( parentPath.IsEmpty() || parentPath.IsPackage() )
				{
					break;
				}

				baseResourcePath = parentPath;
			}

			FilePath sourceFilePath;
			if ( !FileLocations::GetDataDirectory( sourceFilePath ) )
			{
				HELIUM_TRACE(
					TraceLevels::Warning,
					"LooseAssetLoader::CacheObject(): Could not obtain data directory.\n" );

				return false;
			}

			sourceFilePath += baseResourcePath.ToFilePathString().GetData();

			Status stat;
			stat.Read( sourceFilePath.Get().c_str() );

			int64_t sourceFileTimestamp = stat.m_ModifiedTime;
			if ( sourceFileTimestamp > objectTimestamp )
			{
				objectTimestamp = sourceFileTimestamp;
			}
		}
	}

	// Cache the object.
	bool bSuccess = pAssetPreprocessor->CacheObject(
		path,
		pAsset,
		objectTimestamp,
		bEvictPlatformPreprocessedResourceData );
	if ( !bSuccess )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"LooseAssetLoader: Failed to cache object \"%s\".\n",
			*path.ToString() );
	}

	return bSuccess;
}

void Helium::LooseAssetLoader::EnumerateRootPackages( DynamicArray< AssetPath > &packagePaths )
{
	FilePath dataDirectory;
	FileLocations::GetDataDirectory( dataDirectory );

	DirectoryIterator packageDirectory( dataDirectory );
	for ( ; !packageDirectory.IsDone(); packageDirectory.Next() )
	{
		if ( packageDirectory.GetItem().m_Path.IsDirectory() )
		{
			AssetPath path;

			std::vector< std::string > directories;
			packageDirectory.GetItem().m_Path.Directories( directories );
			HELIUM_ASSERT( !directories.empty() );
			std::string directory = directories.back();
			if ( directory.size() <= 0 )
			{
				continue;
			}

			path.Set( Name( directory.c_str() ), true, AssetPath( NULL_NAME ) );
			packagePaths.Add( path );
		}

	}
}

void LooseAssetLoader::OnPackagePreloaded( LoosePackageLoader *pPackageLoader )
{
#if USE_LOOSE_ASSET_FILE_WATCHER
	g_FileWatcher.AddPackage( pPackageLoader );
#endif
}
