#include "PcSupportPch.h"
#include "LooseAssetLoader.h"

#include "Platform/File.h"
#include "Foundation/FilePath.h"
#include "Engine/FileLocations.h"
#include "Engine/Config.h"
#include "Engine/Resource.h"
#include "PcSupport/AssetPreprocessor.h"
#include "PcSupport/LoosePackageLoader.h"

using namespace Helium;

/// Constructor.
LooseAssetLoader::LooseAssetLoader()
{
}

/// Destructor.
LooseAssetLoader::~LooseAssetLoader()
{
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
void LooseAssetLoader::OnLoadComplete( AssetPath /*path*/, Asset* pObject, PackageLoader* /*pPackageLoader*/ )
{
	if( pObject )
	{
		CacheObject( pObject, true );
	}
}

/// @copydoc AssetLoader::OnPrecacheReady()
 void LooseAssetLoader::OnPrecacheReady( Asset* pObject, PackageLoader* pPackageLoader )
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
 
	 // Grab the package timestamp.
	 HELIUM_ASSERT( pPackageLoader->CanResolveLooseAssetFilePaths() );
	 int64_t objectTimestamp = pPackageLoader->GetLooseAssetFileSystemTimestamp( pResource->GetPath() );
 
	 // Attempt to load the resource data.
	 pAssetPreprocessor->LoadResourceData( pResource, objectTimestamp );
 }

/// @copydoc AssetLoader::CacheObject()
bool LooseAssetLoader::CacheObject( Asset* pAsset, bool bEvictPlatformPreprocessedResourceData )
{
	HELIUM_ASSERT( pAsset );
	
	HELIUM_TRACE(
		TraceLevels::Warning,
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

	// Configuration objects should not be cached.
	AssetPath objectPath = pAsset->GetPath();

	Config& rConfig = Config::GetStaticInstance();
	AssetPath configPackagePath = rConfig.GetConfigContainerPackagePath();
	HELIUM_ASSERT( !configPackagePath.IsEmpty() );

	for( AssetPath testPath = objectPath; !testPath.IsEmpty(); testPath = testPath.GetParent() )
	{
		if( testPath == configPackagePath )
		{
			return false;
		}
	}

	// Get the timestamp for the object based on the timestamp of its source package file and, if it's a resource,
	// the timestamp of the source resource file.
	Asset* pPackageObject;
	for( pPackageObject = pAsset;
		pPackageObject && !pPackageObject->IsPackage();
		pPackageObject = pPackageObject->GetOwner() )
	{
	}

	HELIUM_ASSERT( pPackageObject );

	PackageLoader* pPackageLoader = Reflect::AssertCast< Package >( pPackageObject )->GetLoader();
	HELIUM_ASSERT( pPackageLoader );
	HELIUM_ASSERT( pPackageLoader->CanResolveLooseAssetFilePaths() );

	int64_t objectTimestamp = pPackageLoader->GetLooseAssetFileSystemTimestamp( pAsset->GetPath() );

	if( !pAsset->IsDefaultTemplate() )
	{
		Resource* pResource = Reflect::SafeCast< Resource >( pAsset );
		if( pResource )
		{
			AssetPath baseResourcePath = pResource->GetPath();
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
