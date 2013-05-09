//----------------------------------------------------------------------------------------------------------------------
// XmlPackageLoader.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

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
#include "Engine/AssetType.h"
#include "Engine/AssetLoader.h"
#include "Engine/Resource.h"
#include "PcSupport/AssetPreprocessor.h"
#include "PcSupport/ResourceHandler.h"
#include "Reflect/TranslatorDeduction.h"
#include "Persist/ArchiveJson.h"

REFLECT_DEFINE_OBJECT( Helium::ObjectDescriptor );

using namespace Helium;

void Helium::ObjectDescriptor::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField(&ObjectDescriptor::m_Name, TXT("m_Name"));
	comp.AddField(&ObjectDescriptor::m_TypeName, TXT("m_TypeName"));
	comp.AddField(&ObjectDescriptor::m_TemplatePath, TXT("m_TemplatePath"));
}

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
	Shutdown();
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
	Shutdown();

	// Make sure the path represents a package.
	if( packagePath.IsEmpty() )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "LoosePackageLoader::Initialize(): Empty package path specified.\n" ) );

		return false;
	}

	HELIUM_TRACE(
		TraceLevels::Debug,
		TXT( "LoosePackageLoader::Initialize(): Initializing loader for package \"%s\".\n" ),
		*packagePath.ToString() );

	if( !packagePath.IsPackage() )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "LoosePackageLoader::Initialize(): \"%s\" does not represent a package path.\n" ),
			*packagePath.ToString() );

		return false;
	}

	// Store the package path.
	m_packagePath = packagePath;

	// Attempt to locate the specified package if it already happens to exist.
	m_spPackage = Asset::Find< Package >( packagePath );
	Package* pPackage = m_spPackage;
	if( pPackage )
	{
		if( pPackage->GetLoader() )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "LoosePackageLoader::Initialize(): Package \"%s\" already has a loader.\n" ),
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
		if( spObject )
		{
			HELIUM_ASSERT( !spObject->IsPackage() );

			HELIUM_TRACE(
				TraceLevels::Error,
				( TXT( "PackageLoader::Initialize(): Package loader cannot be initialized for \"%s\", as an " )
				TXT( "object with the same name exists that is not a package.\n" ) ),
				*packagePath.ToString() );

			return false;
		}
	}

	// Build the package file path.  If the package is a user configuration package, use the user data directory,
	// otherwise use the global data directory.
	Config& rConfig = Config::GetStaticInstance();
	FilePath dataDirectory;

	if ( packagePath == rConfig.GetUserConfigPackagePath() )
	{
		if ( !FileLocations::GetUserDataDirectory( dataDirectory ) )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "PackageLoader::Initialize(): Could not obtain user data directory." ) );

			return false;
		}
	}
	else
	{
		if ( !FileLocations::GetDataDirectory( dataDirectory ) )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "PackageLoader::Initialize(): Could not obtain user data directory." ) );

			return false;
		}
	}

	// Set up to read the TOC (which may not exist)
	//SetInvalid( m_packageTocFileSize );

	// First do this check without a trailing "/" so that FilePath has to actually look at the file system
	FilePath package_dir = dataDirectory + packagePath.ToFilePathString().GetData();
	
	if (!package_dir.Exists())
	{
		// Some packages like types or uninitialized user config packages may not exist on file system
		m_packageDirPath = package_dir + TXT("/");
		return true;
	}

	if (!package_dir.IsDirectory())
	{
		// Packages should not be files
		return false;
	}
	
	// But internally we will store this 
	m_packageDirPath = package_dir + TXT("/");

	//m_packageTocFilePath = m_packageDirPath + HELIUM_ARCHIVE_PACKAGE_TOC_FILENAME;
	
//     if (!m_packageTocFilePath.IsFile())
//     {
//         HELIUM_TRACE(
//             TraceLevels::Warning,
//             TXT( "LoosePackageLoader::Initialize(): No TOC file for package \"%s\". Expected file location: \"%s\"\n" ),
//             *m_packagePath.ToString(),
//             *m_packageDirPath);
//     }
//     else
//     {
//         int64_t packageFileSize = m_packageTocFilePath.Size();
// 
//         if( packageFileSize == -1 )
//         {
//             HELIUM_TRACE(
//                 TraceLevels::Warning,
//                 TXT( "LoosePackageLoader::Initialize(): Could not get file size for TOC of package \"%s\". Expected file location: \"%s\"\n" ),
//                 *m_packagePath.ToString(),
//                 *m_packageDirPath );
//         }
//         else if( packageFileSize == 0 )
//         {
//             HELIUM_TRACE(
//                 TraceLevels::Warning,
//                 TXT( "LoosePackageLoader::Initialize(): Package TOC file \"%s\" for package \"%s\" is empty.\n" ),
//                 *m_packageTocFilePath,
//                 *packagePath.ToString() );
//         }
//         else if( static_cast< uint64_t >( packageFileSize ) > static_cast< uint64_t >( ~static_cast< size_t >( 0 ) ) )
//         {
//             HELIUM_TRACE(
//                 TraceLevels::Error,
//                 ( TXT( "LoosePackageLoader::Initialize(): Package TOC file \"%s\" exceeds the maximum size supported by " )
//                 TXT( "the current platform (package: %" ) TPRIu64 TXT( " bytes; max supported: %" ) TPRIuSZ
//                 TXT( " bytes).\n" ) ),
//                 m_packageTocFilePath.c_str(),
//                 static_cast< uint64_t >( packageFileSize ),
//                 ~static_cast< size_t >( 0 ) );
//             return false;
//         }
//         else
//         {
//             // We know the TOC exists now, so set up the load
//             m_packageTocFileSize = static_cast< size_t >( packageFileSize );
//         }
// 
//     }

	return true;
}

/// @copydoc PackageLoader::Shutdown()
void LoosePackageLoader::Shutdown()
{
	// Sync with any in-flight async load requests.
	if( m_startPreloadCounter )
	{
		while( !TryFinishPreload() )
		{
			Tick();
		}
	}
	
	HELIUM_ASSERT( IsInvalid( m_parentPackageLoadId ) );

	// Unset the reference back to this loader in the package.
	Package* pPackage = m_spPackage;
	if( pPackage )
	{
		pPackage->SetLoader( NULL );
	}

	m_spPackage.Release();
	m_packagePath.Clear();

	AtomicExchangeRelease( m_startPreloadCounter, 0 );
	AtomicExchangeRelease( m_preloadedCounter, 0 );

	m_objects.Clear();

	size_t loadRequestCount = m_loadRequests.GetSize();
	for( size_t requestIndex = 0; requestIndex < loadRequestCount; ++requestIndex )
	{
		if( m_loadRequests.IsElementValid( requestIndex ) )
		{
			LoadRequest* pRequest = m_loadRequests[ requestIndex ];
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
	MutexScopeLock scopeLock( m_accessLock );

	HELIUM_ASSERT( !m_startPreloadCounter );
	HELIUM_ASSERT( !m_preloadedCounter );
	HELIUM_ASSERT( IsInvalid( m_parentPackageLoadId ) );

	// Load the parent package if we need to create the current package.
	if( !m_spPackage )
	{
		AssetPath parentPackagePath = m_packagePath.GetParent();
		if( !parentPackagePath.IsEmpty() )
		{
			AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
			HELIUM_ASSERT( pAssetLoader );

			m_parentPackageLoadId = pAssetLoader->BeginLoadObject( parentPackagePath );
		}
	}

//     if ( IsValid( m_packageTocFileSize ) )
//     {
//         HELIUM_ASSERT( !m_pTocLoadBuffer );
//         m_pTocLoadBuffer = DefaultAllocator().Allocate( m_packageTocFileSize );
//         HELIUM_ASSERT( m_pTocLoadBuffer );
// 
//         AsyncLoader &rAsyncLoader = AsyncLoader::GetStaticInstance();
//         m_tocAsyncLoadId = rAsyncLoader.QueueRequest( m_pTocLoadBuffer, String(m_packageTocFilePath.c_str()), 0, m_packageTocFileSize );
//         HELIUM_ASSERT( IsValid(m_tocAsyncLoadId) );
//     }

	AsyncLoader &rAsyncLoader = AsyncLoader::GetStaticInstance();
	//rAsyncLoader.
	
	DirectoryIterator packageDirectory( m_packageDirPath );
	
	HELIUM_TRACE( TraceLevels::Info, TXT(" LoosePackageLoader::BeginPreload - Issuing read requests for all files in %s\n"), m_packageDirPath.c_str() );

	for( ; !packageDirectory.IsDone(); packageDirectory.Next() )
	{
		const DirectoryIteratorItem& item = packageDirectory.GetItem();

		if ( item.m_Path.Extension() == Persist::ArchiveExtensions[ Persist::ArchiveTypes::Json ] )
		{
			HELIUM_TRACE( TraceLevels::Info, TXT("- Reading file [%s]\n"), item.m_Path.c_str() );

			FileReadRequest *request = m_fileReadRequests.New();
			request->expectedSize = item.m_Size;

			HELIUM_ASSERT( item.m_Size < UINT32_MAX );

			// Create a buffer for the file to be read into temporarily
			request->pLoadBuffer = DefaultAllocator().Allocate( static_cast< size_t > ( item.m_Size ) );
			HELIUM_ASSERT( request->pLoadBuffer );

			// Queue up the read
			request->asyncLoadId = rAsyncLoader.QueueRequest( request->pLoadBuffer, String( item.m_Path.c_str() ), 0, static_cast< size_t >( item.m_Size ) );
			HELIUM_ASSERT( IsValid( request->asyncLoadId ) );

			request->filePath = item.m_Path;
			request->fileTimestamp = item.m_ModTime;
		}
		else
		{
			HELIUM_TRACE( TraceLevels::Info, TXT("- Skipping file [%s] (Extension is %s)\n"), item.m_Path.c_str(), item.m_Path.Extension().c_str() );
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
size_t LoosePackageLoader::BeginLoadObject( AssetPath path, Reflect::ObjectResolver *pResolver )
{	
	HELIUM_TRACE( TraceLevels::Info, TXT(" LoosePackageLoader::BeginLoadObject - Loading path %s\n"), *path.ToString() );

	HELIUM_TRACE(
		TraceLevels::Debug,
		TXT( "LoosePackageLoader::BeginLoadObject: Beginning load for path \"%s\".\n"),
		*path.ToString());

	
	HELIUM_TRACE(
		TraceLevels::Debug,
		TXT( "LoosePackageLoader::BeginLoadObject: Beginning load for path \"%s\". pResolver = %x\n"),
		*path.ToString(),
		pResolver);

	// Make sure preloading has completed.
	HELIUM_ASSERT( m_preloadedCounter != 0 );
	if( !m_preloadedCounter )
	{
		return Invalid< size_t >();
	}

	// If this package is requested, simply provide the (already loaded) package instance.
	if( path == m_packagePath )
	{
		LoadRequest* pRequest = m_loadRequestPool.Allocate();
		HELIUM_ASSERT( pRequest );

		HELIUM_ASSERT( m_spPackage );
		pRequest->spObject = m_spPackage;

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

		pRequest->flags = LOAD_FLAG_PRELOADED;

		size_t requestId = m_loadRequests.Add( pRequest );

		return requestId;
	}

	size_t objectIndex = FindObjectByPath( path );
	size_t objectCount = GetObjectCount();

	if( objectIndex >= objectCount )
	{
		HELIUM_TRACE(
			TraceLevels::Info,
			TXT( "LoosePackageLoader::BeginLoadObject(): Failed to locate \"%s\" for loading.\n" ),
			*path.ToString() );

		return Invalid< size_t >();
	}

	SerializedObjectData& rObjectData = m_objects[ objectIndex ];

	// Verify that the metadata was read successfully
	if( !rObjectData.bMetadataGood )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "LoosePackageLoader::BeginLoadObject(): Failed to read metadata for object \"%s\" during PackagePreload. Search log for parsing errors.\n" ),
			*path.ToString() );

		return Invalid< size_t >();
	}

	// Locate the type object.
	HELIUM_ASSERT( !rObjectData.typeName.IsEmpty() );
	AssetType* pType = AssetType::Find( rObjectData.typeName );
	if( !pType )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "LoosePackageLoader::BeginLoadObject(): Failed to locate type \"%s\" for loading object \"%s\".\n" ),
			*rObjectData.typeName,
			*path.ToString() );

		return Invalid< size_t >();
	}

#ifndef NDEBUG
	size_t loadRequestSize = m_loadRequests.GetSize();
	for( size_t loadRequestIndex = 0; loadRequestIndex < loadRequestSize; ++loadRequestIndex )
	{
		if( !m_loadRequests.IsElementValid( loadRequestIndex ) )
		{
			continue;
		}

		LoadRequest* pRequest = m_loadRequests[ loadRequestIndex ];
		HELIUM_ASSERT( pRequest );
		HELIUM_ASSERT( pRequest->index != objectIndex );
		if( pRequest->index == objectIndex )
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

	pRequest->flags = 0;

	// If a fully-loaded object already exists with the same name, do not attempt to re-load the object (just mark
	// the request as complete).
	pRequest->spObject = Asset::FindObject( path );

	Asset* pObject = pRequest->spObject;
	if( pObject && pObject->IsFullyLoaded() )
	{
		pRequest->flags = LOAD_FLAG_PRELOADED;
	}
	else
	{
		HELIUM_ASSERT( !pObject || !pObject->GetAnyFlagSet( Asset::FLAG_LOADED | Asset::FLAG_LINKED ) );

		// Begin loading the template and owner objects.  Note that there isn't much reason to check for failure
		// until we tick this request, as we need to make sure any other load requests for the template/owner that
		// did succeed are properly synced anyway.
		AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
		HELIUM_ASSERT( pAssetLoader );

		if( rObjectData.templatePath.IsEmpty() )
		{
			// Make sure the template is fully loaded.
			Asset* pTemplate = pType->GetTemplate();
			rObjectData.templatePath = pTemplate->GetPath();
			if( pTemplate->IsFullyLoaded() )
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
		if( ownerPath == m_packagePath )
		{
			// Easy check: if the owner is this package (which is likely), we don't need to load it.
			pRequest->spOwner = m_spPackage;
		}
		else if( !ownerPath.IsEmpty() )
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

	LoadRequest* pRequest = m_loadRequests[ requestId ];
	HELIUM_ASSERT( pRequest );
	if( ( pRequest->flags & LOAD_FLAG_PRELOADED ) != LOAD_FLAG_PRELOADED )
	{
		return false;
	}

	HELIUM_ASSERT ( !IsValid( pRequest->templateLoadId ) );
	HELIUM_ASSERT ( !IsValid( pRequest->ownerLoadId ) );
	HELIUM_ASSERT ( !IsValid( pRequest->persistentResourceDataLoadId ) );

	DefaultAllocator().Free( pRequest->pCachedObjectDataBuffer );
	pRequest->pCachedObjectDataBuffer = NULL;
	pRequest->cachedObjectDataBufferSize = 0;

	rspObject = pRequest->spObject;
	Asset* pObject = rspObject;
	if( pObject && ( pRequest->flags & LOAD_FLAG_ERROR ) )
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
	if( !m_startPreloadCounter )
	{
		return;
	}

	if( !m_preloadedCounter )
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

	return m_objects[ index ].objectPath;
}

const FilePath &LoosePackageLoader::GetAssetFileSystemPath( const AssetPath &path ) const
{
	size_t index = FindObjectByPath( path );

	HELIUM_ASSERT( index < m_objects.GetSize() );

	return m_objects[ index ].filePath;

}

int64_t LoosePackageLoader::GetAssetFileSystemTimestamp( const AssetPath &path ) const
{
	size_t index = FindObjectByPath( path );
	HELIUM_ASSERT( index < m_objects.GetSize() );
	if ( index < m_objects.GetSize() )
	{
		return m_objects[ index ].fileTimeStamp;
	}
	else
	{
		return 0;
	}
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

	AsyncLoader& rAsyncLoader = AsyncLoader::GetStaticInstance();

	// Walk through every load request
	for (size_t i = 0; i < m_fileReadRequests.GetSize();)
	{
		FileReadRequest &rRequest = m_fileReadRequests[i];
		HELIUM_ASSERT(rRequest.asyncLoadId);
		HELIUM_ASSERT(rRequest.pLoadBuffer);

		size_t bytes_read = 0;
		if (!rAsyncLoader.TrySyncRequest(rRequest.asyncLoadId, bytes_read))
		{
			// Havn't finished reading yet, move on to next entry
			++i;
			continue;
		}

		HELIUM_ASSERT(bytes_read == rRequest.expectedSize);
		if( IsInvalid( bytes_read ) )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "LoosePackageLoader: Failed to read the contents of async load request \"%d\".\n" ),
				rRequest.asyncLoadId );
		}
		else if( bytes_read != rRequest.expectedSize)
		{
			HELIUM_TRACE(
				TraceLevels::Warning,
				( TXT( "LoosePackageLoader: Attempted to read %" ) TPRIuSZ TXT( " bytes from package file \"%s\", " )
				TXT( "but only %" ) TPRIuSZ TXT( " bytes were read.\n" ) ),
				rRequest.expectedSize,
				bytes_read );
		}
		else
		{
			StaticMemoryStream archiveStream ( rRequest.pLoadBuffer, rRequest.expectedSize );
			Persist::ArchiveReaderJson archive ( &archiveStream );

			try
			{
				Reflect::ObjectPtr descriptor;
				archive.Start();
				archive.ReadNext( descriptor );

				if (descriptor.ReferencesObject())
				{
					ObjectDescriptor *object_descriptor = Reflect::SafeCast<ObjectDescriptor>(descriptor.Get());
					if (object_descriptor)
					{
						Name object_name;
						object_name.Set(object_descriptor->m_Name.c_str());
				
						// TODO: Consider changing tstring to String
						Name type_name;
						type_name.Set(object_descriptor->m_TypeName.c_str());

						SerializedObjectData* pObjectData = m_objects.New();
						HELIUM_ASSERT( pObjectData );
						HELIUM_VERIFY( pObjectData->objectPath.Set( object_name, false, m_packagePath ) );
						pObjectData->templatePath.Set(object_descriptor->m_TemplatePath.c_str());
						pObjectData->typeName = type_name;
						pObjectData->filePath = rRequest.filePath;
						pObjectData->fileTimeStamp = rRequest.fileTimestamp;
						pObjectData->bMetadataGood = true;
					}
					else
					{
						HELIUM_TRACE(
							TraceLevels::Error,
							TXT( "LoosePackageLoader: First object in asset file \"%s\" was not an ObjectDescriptor" ),
							rRequest.filePath.c_str(),
							bytes_read );
					}
				}
				else
				{
					HELIUM_TRACE(
						TraceLevels::Error,
						TXT( "LoosePackageLoader: Failed to read a valid object from asset file \"%s\"" ),
						rRequest.filePath.c_str(),
						bytes_read );
				}
			}
			catch ( Persist::Exception &pe )
			{
				HELIUM_TRACE(
					TraceLevels::Error,
					TXT( "LoosePackageLoader: Error processing metadata in asset file \"%s\": %s" ),
					rRequest.filePath.c_str(),
					pe.Get().c_str() );
			}
		}

		// We're finished with this load, so deallocate memory and get rid of the request
		DefaultAllocator().Free( rRequest.pLoadBuffer );
		rRequest.pLoadBuffer = NULL;
		SetInvalid(rRequest.asyncLoadId);
		m_fileReadRequests.RemoveSwap(i);
	}

	// Wait for the parent package to finish loading.
	AssetPtr spParentPackage;
	if( IsValid( m_parentPackageLoadId ) )
	{
		AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
		HELIUM_ASSERT( pAssetLoader );
		if( !pAssetLoader->TryFinishLoad( m_parentPackageLoadId, spParentPackage ) )
		{
			return;
		}

		SetInvalid( m_parentPackageLoadId );

		// Package loading should not fail.  If it does, this is a sign of a potentially serious issue.
		HELIUM_ASSERT( spParentPackage );
	}

	// Create the package object if it does not yet exist.
	Package* pPackage = m_spPackage;
	if( !pPackage )
	{
		HELIUM_ASSERT( spParentPackage ? !m_packagePath.GetParent().IsEmpty() : m_packagePath.GetParent().IsEmpty() );
		HELIUM_VERIFY( Asset::Create< Package >( m_spPackage, m_packagePath.GetName(), spParentPackage ) );
		pPackage = m_spPackage;
		HELIUM_ASSERT( pPackage );
		pPackage->SetLoader( this );
	}

	HELIUM_ASSERT( pPackage->GetLoader() == this );

	// Add all resource objects that exist in the package directory.
	DynamicArray< ResourceHandler* > resourceHandlers;
	ResourceHandler::GetAllResourceHandlers( resourceHandlers );
	size_t resourceHandlerCount = resourceHandlers.GetSize();

	FilePath packageDirectoryPath;

	if ( !FileLocations::GetDataDirectory( packageDirectoryPath ) )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "LoosePackageLoader::TickPreload(): Could not get data directory.\n" ) );
		return;
	}

	packageDirectoryPath += m_packagePath.ToFilePathString().GetData();
	packageDirectoryPath += TXT("/");

	DirectoryIterator packageDirectory( packageDirectoryPath );
	for( ; !packageDirectory.IsDone(); packageDirectory.Next() )
	{
		const DirectoryIteratorItem& item = packageDirectory.GetItem();

		if ( !item.m_Path.IsFile() )
		{
			continue;
		}

		// Make sure an object entry doesn't already exist for the file.
		String objectNameString( *item.m_Path );

		size_t pathSeparatorLocation = objectNameString.FindReverse( TXT( '/' ) );
		if( IsValid( pathSeparatorLocation ) )
		{
			objectNameString.Substring( objectNameString, pathSeparatorLocation + 1 );
		}

		Name objectName( objectNameString );
		size_t objectCount = m_objects.GetSize();
		size_t objectIndex;
		for( objectIndex = 0; objectIndex < objectCount; ++objectIndex )
		{
			SerializedObjectData& rObjectData = m_objects[ objectIndex ];
			if( rObjectData.objectPath.GetName() == objectName &&
				rObjectData.objectPath.GetParent() == m_packagePath )
			{
				break;
			}
		}

		if( objectIndex < objectCount )
		{
			continue;
		}

		// Check the extension to see if the file is supported by one of the resource handlers.
		ResourceHandler* pBestHandler = NULL;
		size_t bestHandlerExtensionLength = 0;

		for( size_t handlerIndex = 0; handlerIndex < resourceHandlerCount; ++handlerIndex )
		{
			ResourceHandler* pHandler = resourceHandlers[ handlerIndex ];
			HELIUM_ASSERT( pHandler );

			const tchar_t* const* ppExtensions;
			size_t extensionCount;
			pHandler->GetSourceExtensions( ppExtensions, extensionCount );
			HELIUM_ASSERT( ppExtensions || extensionCount == 0 );

			for( size_t extensionIndex = 0; extensionIndex < extensionCount; ++extensionIndex )
			{
				const tchar_t* pExtension = ppExtensions[ extensionIndex ];
				HELIUM_ASSERT( pExtension );

				size_t extensionLength = StringLength( pExtension );
				if( extensionLength > bestHandlerExtensionLength && objectNameString.EndsWith( pExtension ) )
				{
					pBestHandler = pHandler;
					bestHandlerExtensionLength = extensionLength;

					break;
				}
			}
		}

		if( pBestHandler )
		{
			// File extension matches a supported source asset type, so add it to the object list.
			const AssetType* pResourceType = pBestHandler->GetResourceType();
			HELIUM_ASSERT( pResourceType );

			HELIUM_TRACE(
				TraceLevels::Debug,
				( TXT( "LoosePackageLoader: Registered source asset file \"%s\" as as instance of resource " )
				TXT( "type \"%s\" in package \"%s\".\n" ) ),
				*objectNameString,
				*pResourceType->GetName(),
				*m_packagePath.ToString() );

			SerializedObjectData* pObjectData = m_objects.New();
			HELIUM_ASSERT( pObjectData );
			HELIUM_VERIFY( pObjectData->objectPath.Set( objectName, false, m_packagePath ) );
			pObjectData->typeName = pResourceType->GetName();
			pObjectData->templatePath.Clear();
			pObjectData->filePath.Clear();
			pObjectData->fileTimeStamp = 0;
			pObjectData->bMetadataGood = true;
		}
	}


	// Package preloading is now complete.
	pPackage->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );
	pPackage->ConditionalFinalizeLoad();

	AtomicExchangeRelease( m_preloadedCounter, 1 );
}

/// Update load processing of object load requests.
void LoosePackageLoader::TickLoadRequests()
{
	size_t loadRequestCount = m_loadRequests.GetSize();
	for( size_t loadRequestIndex = 0; loadRequestIndex < loadRequestCount; ++loadRequestIndex )
	{
		if( !m_loadRequests.IsElementValid( loadRequestIndex ) )
		{
			continue;
		}

		LoadRequest* pRequest = m_loadRequests[ loadRequestIndex ];
		HELIUM_ASSERT( pRequest );

		if( !( pRequest->flags & LOAD_FLAG_PROPERTY_PRELOADED ) )
		{
			if( !TickDeserialize( pRequest ) )
			{
				continue;
			}
		}

		//TODO: Investigate removing need to preload properties first. Probably need to have the
		//      restriction as TickPersistentResourcePreload assumes the object exists.. but this
		//      may not be the best place to put this 
		// We can probably remove these continues..
		if( !( pRequest->flags & LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED ) )
		{
			if( !TickPersistentResourcePreload( pRequest ) )
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
	for(size_t objectIndex = 0; objectIndex < objectCount; ++objectIndex )
	{
		const SerializedObjectData& rObjectData = m_objects[ objectIndex ];
		if( rObjectData.objectPath == path )
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
	SerializedObjectData& rObjectData = m_objects[ pRequest->index ];

	// Wait for the template and owner objects to load.
	AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
	HELIUM_ASSERT( pAssetLoader );

	if( !rObjectData.templatePath.IsEmpty() )
	{
		if( IsValid( pRequest->templateLoadId ) )
		{
			if( !pAssetLoader->TryFinishLoad( pRequest->templateLoadId, pRequest->spTemplate ) )
			{
				return false;
			}

			SetInvalid( pRequest->templateLoadId );
		}

		if( !pRequest->spTemplate )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "LoosePackageLoader: Failed to load template object for \"%s\".\n" ),
				*rObjectData.objectPath.ToString() );

			if( pObject )
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
	if( !ownerPath.IsEmpty() )
	{
		if( IsValid( pRequest->ownerLoadId ) )
		{
			if( !pAssetLoader->TryFinishLoad( pRequest->ownerLoadId, pRequest->spOwner ) )
			{
				return false;
			}

			SetInvalid( pRequest->ownerLoadId );
		}

		if( !pRequest->spOwner )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "LoosePackageLoader: Failed to load owner object for \"%s\".\n" ),
				*rObjectData.objectPath.ToString() );

			if( pObject )
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

	AsyncLoader& rAsyncLoader = AsyncLoader::GetStaticInstance();
	FilePath object_file_path = m_packageDirPath + *rObjectData.objectPath.GetName() + TXT( "." ) + Persist::ArchiveExtensions[ Persist::ArchiveTypes::Json ];

	bool load_properties_from_file = true;
	size_t object_file_size = 0;
	if ( !IsValid( pRequest->asyncFileLoadId ) )
	{
		if (!object_file_path.IsFile())
		{
			if (pType->GetClass()->IsType( Reflect::GetClass< Resource >() ))
			{
				HELIUM_TRACE(
					TraceLevels::Info,
					TXT( "LoosePackageLoader::TickDeserialize(): No object file found for resource \"%s\". Expected file location: \"%s\". This is normal for newly added resources.\n" ),
					*rObjectData.objectPath.ToString(),
					*object_file_path);

				// We will allow continuing to load using all default properties. This behavior is to support dropping resources into the 
				// data property and autogenerating objects from them.
				load_properties_from_file = false;
			}
			else
			{
				HELIUM_TRACE(
					TraceLevels::Warning,
					TXT( "LoosePackageLoader::TickDeserialize(): No object file found for object \"%s\". Expected file location: \"%s\"\n" ),
					*rObjectData.objectPath.ToString(),
					*object_file_path);
			}
		}
		else
		{
			Status status;
			status.Read( object_file_path.Get().c_str() );
			int64_t i64_object_file_size = status.m_Size;

			if( i64_object_file_size == -1 )
			{
				HELIUM_TRACE(
					TraceLevels::Warning,
					TXT( "LoosePackageLoader::TickDeserialize(): Could not get file size for object file of object \"%s\". Expected file location: \"%s\"\n" ),
					*rObjectData.objectPath.ToString(),
					*object_file_path );
			}
			else if( i64_object_file_size == 0 )
			{
				HELIUM_TRACE(
					TraceLevels::Warning,
					TXT( "LoosePackageLoader::TickDeserialize(): Object file \"%s\" for objct \"%s\" is empty.\n" ),
					*object_file_path,
					*rObjectData.objectPath.ToString() );
			}
			else if( static_cast< uint64_t >( i64_object_file_size ) > static_cast< uint64_t >( ~static_cast< size_t >( 0 ) ) )
			{
				HELIUM_TRACE(
					TraceLevels::Error,
					( TXT( "LoosePackageLoader::TickDeserialize(): Object file \"%s\" exceeds the maximum size supported by " )
					TXT( "the current platform (file size: %" ) TPRIu64 TXT( " bytes; max supported: %" ) TPRIuSZ
					TXT( " bytes).\n" ) ),
					object_file_path.c_str(),
					static_cast< uint64_t >( i64_object_file_size ),
					~static_cast< size_t >( 0 ) );
			}
			else
			{
				object_file_size = static_cast< size_t >(i64_object_file_size);
			}
		}
		
		if (!load_properties_from_file)
		{
			HELIUM_ASSERT(!object_file_size);
		}
		else if (!object_file_size)
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

			pRequest->asyncFileLoadId = rAsyncLoader.QueueRequest(
				pRequest->pAsyncFileLoadBuffer,
				String(object_file_path.c_str()),
				0,
				pRequest->asyncFileLoadBufferSize);
		}
		
	}
	
	size_t bytesRead = 0;
	if (load_properties_from_file)
	{
		HELIUM_ASSERT( IsValid( pRequest->asyncFileLoadId ) );

		if ( !rAsyncLoader.TrySyncRequest( pRequest->asyncFileLoadId, bytesRead ) )
		{
			return false;
		}
	}

	/////// POINT OF NO RETURN: We *will* return true after this point, and the object *will* be finished preloading,
	/////// for good or for bad.

	SetInvalid(pRequest->asyncFileLoadId);
	bool object_creation_failure = false;

	// If we already had an existing object, make sure the type and template match.
	if( pObject )
	{
		const AssetType* pExistingType = pObject->GetAssetType();
		HELIUM_ASSERT( pExistingType );
		if( pExistingType != pType )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				( TXT( "LoosePackageLoader: Cannot load \"%s\" using the existing object as the types do not match " )
				TXT( "(existing type: \"%s\"; serialized type: \"%s\".\n" ) ),
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
		// Create the object.
		bool bCreateResult = Asset::CreateObject(
			pRequest->spObject,
			pType,
			rObjectData.objectPath.GetName(),
			pOwner,
			pTemplate );
		if( !bCreateResult )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "LoosePackageLoader: Failed to create \"%s\" during loading.\n" ),
				*rObjectData.objectPath.ToString() );
				
			object_creation_failure = true;
		}

		pObject = pRequest->spObject;
		HELIUM_ASSERT( pObject );
	}

	if (load_properties_from_file && !object_creation_failure)
	{
		// Sanity checks for file load, then success path
		HELIUM_ASSERT( bytesRead == pRequest->asyncFileLoadBufferSize );
		if( IsInvalid( bytesRead ) )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "LoosePackageLoader: Failed to read the contents of object file \"%s\" in async load request \"%d\".\n" ),
				object_file_path.c_str(),
				pRequest->asyncFileLoadId );
		}
		else if( bytesRead != pRequest->asyncFileLoadBufferSize )
		{
			HELIUM_TRACE(
				TraceLevels::Warning,
				( TXT( "LoosePackageLoader: Attempted to read %" ) TPRIuSZ TXT( " bytes from object file \"%s\", " )
				TXT( "but only %" ) TPRIuSZ TXT( " bytes were read.\n" ) ),
				pRequest->asyncFileLoadBufferSize,
				object_file_path.c_str(),
				bytesRead );
		}
		else
		{
			StaticMemoryStream archiveStream ( pRequest->pAsyncFileLoadBuffer, pRequest->asyncFileLoadBufferSize );

			HELIUM_TRACE(
				TraceLevels::Info,
				TXT( "LoosePackageLoader: Reading %s. pResolver = %x"), 
				object_file_path.c_str(),
				pRequest->pResolver);

			Persist::ArchiveReaderJson archive ( &archiveStream, pRequest->pResolver );

			Reflect::ObjectPtr descriptor;
			archive.Start();
			archive.ReadNext( descriptor );

#pragma TODO( "We can add asserts here that the descriptor in this file match our expectations" )
			// Now that we have an object instance with the proper type, name, template, etc. we can finally read in properties
			if (!object_creation_failure)
			{
				//Asset *old_object_ptr = pObject;
				Reflect::ObjectPtr object_ptr;
				object_ptr.Set(pRequest->spObject.Get());
				archive.ReadNext( object_ptr );

				HELIUM_ASSERT(object_ptr.Get());
				HELIUM_ASSERT(object_ptr.Get() == pRequest->spObject.Get());

				pObject = Reflect::AssertCast<Asset>(object_ptr.Get());
				pRequest->spObject.Set(pObject);
			}
		}
	}

	if (load_properties_from_file)
	{
		DefaultAllocator().Free(pRequest->pAsyncFileLoadBuffer);
		pRequest->pAsyncFileLoadBuffer = NULL;
		pRequest->asyncFileLoadBufferSize = 0;
	}

	pRequest->flags |= LOAD_FLAG_PROPERTY_PRELOADED;

	if( object_creation_failure )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "LoosePackageLoader: Deserialization of object \"%s\" failed.\n" ),
			*rObjectData.objectPath.ToString() );
		
#pragma TODO("We used to clear link indices here... but that should be handled by the Resolver now -gorlak")
		pObject->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );
		pObject->ConditionalFinalizeLoad();

		pRequest->flags |= LOAD_FLAG_ERROR;

		return true;
	}

	// If the object is a resource (not including the default template object for resource types), attempt to begin
	// loading any existing persistent resource data stored in the object cache.
	if( !pObject->IsDefaultTemplate() )
	{
		Resource* pResource = Reflect::SafeCast< Resource >( pObject );
		if( pResource )
		{
			Name objectCacheName = Name( HELIUM_ASSET_CACHE_NAME );
			CacheManager& rCacheManager = CacheManager::GetStaticInstance();

			Cache* pCache = rCacheManager.GetCache( objectCacheName );
			HELIUM_ASSERT( pCache );
			pCache->EnforceTocLoad();

			const Cache::Entry* pEntry = pCache->FindEntry( rObjectData.objectPath, 0 );
			if( pEntry && pEntry->size != 0 )
			{
				HELIUM_ASSERT( IsInvalid( pRequest->persistentResourceDataLoadId ) );
				HELIUM_ASSERT( !pRequest->pCachedObjectDataBuffer );

				pRequest->pCachedObjectDataBuffer =
					static_cast< uint8_t* >( DefaultAllocator().Allocate( pEntry->size ) );
				HELIUM_ASSERT( pRequest->pCachedObjectDataBuffer );
				pRequest->cachedObjectDataBufferSize = pEntry->size;

				AsyncLoader& rAsyncLoader = AsyncLoader::GetStaticInstance();
				pRequest->persistentResourceDataLoadId = rAsyncLoader.QueueRequest(
					pRequest->pCachedObjectDataBuffer,
					pCache->GetCacheFileName(),
					pEntry->offset,
					pEntry->size );
				HELIUM_ASSERT( IsValid( pRequest->persistentResourceDataLoadId ) );
			}
		}
	}

	if( IsInvalid( pRequest->persistentResourceDataLoadId ) )
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

	// Wait for the cached data load to complete.
	AsyncLoader& rAsyncLoader = AsyncLoader::GetStaticInstance();

	size_t bytesRead = 0;
	HELIUM_ASSERT( IsValid( pRequest->persistentResourceDataLoadId ) );
	if( !rAsyncLoader.TrySyncRequest( pRequest->persistentResourceDataLoadId, bytesRead ) )
	{
		return false;
	}

	SetInvalid( pRequest->persistentResourceDataLoadId );

	if( bytesRead != pRequest->cachedObjectDataBufferSize )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			( TXT( "LoosePackageLoader: Requested load of %" ) TPRIu32 TXT( " bytes from cached object data for " )
			TXT( "\"%s\", but only %" ) TPRIuSZ TXT( " bytes were read.\n" ) ),
			pRequest->cachedObjectDataBufferSize,
			*pResource->GetPath().ToString(),
			bytesRead );
	}

	// Make sure we read enough bytes to cover the object property data size.
	if( bytesRead < sizeof( uint32_t ) )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			( TXT( "LoosePackageLoader: Not enough bytes read of cached object data \"%s\" from which to parse the " )
			TXT( "property stream size.\n" ) ),
			*pResource->GetPath().ToString() );

		pRequest->flags |= LOAD_FLAG_ERROR;
	}
	else
	{
		// Skip over the object property data.
		uint8_t* pCachedObjectData = pRequest->pCachedObjectDataBuffer;
		HELIUM_ASSERT( pCachedObjectData );

		uint32_t propertyDataSize = *reinterpret_cast< uint32_t* >( pCachedObjectData );

		size_t byteSkipCount = sizeof( propertyDataSize ) + propertyDataSize;
		if( byteSkipCount > bytesRead )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				( TXT( "LoosePackageLoader: Cached persistent resource data for \"%s\" extends past the end of the " )
				TXT( "cached data stream.\n" ) ),
				*pResource->GetPath().ToString() );

			pRequest->flags |= LOAD_FLAG_ERROR;
		}
		else
		{
			pCachedObjectData += byteSkipCount;
			size_t bytesRemaining = bytesRead - byteSkipCount;

			// Make sure we have enough bytes at the end for the resource sub-data count.
			if( bytesRemaining < sizeof( uint32_t ) )
			{
				HELIUM_TRACE(
					TraceLevels::Error,
					( TXT( "LoosePackageLoader: Not enough space is reserved in the cached persistent resource " )
					TXT( "data stream for \"%s\" for the resource sub-data count.\n" ) ),
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
				persistent_data = Cache::ReadCacheObjectFromBuffer(pCachedObjectData, /*sizeof( uint32_t )*/ 0, bytesRemaining, pRequest->pResolver);

				if (!pResource->LoadPersistentResourceObject(persistent_data))
				{
					HELIUM_TRACE(
						TraceLevels::Error,
						( TXT( "LoosePackageLoader: Failed to load persistent resource object for \"%s\".\n" ) ),
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
