#include "PcSupportPch.h"
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

LooseAssetFileWatcher g_FileWatcher;

/// Constructor.
LooseAssetLoader::LooseAssetLoader()
{
	g_FileWatcher.StartThread();
}

/// Destructor.
LooseAssetLoader::~LooseAssetLoader()
{
	g_FileWatcher.StopThread();
}

/// Initialize the static object loader instance as an LooseAssetLoader.
///
/// @return  True if the loader was initialized successfully, false if not or another object loader instance already
///          exists.
bool LooseAssetLoader::InitializeStaticInstance()
{
	if( sm_pInstance )
	{
		return false;
	}

	sm_pInstance = new LooseAssetLoader;
	HELIUM_ASSERT( sm_pInstance );

	return true;
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
void LooseAssetLoader::OnLoadComplete( const AssetPath &path, Asset* pObject, PackageLoader* /*pPackageLoader*/ )
{
	if( pObject )
	{
		CacheObject( path, pObject, true );
	}
}

/// @copydoc AssetLoader::OnPrecacheReady()
 void LooseAssetLoader::OnPrecacheReady( const AssetPath &path, Asset* pObject, PackageLoader* pPackageLoader )
 {
	 HELIUM_ASSERT( pObject );
	 HELIUM_ASSERT( pPackageLoader );
 
	 // The default template object for a given type never has its resource data preprocessed, so there's no need to
	 // precache default template objects.
	 if( pObject->IsDefaultTemplate() )
	 {
		 return;
	 }
 
	 // Retrieve the object preprocessor if it exists.
	 AssetPreprocessor* pAssetPreprocessor = AssetPreprocessor::GetStaticInstance();
	 if( !pAssetPreprocessor )
	 {
		 HELIUM_TRACE(
			 TraceLevels::Warning,
			 ( TXT( "LooseAssetLoader::OnPrecacheReady(): Missing AssetPreprocessor to use for resource " )
			 TXT( "preprocessing.\n" ) ) );
 
		 return;
	 }
 
	 // We only need to do precache handling for resources, so skip non-resource types.
	 Resource* pResource = Reflect::SafeCast< Resource >( pObject );
	 if( !pResource )
	 {
		 return;
	 }
 
	 // Attempt to load the resource data.
	 pAssetPreprocessor->LoadResourceData( path, pResource );
 }

/// @copydoc AssetLoader::CacheObject()
bool LooseAssetLoader::CacheObject( const AssetPath &path, Asset* pAsset, bool bEvictPlatformPreprocessedResourceData )
{
	HELIUM_ASSERT( pAsset );
	
	HELIUM_TRACE(
		TraceLevels::Info,
		TXT( "LooseAssetLoader::CacheObject(): Caching asset %s.\n" ), *pAsset->GetPath().ToString() );

	// Don't cache broken objects or packages.
	if( pAsset->GetAnyFlagSet( Asset::FLAG_BROKEN ) || pAsset->IsPackage() )
	{
		return false;
	}

	// Make sure we have an object preprocessor instance with which to cache the object.
	AssetPreprocessor* pAssetPreprocessor = AssetPreprocessor::GetStaticInstance();
	if( !pAssetPreprocessor )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			TXT( "LooseAssetLoader::CacheObject(): Missing AssetPreprocessor to use for caching.\n" ) );

		return false;
	}

	// User configuration objects should not be cached.
	AssetPath objectPath = pAsset->GetPath();

	Config& rConfig = Config::GetStaticInstance();

	// Only cache the files we care about
	if ( rConfig.IsAssetPathInUserConfigPackage(objectPath) )
	{
		return false;
	}

	int64_t objectTimestamp = AssetLoader::GetAssetFileTimestamp( path );

	if( !pAsset->IsDefaultTemplate() )
	{
		Resource* pResource = Reflect::SafeCast< Resource >( pAsset );
		if( pResource )
		{
			AssetPath baseResourcePath = path;
			HELIUM_ASSERT( !baseResourcePath.IsPackage() );
			for( ; ; )
			{
				AssetPath parentPath = baseResourcePath.GetParent();
				if( parentPath.IsEmpty() || parentPath.IsPackage() )
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
					TXT( "LooseAssetLoader::CacheObject(): Could not obtain data directory.\n" ) );

				return false;
			}

			sourceFilePath += baseResourcePath.ToFilePathString().GetData();

			Status stat;
			stat.Read( sourceFilePath.Get().c_str() );

			int64_t sourceFileTimestamp = stat.m_ModifiedTime;
			if( sourceFileTimestamp > objectTimestamp )
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
	if( !bSuccess )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "LooseAssetLoader: Failed to cache object \"%s\".\n" ),
			*objectPath.ToString() );
	}

	return bSuccess;
}

#if HELIUM_TOOLS

void Helium::LooseAssetLoader::EnumerateRootPackages( DynamicArray< AssetPath > &packagePaths )
{
	FilePath dataDirectory;
	FileLocations::GetDataDirectory( dataDirectory );

	DirectoryIterator packageDirectory( dataDirectory );
	for( ; !packageDirectory.IsDone(); packageDirectory.Next() )
	{
		if (packageDirectory.GetItem().m_Path.IsDirectory())
		{
			AssetPath path;

			//std::string filename = packageDirectory.GetItem().m_Path.Parent();
			std::vector< std::string > filename = packageDirectory.GetItem().m_Path.DirectoryAsVector();
			HELIUM_ASSERT(!filename.empty());
			std::string directory = filename.back();

			if (directory.size() <= 0)
			{
				continue;
			}
			path.Set( Name( directory.c_str() ), true, AssetPath(NULL_NAME) );

			packagePaths.Add( path );
		}

	}
}
#endif

void LooseAssetLoader::OnPackagePreloaded( LoosePackageLoader *pPackageLoader )
{
#if HELIUM_TOOLS
	g_FileWatcher.AddPackage( pPackageLoader );
#endif
}
