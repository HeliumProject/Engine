#include "PcSupportPch.h"
#include "PcSupport/LoosePackageLoader.h"

#include "Engine/FileLocations.h"
#include "Foundation/FilePath.h"
#include "Foundation/DirectoryIterator.h"
#include "Foundation/FileStream.h"
#include "Foundation/MemoryStream.h"
#include "Engine/AsyncLoader.h"
#include "Engine/CacheManager.h"
#include "Engine/Config.h"
#include "Engine/AssetLoader.h"
#include "Engine/Resource.h"
#include "PcSupport/AssetPreprocessor.h"
#include "PcSupport/ResourceHandler.h"
#include "Reflect/TranslatorDeduction.h"
#include "Persist/ArchiveJson.h"

#include "LooseAssetLoader.h"

using namespace Helium;

/// Constructor.
LoosePackageLoader::LoosePackageLoader()
	: m_startPreloadCounter( 0 )
	, m_preloadedCounter( 0 )
	, m_loadRequestPool( LOAD_REQUEST_POOL_BLOCK_SIZE )
	, m_parentPackageLoadId( Invalid< size_t >() )
	//, m_pTocLoadBuffer( 0 )
	//, m_tocAsyncLoadId( Invalid<size_t>() )
	//, m_packageTocFileSize( 0 )
{
}

/// Destructor.
LoosePackageLoader::~LoosePackageLoader()
{
	Cleanup();
}

/// Initialize this package loader.
///
/// @param[in] packagePath  Asset path of the package to load.
///
/// @return  True if this loader was initialized successfully, false if not.
///
/// @see Shutdown()
bool LoosePackageLoader::Initialize( AssetPath packagePath )
{
	Cleanup();

	// Make sure the path represents a package.
	if ( packagePath.IsEmpty() )
	{
		HELIUM_TRACE( TraceLevels::Error, "LoosePackageLoader::Initialize(): Empty package path specified.\n" );

		return false;
	}

	HELIUM_TRACE(
		TraceLevels::Debug,
		"LoosePackageLoader::Initialize(): Initializing loader for package \"%s\".\n",
		*packagePath.ToString() );

	if ( !packagePath.IsPackage() )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"LoosePackageLoader::Initialize(): \"%s\" does not represent a package path.\n",
			*packagePath.ToString() );

		return false;
	}

	// Store the package path.
	m_packagePath = packagePath;

	// Attempt to locate the specified package if it already happens to exist.
	m_spPackage = Asset::Find< Package >( packagePath );
	Package* pPackage = m_spPackage;
	if ( pPackage )
	{
		if ( pPackage->GetLoader() )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				"LoosePackageLoader::Initialize(): Package \"%s\" already has a loader.\n",
				*packagePath.ToString() );

			m_spPackage.Release();

			return false;
		}

		pPackage->SetLoader( this );
	}
	else
	{
		// Make sure we don't have a name clash with a non-package object.
		AssetPtr spObject( Asset::FindObject( packagePath ) );
		if ( spObject )
		{
			HELIUM_ASSERT( !spObject->IsPackage() );

			HELIUM_TRACE(
				TraceLevels::Error,
				"PackageLoader::Initialize(): Package loader cannot be initialized for \"%s\", as an object with the same name exists that is not a package.\n",
				*packagePath.ToString() );

			return false;
		}
	}

	// Build the package file path.  If the package is a user configuration package, use the user data directory,
	// otherwise use the global data directory.
	Config* pConfig = Config::GetInstance();
	HELIUM_ASSERT( pConfig );

	FilePath dataDirectory;
	if ( !FileLocations::GetDataDirectory( dataDirectory ) )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"PackageLoader::Initialize(): Could not obtain user data directory." );

		return false;
	}

	// Set up to read the TOC (which may not exist)
	//SetInvalid( m_packageTocFileSize );

	// First do this check without a trailing "/" so that FilePath has to actually look at the file system
	FilePath package_dir = dataDirectory + packagePath.ToFilePathString().GetData();

	if ( !package_dir.Exists() )
	{
		// Some packages like types or uninitialized user config packages may not exist on file system
		m_packageDirPath = package_dir + "/";
		return true;
	}

	if ( !package_dir.IsDirectory() )
	{
		// Packages should not be files
		return false;
	}

	// But internally we will store this 
	m_packageDirPath = package_dir + "/";

	return true;
}

/// @copydoc PackageLoader::Cleanup()
void LoosePackageLoader::Cleanup()
{
	// Sync with any in-flight async load requests.
	if ( m_startPreloadCounter )
	{
		while ( !TryFinishPreload() )
		{
			Tick();
		}
	}

	HELIUM_ASSERT( IsInvalid( m_parentPackageLoadId ) );

	// Unset the reference back to this loader in the package.
	Package* pPackage = m_spPackage;
	if ( pPackage )
	{
		pPackage->SetLoader( NULL );
	}

	m_spPackage.Release();
	m_packagePath.Clear();

	AtomicExchangeRelease( m_startPreloadCounter, 0 );
	AtomicExchangeRelease( m_preloadedCounter, 0 );

	m_objects.Clear();

	size_t loadRequestCount = m_loadRequests.GetSize();
	for ( size_t requestIndex = 0; requestIndex < loadRequestCount; ++requestIndex )
	{
		if ( m_loadRequests.IsElementValid( requestIndex ) )
		{
			LoadRequest* pRequest = m_loadRequests[requestIndex];
			HELIUM_ASSERT( pRequest );
			m_loadRequestPool.Release( pRequest );
		}
	}

	m_loadRequests.Clear();

	m_packageDirPath.Clear();
}

/// Begin asynchronous pre-loading of package information.
///
/// @see TryFinishPreload()
bool LoosePackageLoader::BeginPreload()
{
	HELIUM_ASSERT( !m_startPreloadCounter );
	HELIUM_ASSERT( !m_preloadedCounter );
	HELIUM_ASSERT( IsInvalid( m_parentPackageLoadId ) );

	// Load the parent package if we need to create the current package.
	if ( !m_spPackage )
	{
		AssetPath parentPackagePath = m_packagePath.GetParent();
		if ( !parentPackagePath.IsEmpty() )
		{
			AssetLoader* pAssetLoader = AssetLoader::GetInstance();
			HELIUM_ASSERT( pAssetLoader );

			m_parentPackageLoadId = pAssetLoader->BeginLoadObject( parentPackagePath );
			HELIUM_ASSERT( IsValid( m_parentPackageLoadId ) );
		}
	}

	AsyncLoader* pAsyncLoader = AsyncLoader::GetInstance();
	HELIUM_ASSERT( pAsyncLoader );

	if ( !m_packageDirPath.Exists() )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			"LoosePackageLoader::BeginPreload - Package physical path '%s' does not exist\n",
			m_packageDirPath.Data() );
	}
	else if ( !m_packageDirPath.IsDirectory() )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			"LoosePackageLoader::BeginPreload - Package physical path '%s' is not a directory\n",
			m_packageDirPath.Data() );
	}
	else
	{
		DirectoryIterator packageDirectory( m_packageDirPath );

		HELIUM_TRACE( TraceLevels::Info, " LoosePackageLoader::BeginPreload - Issuing read requests for all files in %s\n", m_packageDirPath.Data() );

		for ( ; !packageDirectory.IsDone(); packageDirectory.Next() )
		{
			const DirectoryIteratorItem& item = packageDirectory.GetItem();

#if HELIUM_TOOLS
			if ( item.m_Path.IsDirectory() )
			{
				AssetPath packagePath;
				std::vector< std::string > directories;
				item.m_Path.Directories( directories );
				std::string name = directories.back();
				packagePath.Set( Name( name.c_str() ), true, m_packagePath );
				m_childPackagePaths.Add( packagePath );
				HELIUM_TRACE( TraceLevels::Info, "- Skipping directory [%s]\n", item.m_Path.Data(), item.m_Path.Extension().c_str() );
			}
			else
#endif
				if ( item.m_Path.Extension() == Persist::ArchiveExtensions[Persist::ArchiveTypes::Json] )
				{
					HELIUM_TRACE( TraceLevels::Info, "- Reading file [%s]\n", item.m_Path.Data() );

					FileReadRequest *request = m_fileReadRequests.New();
					request->expectedSize = item.m_Size;

					HELIUM_ASSERT( item.m_Size < UINT32_MAX );

					// Create a buffer for the file to be read into temporarily
					request->pLoadBuffer = DefaultAllocator().Allocate( static_cast<size_t> ( item.m_Size ) + 1 );
					static_cast<char*>( request->pLoadBuffer )[static_cast<size_t> ( item.m_Size )] = '\0'; // for efficiency parsing text files
					HELIUM_ASSERT( request->pLoadBuffer );

					// Queue up the read
					request->asyncLoadId = pAsyncLoader->QueueRequest( request->pLoadBuffer, String( item.m_Path.Data() ), 0, static_cast<size_t>( item.m_Size ) );
					HELIUM_ASSERT( IsValid( request->asyncLoadId ) );

					request->filePath = item.m_Path;
					request->fileTimestamp = item.m_ModTime;
				}
				else
				{
					HELIUM_TRACE( TraceLevels::Info, "- Skipping file [%s] (Extension is %s)\n", item.m_Path.Data(), item.m_Path.Extension().c_str() );
				}
		}
	}

	AtomicExchangeRelease( m_startPreloadCounter, 1 );

	return true;
}

/// @copydoc PackageLoader::TryFinishPreload()
bool LoosePackageLoader::TryFinishPreload()
{
	return ( m_preloadedCounter != 0 );
}

/// @copydoc PackageLoader::BeginLoadObject()
size_t LoosePackageLoader::BeginLoadObject( AssetPath path, Reflect::ObjectResolver *pResolver, bool forceReload )
{
	HELIUM_TRACE( TraceLevels::Info, " LoosePackageLoader::BeginLoadObject - Loading path %s\n", *path.ToString() );

	HELIUM_TRACE(
		TraceLevels::Debug,
		"LoosePackageLoader::BeginLoadObject: Beginning load for path \"%s\".\n",
		*path.ToString() );


	HELIUM_TRACE(
		TraceLevels::Debug,
		"LoosePackageLoader::BeginLoadObject: Beginning load for path \"%s\". pResolver = %x\n",
		*path.ToString(),
		pResolver );

	// Make sure preloading has completed.
	HELIUM_ASSERT( m_preloadedCounter != 0 );
	if ( !m_preloadedCounter )
	{
		return Invalid< size_t >();
	}

	// If this package is requested, simply provide the (already loaded) package instance.
	if ( path == m_packagePath )
	{
		LoadRequest* pRequest = m_loadRequestPool.Allocate();
		HELIUM_ASSERT( pRequest );

		HELIUM_ASSERT( m_spPackage );
		pRequest->spObject = m_spPackage.Ptr();

		SetInvalid( pRequest->index );
		HELIUM_ASSERT( !pRequest->spType );
		HELIUM_ASSERT( !pRequest->spTemplate );
		HELIUM_ASSERT( !pRequest->spOwner );
		SetInvalid( pRequest->templateLoadId );
		SetInvalid( pRequest->ownerLoadId );
		SetInvalid( pRequest->persistentResourceDataLoadId );
		pRequest->pCachedObjectDataBuffer = NULL;
		pRequest->cachedObjectDataBufferSize = 0;
		SetInvalid( pRequest->asyncFileLoadId );
		pRequest->pAsyncFileLoadBuffer = NULL;
		pRequest->asyncFileLoadBufferSize = 0;
		pRequest->pResolver = NULL;
		pRequest->forceReload = forceReload;

		pRequest->flags = LOAD_FLAG_PRELOADED;

		size_t requestId = m_loadRequests.Add( pRequest );

		return requestId;
	}

	size_t objectIndex = FindObjectByPath( path );
	size_t objectCount = GetObjectCount();

	if ( objectIndex >= objectCount )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"LoosePackageLoader::BeginLoadObject(): Failed to locate \"%s\" for loading. Verify the file exists.\n",
			*path.ToString() );

		return Invalid< size_t >();
	}

	SerializedObjectData& rObjectData = m_objects[objectIndex];

	// Verify that the metadata was read successfully
	if ( !rObjectData.bMetadataGood )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"LoosePackageLoader::BeginLoadObject(): Failed to read metadata for object \"%s\" during PackagePreload. Search log for parsing errors.\n",
			*path.ToString() );

		return Invalid< size_t >();
	}

	// Locate the type object.
	HELIUM_ASSERT( !rObjectData.typeName.IsEmpty() );
	AssetType* pType = AssetType::Find( rObjectData.typeName );
	if ( !pType )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"LoosePackageLoader::BeginLoadObject(): Failed to locate type \"%s\" for loading object \"%s\".\n",
			*rObjectData.typeName,
			*path.ToString() );

		HELIUM_TRACE(
			TraceLevels::Info,
			"Current registered types:\n" );

		for ( AssetType::ConstIterator iter = AssetType::GetTypeBegin();
			iter != AssetType::GetTypeEnd(); ++iter )
		{
			HELIUM_TRACE(
				TraceLevels::Info,
				" - %s\n",
				*iter->GetName() );
		}

		return Invalid< size_t >();
	}

#ifndef NDEBUG
	size_t loadRequestSize = m_loadRequests.GetSize();
	for ( size_t loadRequestIndex = 0; loadRequestIndex < loadRequestSize; ++loadRequestIndex )
	{
		if ( !m_loadRequests.IsElementValid( loadRequestIndex ) )
		{
			continue;
		}

		LoadRequest* pRequest = m_loadRequests[loadRequestIndex];
		HELIUM_ASSERT( pRequest );
		HELIUM_ASSERT( pRequest->index != objectIndex );
		if ( pRequest->index == objectIndex )
		{
			return Invalid< size_t >();
		}
	}
#endif

	LoadRequest* pRequest = m_loadRequestPool.Allocate();
	HELIUM_ASSERT( pRequest );
	HELIUM_ASSERT( !pRequest->spObject );
	pRequest->index = objectIndex;
	pRequest->spType = pType;
	HELIUM_ASSERT( !pRequest->spTemplate );
	HELIUM_ASSERT( !pRequest->spOwner );
	SetInvalid( pRequest->templateLoadId );
	SetInvalid( pRequest->ownerLoadId );
	SetInvalid( pRequest->persistentResourceDataLoadId );
	pRequest->pCachedObjectDataBuffer = NULL;
	pRequest->cachedObjectDataBufferSize = 0;
	SetInvalid( pRequest->asyncFileLoadId );
	pRequest->pAsyncFileLoadBuffer = NULL;
	pRequest->asyncFileLoadBufferSize = 0;
	pRequest->pResolver = pResolver;
	pRequest->forceReload = forceReload;

	pRequest->flags = 0;

	// If a fully-loaded object already exists with the same name, do not attempt to re-load the object (just mark
	// the request as complete).

	if ( !forceReload )
	{
		pRequest->spObject = Asset::FindObject( path );
	}

	Asset* pObject = pRequest->spObject;
	if ( pObject && pObject->IsFullyLoaded() )
	{
		pRequest->flags = LOAD_FLAG_PRELOADED;
	}
	else
	{
		HELIUM_ASSERT( !pObject || !pObject->GetAnyFlagSet( Asset::FLAG_LOADED | Asset::FLAG_LINKED ) );

		// Begin loading the template and owner objects.  Note that there isn't much reason to check for failure
		// until we tick this request, as we need to make sure any other load requests for the template/owner that
		// did succeed are properly synced anyway.
		AssetLoader* pAssetLoader = AssetLoader::GetInstance();
		HELIUM_ASSERT( pAssetLoader );

		if ( rObjectData.templatePath.IsEmpty() )
		{
			// Make sure the template is fully loaded.
			Asset* pTemplate = pType->GetTemplate();
			rObjectData.templatePath = pTemplate->GetPath();
			if ( pTemplate->IsFullyLoaded() )
			{
				pRequest->spTemplate = pTemplate;
			}
			else
			{
				pRequest->templateLoadId = pAssetLoader->BeginLoadObject( rObjectData.templatePath );
			}
		}
		else
		{
			pRequest->templateLoadId = pAssetLoader->BeginLoadObject( rObjectData.templatePath );
		}

		AssetPath ownerPath = path.GetParent();
		if ( ownerPath == m_packagePath )
		{
			// Easy check: if the owner is this package (which is likely), we don't need to load it.
			pRequest->spOwner = m_spPackage.Ptr();
		}
		else if ( !ownerPath.IsEmpty() )
		{
			pRequest->ownerLoadId = pAssetLoader->BeginLoadObject( ownerPath );
		}
	}

	size_t requestId = m_loadRequests.Add( pRequest );

	return requestId;
}

/// @copydoc PackageLoader::TryFinishLoadObject()
bool LoosePackageLoader::TryFinishLoadObject( size_t requestId, AssetPtr& rspObject )
{
	HELIUM_ASSERT( requestId < m_loadRequests.GetSize() );
	HELIUM_ASSERT( m_loadRequests.IsElementValid( requestId ) );

	LoadRequest* pRequest = m_loadRequests[requestId];
	HELIUM_ASSERT( pRequest );
	if ( ( pRequest->flags & LOAD_FLAG_PRELOADED ) != LOAD_FLAG_PRELOADED )
	{
		return false;
	}

	HELIUM_ASSERT( !IsValid( pRequest->templateLoadId ) );
	HELIUM_ASSERT( !IsValid( pRequest->ownerLoadId ) );
	HELIUM_ASSERT( !IsValid( pRequest->persistentResourceDataLoadId ) );

	DefaultAllocator().Free( pRequest->pCachedObjectDataBuffer );
	pRequest->pCachedObjectDataBuffer = NULL;
	pRequest->cachedObjectDataBufferSize = 0;

	rspObject = pRequest->spObject;
	Asset* pObject = rspObject;
	if ( pObject && ( pRequest->flags & LOAD_FLAG_ERROR ) )
	{
		pObject->SetFlags( Asset::FLAG_BROKEN );
	}

	pRequest->pResolver = NULL;
	pRequest->spObject.Release();
	pRequest->spType.Release();
	pRequest->spTemplate.Release();
	pRequest->spOwner.Release();

	m_loadRequests.Remove( requestId );
	m_loadRequestPool.Release( pRequest );

	return true;
}

/// @copydoc PackageLoader::Tick()
void LoosePackageLoader::Tick()
{
	MutexScopeLock scopeLock( m_accessLock );

	// Do nothing until pre-loading has been started.
	if ( !m_startPreloadCounter )
	{
		return;
	}

	if ( !m_preloadedCounter )
	{
		// Update package preloading.
		TickPreload();
	}
	else
	{
		// Process pending dependencies.
		TickLoadRequests();
	}
}

/// @copydoc PackageLoader::GetObjectCount()
size_t LoosePackageLoader::GetObjectCount() const
{
	return m_objects.GetSize();
}

/// @copydoc PackageLoader::GetAssetPath()
AssetPath LoosePackageLoader::GetAssetPath( size_t index ) const
{
	HELIUM_ASSERT( index < m_objects.GetSize() );

	return m_objects[index].objectPath;
}

const FilePath &LoosePackageLoader::GetAssetFileSystemPath( const AssetPath &path ) const
{
	size_t index = FindObjectByName( path.GetRootName() );

	HELIUM_ASSERT( index < m_objects.GetSize() );

	return m_objects[index].filePath;

}

int64_t LoosePackageLoader::GetAssetFileSystemTimestamp( const AssetPath &path ) const
{
	size_t index = FindObjectByName( path.GetRootName() );
	HELIUM_ASSERT( index < m_objects.GetSize() );
	if ( index < m_objects.GetSize() )
	{
		return m_objects[index].fileTimeStamp;
	}
	else
	{
		return 0;
	}
}

void LoosePackageLoader::EnumerateChildren( DynamicArray< AssetPath > &children ) const
{
	for ( DynamicArray< AssetPath >::ConstIterator iter = m_childPackagePaths.Begin();
		iter != m_childPackagePaths.End(); ++iter )
	{
		children.Add( *iter );
	}

	for ( DynamicArray< SerializedObjectData >::ConstIterator iter = m_objects.Begin();
		iter != m_objects.End(); ++iter )
	{
		children.Add( iter->objectPath );
	}
}

bool LoosePackageLoader::SaveAsset( Asset *pAsset ) const
{
	HELIUM_ASSERT( pAsset );
	HELIUM_ASSERT( pAsset->GetOwningPackage() );
	HELIUM_ASSERT( pAsset->GetOwningPackage()->GetLoader() );
	HELIUM_ASSERT( pAsset->GetOwningPackage()->GetLoader() == this );
	HELIUM_ASSERT( pAsset->GetPath().GetParent() == GetPackagePath() );

	AssetIdentifier assetIdentifier;
	FilePath filepath = GetAssetFileSystemPath( pAsset->GetPath() );
	if ( HELIUM_VERIFY( !filepath.Get().empty() ) )
	{
		Persist::ArchiveWriter::WriteToFile( filepath, pAsset, &assetIdentifier );
		pAsset->ClearFlags( Asset::FLAG_CHANGED_SINCE_LOADED );
		return true;
	}

	return false;
}

/// Get the package managed by this loader.
///
/// @return  Associated package.
///
/// @see GetPackagePath()
Package* LoosePackageLoader::GetPackage() const
{
	return m_spPackage;
}

/// Get the object path for the package managed by this loader.
///
/// @return  FilePath of the associated package.
///
/// @see GetPackage()
AssetPath LoosePackageLoader::GetPackagePath() const
{
	return m_packagePath;
}

/// @copydoc PackageLoader::HasAssetFileState()
bool LoosePackageLoader::HasAssetFileState() const
{
	return true;
}

/// Update during the package preload process.
void LoosePackageLoader::TickPreload()
{
	HELIUM_ASSERT( m_startPreloadCounter != 0 );
	HELIUM_ASSERT( m_preloadedCounter == 0 );

	AsyncLoader* pAsyncLoader = AsyncLoader::GetInstance();
	HELIUM_ASSERT( pAsyncLoader );

	bool bAllFileRequestsDone = true;

	// Walk through every load request
	for ( size_t i = 0; i < m_fileReadRequests.GetSize(); )
	{
		FileReadRequest &rRequest = m_fileReadRequests[i];
		HELIUM_ASSERT( rRequest.asyncLoadId );
		HELIUM_ASSERT( rRequest.pLoadBuffer );

		size_t bytes_read = 0;
		if ( !pAsyncLoader->TrySyncRequest( rRequest.asyncLoadId, bytes_read ) )
		{
			// Havn't finished reading yet, move on to next entry
			bAllFileRequestsDone = false;
			++i;
			continue;
		}

		HELIUM_ASSERT( bytes_read == rRequest.expectedSize );
		if ( IsInvalid( bytes_read ) )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				"LoosePackageLoader: Failed to read the contents of async load request \"%d\".\n",
				rRequest.asyncLoadId );
		}
		else if ( bytes_read != rRequest.expectedSize )
		{
			HELIUM_TRACE(
				TraceLevels::Warning,
				"LoosePackageLoader: Attempted to read %" PRIuSZ " bytes from package file \"%s\", but only %" PRIuSZ " bytes were read.\n",
				rRequest.expectedSize,
				bytes_read );
		}
		else
		{
			HELIUM_ASSERT( rRequest.expectedSize < ~static_cast<size_t>( 0 ) );

			// the name is deduced from the file name (bad idea to store it in the file)
			Name name( m_fileReadRequests[i].filePath.Basename().c_str() );

			// read some preliminary data from the json
			struct PreliminaryObjectHandler : rapidjson::BaseReaderHandler<>
			{
				Helium::Name typeName;
				Helium::String templatePath;
				bool templateIsNext;

				PreliminaryObjectHandler()
					: typeName( ENullName() )
					, templatePath( "" )
				{
					templateIsNext = false;
				}

				bool Key( const Ch* chars, rapidjson::SizeType length, bool copy )
				{
					if ( typeName.IsEmpty() )
					{
						typeName.Set( Helium::String( chars, length ) );
						return true;
					}

					if ( templatePath.IsEmpty() )
					{
						Helium::String str( chars, length );

						if ( str == "m_spTemplate" )
						{
							templateIsNext = true;
							return true;
						}
					}

					return true;
				}

				bool String( const Ch* chars, rapidjson::SizeType length, bool copy )
				{
					if ( templatePath.IsEmpty() )
					{
						Helium::String str( chars, length );

						if ( templateIsNext )
						{
							templatePath = str;
							templateIsNext = false;
							return true;
						}
					}

					return true;
				}
			} handler;

			// non destructive in-place stream helper
			rapidjson::StringStream stream( static_cast<char*>( rRequest.pLoadBuffer ) );

			// the main reader object
			rapidjson::Reader reader;
			if ( reader.Parse< rapidjson::kParseDefaultFlags >( stream, handler ) )
			{
				SerializedObjectData* pObjectData = m_objects.New();
				HELIUM_ASSERT( pObjectData );
				HELIUM_VERIFY( pObjectData->objectPath.Set( name, false, m_packagePath ) );
				pObjectData->templatePath.Set( handler.templatePath );
				pObjectData->typeName = handler.typeName;
				pObjectData->filePath = rRequest.filePath;
				pObjectData->fileTimeStamp = rRequest.fileTimestamp;
				pObjectData->bMetadataGood = true;

				HELIUM_TRACE(
					TraceLevels::Debug,
					"LoosePackageLoader: Success reading preliminary data for object '%s' from file '%s'.\n",
					*name,
					rRequest.filePath.Data() );
			}
			else
			{
				HELIUM_TRACE(
					TraceLevels::Error,
					"LoosePackageLoader: Failure reading preliminary data for object '%s' from file '%s': Error parsing JSON (%d): %s\n",
					*name,
					rRequest.filePath.Data(),
					reader.GetErrorOffset(),
					rapidjson::GetParseError_En( reader.GetParseErrorCode() ) );
			}
		}

		// We're finished with this load, so deallocate memory and get rid of the request
		DefaultAllocator().Free( rRequest.pLoadBuffer );
		rRequest.pLoadBuffer = NULL;
		SetInvalid( rRequest.asyncLoadId );
		m_fileReadRequests.RemoveSwap( i );
	}

	// Wait for the parent package to finish loading.
	AssetPtr spParentPackage;
	if ( IsValid( m_parentPackageLoadId ) )
	{
		AssetLoader* pAssetLoader = AssetLoader::GetInstance();
		HELIUM_ASSERT( pAssetLoader );
		if ( !pAssetLoader->TryFinishLoad( m_parentPackageLoadId, spParentPackage ) )
		{
			return;
		}

		SetInvalid( m_parentPackageLoadId );

		// Package loading should not fail.  If it does, this is a sign of a potentially serious issue.
		HELIUM_ASSERT( spParentPackage );
	}

	// Everything beyond this point "finalizes" the package preload, so stop here if we aren't ready to go
	if ( !bAllFileRequestsDone )
	{
		return;
	}

	// Create the package object if it does not yet exist.
	Package* pPackage = m_spPackage;
	if ( !pPackage )
	{
		HELIUM_TRACE( TraceLevels::Debug, "LoosePackageLoader: Creating package object: %s\n", *m_packagePath.ToString() );
		HELIUM_ASSERT( spParentPackage ? !m_packagePath.GetParent().IsEmpty() : m_packagePath.GetParent().IsEmpty() );
		HELIUM_VERIFY( Asset::Create< Package >( m_spPackage, m_packagePath.GetName(), spParentPackage ) );
		pPackage = m_spPackage;
		HELIUM_ASSERT( pPackage );
		pPackage->SetLoader( this );
	}

	HELIUM_ASSERT( pPackage->GetLoader() == this );

	FilePath packageDirectoryPath;

	if ( !FileLocations::GetDataDirectory( packageDirectoryPath ) )
	{
		HELIUM_TRACE( TraceLevels::Error, "LoosePackageLoader::TickPreload(): Could not get data directory.\n" );
		return;
	}

	packageDirectoryPath += m_packagePath.ToFilePathString().GetData();
	packageDirectoryPath += "/";

	DirectoryIterator packageDirectory( packageDirectoryPath );
	for ( ; !packageDirectory.IsDone(); packageDirectory.Next() )
	{
		const DirectoryIteratorItem& item = packageDirectory.GetItem();

		if ( !item.m_Path.IsFile() )
		{
			continue;
		}

		Name objectName( item.m_Path.Filename().Data() );
		String objectNameString( item.m_Path.Filename().Data() );

		size_t objectIndex = FindObjectByName( objectName );

		if ( objectIndex != Invalid< size_t >() )
		{
			m_objects[objectIndex].fileTimeStamp = Helium::Max(
				m_objects[objectIndex].fileTimeStamp,
				static_cast<int64_t>( packageDirectory.GetItem().m_ModTime ) );

			continue;
		}

		// Check the extension to see if the file is supported by one of the resource handlers.
		ResourceHandler* pBestHandler = ResourceHandler::GetBestResourceHandlerForFile( objectNameString );

		if ( pBestHandler )
		{
			// File extension matches a supported source asset type, so add it to the object list.
			const AssetType* pResourceType = pBestHandler->GetResourceType();
			HELIUM_ASSERT( pResourceType );

			HELIUM_TRACE(
				TraceLevels::Debug,
				"LoosePackageLoader: Registered source asset file \"%s\" as as instance of resource type \"%s\" in package \"%s\".\n",
				*objectNameString,
				*pResourceType->GetName(),
				*m_packagePath.ToString() );

			SerializedObjectData* pObjectData = m_objects.New();
			HELIUM_ASSERT( pObjectData );
			HELIUM_VERIFY( pObjectData->objectPath.Set( objectName, false, m_packagePath ) );
			pObjectData->typeName = pResourceType->GetName();
			pObjectData->templatePath.Clear();
			pObjectData->filePath.Clear();
			pObjectData->fileTimeStamp = packageDirectory.GetItem().m_ModTime;
			pObjectData->bMetadataGood = true;
		}
		else
		{
			HELIUM_TRACE(
				TraceLevels::Debug,
				"LoosePackageLoader: Did not recognize \"%s\" as as instance of resource in package \"%s\".\n",
				*objectNameString,
				*m_packagePath.ToString() );

			for ( AssetType::ConstIterator iter = AssetType::GetTypeBegin();
				iter != AssetType::GetTypeEnd(); ++iter )
			{
				HELIUM_TRACE(
					TraceLevels::Info,
					" - %s\n",
					*iter->GetName() );
			}
		}
	}

	// Package preloading is now complete.
	pPackage->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );
	pPackage->ConditionalFinalizeLoad();

	AtomicExchangeRelease( m_preloadedCounter, 1 );

	LooseAssetLoader::OnPackagePreloaded( this );
}

/// Update load processing of object load requests.
void LoosePackageLoader::TickLoadRequests()
{
	size_t loadRequestCount = m_loadRequests.GetSize();
	for ( size_t loadRequestIndex = 0; loadRequestIndex < loadRequestCount; ++loadRequestIndex )
	{
		if ( !m_loadRequests.IsElementValid( loadRequestIndex ) )
		{
			continue;
		}

		LoadRequest* pRequest = m_loadRequests[loadRequestIndex];
		HELIUM_ASSERT( pRequest );

		if ( !( pRequest->flags & LOAD_FLAG_PROPERTY_PRELOADED ) )
		{
			if ( !TickDeserialize( pRequest ) )
			{
				continue;
			}
		}

		//TODO: Investigate removing need to preload properties first. Probably need to have the
		//      restriction as TickPersistentResourcePreload assumes the object exists.. but this
		//      may not be the best place to put this 
		// We can probably remove these continues..
		if ( !( pRequest->flags & LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED ) )
		{
			if ( !TickPersistentResourcePreload( pRequest ) )
			{
				continue;
			}
		}
	}
}

size_t LoosePackageLoader::FindObjectByPath( const AssetPath &path ) const
{
	// Locate the object within this package.
	size_t objectCount = m_objects.GetSize();
	for ( size_t objectIndex = 0; objectIndex < objectCount; ++objectIndex )
	{
		const SerializedObjectData& rObjectData = m_objects[objectIndex];
		if ( rObjectData.objectPath == path )
		{
			return objectIndex;
		}
	}

	return Invalid<size_t>();
}

size_t LoosePackageLoader::FindObjectByName( const Name &name ) const
{
	// Locate the object within this package.
	size_t objectCount = m_objects.GetSize();
	for ( size_t objectIndex = 0; objectIndex < objectCount; ++objectIndex )
	{
		const SerializedObjectData& rObjectData = m_objects[objectIndex];
		if ( rObjectData.objectPath.GetName() == name )
		{
			return objectIndex;
		}
	}

	return Invalid<size_t>();
}

/// Update processing of object property preloading for a given load request.
///
/// @param[in] pRequest  Load request to process.
///
/// @return  True if object property preloading for the given load request has completed, false if not.
bool LoosePackageLoader::TickDeserialize( LoadRequest* pRequest )
{
	HELIUM_ASSERT( pRequest );
	HELIUM_ASSERT( !( pRequest->flags & LOAD_FLAG_PROPERTY_PRELOADED ) );

	Asset* pObject = pRequest->spObject;

	HELIUM_ASSERT( pRequest->index < m_objects.GetSize() );
	SerializedObjectData& rObjectData = m_objects[pRequest->index];

	// Wait for the template and owner objects to load.
	AssetLoader* pAssetLoader = AssetLoader::GetInstance();
	HELIUM_ASSERT( pAssetLoader );

	if ( !rObjectData.templatePath.IsEmpty() )
	{
		if ( IsValid( pRequest->templateLoadId ) )
		{
			if ( !pAssetLoader->TryFinishLoad( pRequest->templateLoadId, pRequest->spTemplate ) )
			{
				return false;
			}

			SetInvalid( pRequest->templateLoadId );
		}

		if ( !pRequest->spTemplate )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				"LoosePackageLoader: Failed to load template object for \"%s\".\n",
				*rObjectData.objectPath.ToString() );

			if ( pObject )
			{
				pObject->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );
				pObject->ConditionalFinalizeLoad();
			}

			pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;

			return true;
		}
	}

	HELIUM_ASSERT( IsInvalid( pRequest->templateLoadId ) );
	Asset* pTemplate = pRequest->spTemplate;

	AssetPath ownerPath = rObjectData.objectPath.GetParent();
	if ( !ownerPath.IsEmpty() )
	{
		if ( IsValid( pRequest->ownerLoadId ) )
		{
			if ( !pAssetLoader->TryFinishLoad( pRequest->ownerLoadId, pRequest->spOwner ) )
			{
				return false;
			}

			SetInvalid( pRequest->ownerLoadId );
		}

		if ( !pRequest->spOwner )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				"LoosePackageLoader: Failed to load owner object for \"%s\".\n",
				*rObjectData.objectPath.ToString() );

			if ( pObject )
			{
				pObject->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );
				pObject->ConditionalFinalizeLoad();
			}

			pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;

			return true;
		}
	}

	HELIUM_ASSERT( IsInvalid( pRequest->ownerLoadId ) );
	Asset* pOwner = pRequest->spOwner;

	AssetType* pType = pRequest->spType;
	HELIUM_ASSERT( pType );

	HELIUM_ASSERT( !pOwner || pOwner->IsFullyLoaded() );
	HELIUM_ASSERT( !pTemplate || pTemplate->IsFullyLoaded() );

	AsyncLoader* pAsyncLoader = AsyncLoader::GetInstance();
	HELIUM_ASSERT( pAsyncLoader );

	FilePath object_file_path = m_packageDirPath + *rObjectData.objectPath.GetName() + "." + Persist::ArchiveExtensions[Persist::ArchiveTypes::Json];

	bool load_properties_from_file = true;
	size_t object_file_size = 0;
	if ( !IsValid( pRequest->asyncFileLoadId ) )
	{
		if ( !object_file_path.IsFile() )
		{
			if ( pType->GetMetaClass()->IsType( Reflect::GetMetaClass< Resource >() ) )
			{
				HELIUM_TRACE(
					TraceLevels::Info,
					"LoosePackageLoader::TickDeserialize(): No object file found for resource \"%s\". Expected file location: \"%s\". This is normal for newly added resources.\n",
					*rObjectData.objectPath.ToString(),
					object_file_path.Data() );

				// We will allow continuing to load using all default properties. This behavior is to support dropping resources into the 
				// data property and autogenerating objects from them.
				load_properties_from_file = false;
			}
			else
			{
				HELIUM_TRACE(
					TraceLevels::Warning,
					"LoosePackageLoader::TickDeserialize(): No object file found for object \"%s\". Expected file location: \"%s\"\n",
					*rObjectData.objectPath.ToString(),
					object_file_path.Data() );
			}
		}
		else
		{
			Status status;
			status.Read( object_file_path.Get().c_str() );
			int64_t i64_object_file_size = status.m_Size;

			if ( i64_object_file_size == -1 )
			{
				HELIUM_TRACE(
					TraceLevels::Warning,
					"LoosePackageLoader::TickDeserialize(): Could not get file size for object file of object \"%s\". Expected file location: \"%s\"\n",
					*rObjectData.objectPath.ToString(),
					object_file_path.Data() );
			}
			else if ( i64_object_file_size == 0 )
			{
				HELIUM_TRACE(
					TraceLevels::Warning,
					"LoosePackageLoader::TickDeserialize(): Object file \"%s\" for objct \"%s\" is empty.\n",
					object_file_path.Data(),
					*rObjectData.objectPath.ToString() );
			}
			else if ( static_cast<uint64_t>( i64_object_file_size ) > static_cast<uint64_t>( ~static_cast<size_t>( 0 ) ) )
			{
				HELIUM_TRACE(
					TraceLevels::Error,
					"LoosePackageLoader::TickDeserialize(): Object file \"%s\" exceeds the maximum size supported by the current platform (file size: %" PRIu64 " bytes; max supported: %" PRIuSZ " bytes).\n",
					object_file_path.Data(),
					static_cast<uint64_t>( i64_object_file_size ),
					~static_cast<size_t>( 0 ) );
			}
			else
			{
				object_file_size = static_cast<size_t>( i64_object_file_size );
			}
		}

		if ( !load_properties_from_file )
		{
			HELIUM_ASSERT( !object_file_size );
		}
		else if ( !object_file_size )
		{
			pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;
			return true;
		}
		else
		{
			HELIUM_ASSERT( !pRequest->pAsyncFileLoadBuffer );
			pRequest->pAsyncFileLoadBuffer = DefaultAllocator().Allocate( object_file_size );
			HELIUM_ASSERT( pRequest->pAsyncFileLoadBuffer );

			pRequest->asyncFileLoadBufferSize = object_file_size;

			pRequest->asyncFileLoadId = pAsyncLoader->QueueRequest(
				pRequest->pAsyncFileLoadBuffer,
				String( object_file_path.Data() ),
				0,
				pRequest->asyncFileLoadBufferSize );
		}

	}

	size_t bytesRead = 0;
	if ( load_properties_from_file )
	{
		HELIUM_ASSERT( IsValid( pRequest->asyncFileLoadId ) );

		if ( !pAsyncLoader->TrySyncRequest( pRequest->asyncFileLoadId, bytesRead ) )
		{
			return false;
		}
	}

	/////// POINT OF NO RETURN: We *will* return true after this point, and the object *will* be finished preloading,
	/////// for good or for bad.

	SetInvalid( pRequest->asyncFileLoadId );
	bool object_creation_failure = false;

	// If we already had an existing object, make sure the type and template match.
	if ( pObject )
	{
		const AssetType* pExistingType = pObject->GetAssetType();
		HELIUM_ASSERT( pExistingType );
		if ( pExistingType != pType )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				"LoosePackageLoader: Cannot load \"%s\" using the existing object as the types do not match (existing type: \"%s\"; serialized type: \"%s\".\n",
				*rObjectData.objectPath.ToString(),
				*pExistingType->GetName(),
				*pType->GetName() );

			pObject->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );
			pObject->ConditionalFinalizeLoad();

			object_creation_failure = true;
		}
	}
	else
	{
		bool bCreateResult = false;
		if ( pRequest->forceReload )
		{
			// Create the object.
			bCreateResult = Asset::CreateObject(
				pRequest->spObject,
				pType,
				Name( NULL_NAME ),
				NULL,
				pTemplate );
		}
		else
		{
			// Create the object.
			bCreateResult = Asset::CreateObject(
				pRequest->spObject,
				pType,
				rObjectData.objectPath.GetName(),
				pOwner,
				pTemplate );
		}

		if ( !bCreateResult )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				"LoosePackageLoader: Failed to create \"%s\" during loading.\n",
				*rObjectData.objectPath.ToString() );

			object_creation_failure = true;
		}

		pObject = pRequest->spObject;
		HELIUM_ASSERT( pObject );
	}

	if ( load_properties_from_file && !object_creation_failure )
	{
		// Sanity checks for file load, then success path
		HELIUM_ASSERT( bytesRead == pRequest->asyncFileLoadBufferSize );
		if ( IsInvalid( bytesRead ) )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				"LoosePackageLoader: Failed to read the contents of object file \"%s\" in async load request \"%d\".\n",
				object_file_path.Data(),
				pRequest->asyncFileLoadId );
		}
		else if ( bytesRead != pRequest->asyncFileLoadBufferSize )
		{
			HELIUM_TRACE(
				TraceLevels::Warning,
				"LoosePackageLoader: Attempted to read %" PRIuSZ " bytes from object file \"%s\", but only %" PRIuSZ " bytes were read.\n",
				pRequest->asyncFileLoadBufferSize,
				object_file_path.Data(),
				bytesRead );
		}
		else
		{
			StaticMemoryStream archiveStream( pRequest->pAsyncFileLoadBuffer, pRequest->asyncFileLoadBufferSize );

			HELIUM_TRACE(
				TraceLevels::Info,
				"LoosePackageLoader: Reading %s. pResolver = %x\n",
				object_file_path.Data(),
				pRequest->pResolver );

			DynamicArray< Reflect::ObjectPtr > objects;
			objects.Push( pRequest->spObject.Get() ); // use existing objects
			Persist::ArchiveReaderJson::ReadFromStream( archiveStream, objects, pRequest->pResolver );
			HELIUM_ASSERT( objects[0].Get() == pRequest->spObject.Get() );
		}
	}

	if ( load_properties_from_file )
	{
		DefaultAllocator().Free( pRequest->pAsyncFileLoadBuffer );
		pRequest->pAsyncFileLoadBuffer = NULL;
		pRequest->asyncFileLoadBufferSize = 0;
	}

	pRequest->flags |= LOAD_FLAG_PROPERTY_PRELOADED;

	if ( object_creation_failure )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"LoosePackageLoader: Deserialization of object \"%s\" failed.\n",
			*rObjectData.objectPath.ToString() );

		pObject->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );
		pObject->ConditionalFinalizeLoad();

		pRequest->flags |= LOAD_FLAG_ERROR;
	}
	else if ( !pObject->IsDefaultTemplate() )
	{
		// If the object is a resource (not including the default template object for resource types), attempt to begin
		// loading any existing persistent resource data stored in the object cache.
		Resource* pResource = Reflect::SafeCast< Resource >( pObject );
		if ( pResource )
		{
			Name objectCacheName = Name( HELIUM_ASSET_CACHE_NAME );
			CacheManager* pCacheManager = CacheManager::GetInstance();
			HELIUM_ASSERT( pCacheManager );

			Cache* pCache = pCacheManager->GetCache( objectCacheName );
			HELIUM_ASSERT( pCache );
			pCache->EnforceTocLoad();

			const Cache::Entry* pEntry = pCache->FindEntry( rObjectData.objectPath, 0 );
			if ( pEntry && pEntry->size != 0 )
			{
				HELIUM_ASSERT( IsInvalid( pRequest->persistentResourceDataLoadId ) );
				HELIUM_ASSERT( !pRequest->pCachedObjectDataBuffer );

				pRequest->pCachedObjectDataBuffer =
					static_cast<uint8_t*>( DefaultAllocator().Allocate( pEntry->size ) );
				HELIUM_ASSERT( pRequest->pCachedObjectDataBuffer );
				pRequest->cachedObjectDataBufferSize = pEntry->size;

				AsyncLoader* pAsyncLoader = AsyncLoader::GetInstance();
				HELIUM_ASSERT( pAsyncLoader );

				pRequest->persistentResourceDataLoadId = pAsyncLoader->QueueRequest(
					pRequest->pCachedObjectDataBuffer,
					pCache->GetCacheFileName(),
					pEntry->offset,
					pEntry->size );
				HELIUM_ASSERT( IsValid( pRequest->persistentResourceDataLoadId ) );
			}
		}
	}

	if ( IsInvalid( pRequest->persistentResourceDataLoadId ) )
	{
		// No persistent resource data needs to be loaded.
		pObject->SetFlags( Asset::FLAG_PRELOADED );
		pRequest->flags |= LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED;
	}

	// Asset is now preloaded.
	return true;
}

/// Update processing of persistent resource data loading for a given load request.
///
/// @param[in] pRequest  Load request to process.
///
/// @return  True if persistent resource data loading for the given load request has completed, false if not.
bool LoosePackageLoader::TickPersistentResourcePreload( LoadRequest* pRequest )
{
	HELIUM_ASSERT( pRequest );
	HELIUM_ASSERT( !( pRequest->flags & LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED ) );

	Resource* pResource = Reflect::AssertCast< Resource >( pRequest->spObject.Get() );
	HELIUM_ASSERT( pResource );

	AsyncLoader* pAsyncLoader = AsyncLoader::GetInstance();
	HELIUM_ASSERT( pAsyncLoader );

	// Wait for the cached data load to complete.
	size_t bytesRead = 0;
	HELIUM_ASSERT( IsValid( pRequest->persistentResourceDataLoadId ) );
	if ( !pAsyncLoader->TrySyncRequest( pRequest->persistentResourceDataLoadId, bytesRead ) )
	{
		return false;
	}

	SetInvalid( pRequest->persistentResourceDataLoadId );

	if ( bytesRead != pRequest->cachedObjectDataBufferSize )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			"LoosePackageLoader: Requested load of %" PRIu32 " bytes from cached object data for \"%s\", but only %" PRIuSZ " bytes were read.\n",
			pRequest->cachedObjectDataBufferSize,
			*pResource->GetPath().ToString(),
			bytesRead );
	}

	// Make sure we read enough bytes to cover the object property data size.
	if ( bytesRead < sizeof( uint32_t ) )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"LoosePackageLoader: Not enough bytes read of cached object data \"%s\" from which to parse the property stream size.\n",
			*pResource->GetPath().ToString() );

		pRequest->flags |= LOAD_FLAG_ERROR;
	}
	else
	{
		// Skip over the object property data.
		uint8_t* pCachedObjectData = pRequest->pCachedObjectDataBuffer;
		HELIUM_ASSERT( pCachedObjectData );

		uint32_t propertyDataSize = *reinterpret_cast<uint32_t*>( pCachedObjectData );

		size_t byteSkipCount = sizeof( propertyDataSize ) + propertyDataSize;
		if ( byteSkipCount > bytesRead )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				"LoosePackageLoader: Cached persistent resource data for \"%s\" extends past the end of the cached data stream.\n",
				*pResource->GetPath().ToString() );

			pRequest->flags |= LOAD_FLAG_ERROR;
		}
		else
		{
			pCachedObjectData += byteSkipCount;
			size_t bytesRemaining = bytesRead - byteSkipCount;

			// Make sure we have enough bytes at the end for the resource sub-data count.
			if ( bytesRemaining < sizeof( uint32_t ) )
			{
				HELIUM_TRACE(
					TraceLevels::Error,
					"LoosePackageLoader: Not enough space is reserved in the cached persistent resource data stream for \"%s\" for the resource sub-data count.\n",
					*pResource->GetPath().ToString() );

				pRequest->flags |= LOAD_FLAG_ERROR;
			}
			else
			{
				bytesRemaining -= sizeof( uint32_t );

				//// Deserialize the persistent resource data.
				//// Having to do this copy is unfortunate.. maybe we can revisit this later
				//std::stringstream ss_in;
				//ss_in.write(reinterpret_cast<char *>(pCachedObjectData), bytesRemaining);

				//Reflect::ArchiveBinary archive(new Reflect::CharStream(&ss_in, false, Helium::ByteOrders::LittleEndian, Helium::Reflect::CharacterEncodings::UTF_16), false);

				//Reflect::ObjectPtr persistent_data;
				//archive.ReadSingleObject(persistent_data);

				Reflect::ObjectPtr persistent_data;
				persistent_data = Cache::ReadCacheObjectFromBuffer( pCachedObjectData, /*sizeof( uint32_t )*/ 0, bytesRemaining, pRequest->pResolver );

				if ( !pResource->LoadPersistentResourceObject( persistent_data ) )
				{
					HELIUM_TRACE(
						TraceLevels::Error,
						"LoosePackageLoader: Failed to load persistent resource object for \"%s\".\n",
						*pResource->GetPath().ToString() );

					pRequest->flags |= LOAD_FLAG_ERROR;
				}
			}
		}
	}

	DefaultAllocator().Free( pRequest->pCachedObjectDataBuffer );
	pRequest->pCachedObjectDataBuffer = NULL;
	pRequest->cachedObjectDataBufferSize = 0;

	pResource->SetFlags( Asset::FLAG_PRELOADED );

	pRequest->flags |= LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED;

	return true;
}
