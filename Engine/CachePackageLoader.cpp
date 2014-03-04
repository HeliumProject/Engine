#include "EnginePch.h"
#include "Engine/CachePackageLoader.h"

#include "Engine/Asset.h"
#include "Engine/AssetLoader.h"
#include "Engine/AsyncLoader.h"
#include "Engine/CacheManager.h"
#include "Engine/Resource.h"

using namespace Helium;

/// Constructor.
CachePackageLoader::CachePackageLoader()
: m_pCache( NULL )
, m_bFinishedCacheTocLoad( false )
, m_loadRequestPool( LOAD_REQUEST_POOL_BLOCK_SIZE )
{
}

/// Destructor.
CachePackageLoader::~CachePackageLoader()
{
	Shutdown();
}

/// Initialize this package loader for loading from the specified cache files.
///
/// @param[in] cacheName  Name of the cache to use.
///
/// @return  True if initialization was successful, false if not.
///
/// @see Shutdown()
bool CachePackageLoader::Initialize( Name cacheName )
{
	HELIUM_ASSERT( !cacheName.IsEmpty() );

	Shutdown();

	// Acquire the cache.
	CacheManager& rCacheManager = CacheManager::GetStaticInstance();

	m_pCache = rCacheManager.GetCache( cacheName );
	if( !m_pCache )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "CachePackageLoader::Initialize(): Failed to initialize cache \"%s\".\n" ),
			*cacheName );

		return false;
	}

	return true;
}

/// Shut down this package loader.
///
/// @see Initialize()
void CachePackageLoader::Shutdown()
{
	DefaultAllocator allocator;

	AsyncLoader& rAsyncLoader = AsyncLoader::GetStaticInstance();

	size_t loadRequestCount = m_loadRequests.GetSize();
	for( size_t requestIndex = 0; requestIndex < loadRequestCount; ++requestIndex )
	{
		if( m_loadRequests.IsElementValid( requestIndex ) )
		{
			LoadRequest* pRequest = m_loadRequests[ requestIndex ];
			HELIUM_ASSERT( pRequest );
			if( IsValid( pRequest->asyncLoadId ) )
			{
				rAsyncLoader.SyncRequest( pRequest->asyncLoadId );
			}

			allocator.Free( pRequest->pAsyncLoadBuffer );

			m_loadRequestPool.Release( pRequest );
		}
	}

	m_loadRequests.Clear();

	m_pCache = NULL;
	m_bFinishedCacheTocLoad = false;
}

/// Begin asynchronous loading of the cache table of contents.
///
/// This must be called after calling Initialize() in order to begin using an existing cache.
///
/// @return  True if loading was started successfully, false if not.
///
/// @see TryFinishLoadToc()
bool CachePackageLoader::BeginPreload()
{
	HELIUM_ASSERT( m_pCache );

	bool bResult = m_pCache->IsTocLoaded();
	m_bFinishedCacheTocLoad = bResult;

	if( !bResult )
	{
		bResult = m_pCache->BeginLoadToc();
	}

	return bResult;
}

/// @copydoc PackageLoader::TryFinishPreload()
bool CachePackageLoader::TryFinishPreload()
{
	HELIUM_ASSERT( m_pCache );

	bool bResult = m_bFinishedCacheTocLoad;
	if( !bResult )
	{
		bResult = ( m_pCache->IsTocLoaded() || m_pCache->TryFinishLoadToc() );
		m_bFinishedCacheTocLoad = bResult;
	}

	return bResult;
}

/// @copydoc PackageLoader::BeginLoadObject()
size_t CachePackageLoader::BeginLoadObject( AssetPath path, Reflect::ObjectResolver *pResolver, bool forceReload )
{
	HELIUM_ASSERT( m_pCache );
	HELIUM_ASSERT( !forceReload ); // Not supported

	// Don't load packages from the cache, but instead create them dynamically.
	if( path.IsPackage() )
	{
		HELIUM_TRACE(
			TraceLevels::Debug,
			TXT( "CachePackageLoader::BeginLoadObject(): \"%s\" is a package, resolving immediately.\n" ),
			*path.ToString() );

		LoadRequest* pRequest = m_loadRequestPool.Allocate();
		HELIUM_ASSERT( pRequest );
		pRequest->pEntry = NULL;
		pRequest->pResolver = pResolver;

		ResolvePackage( pRequest->spObject, path );
		HELIUM_ASSERT( pRequest->spObject );

		SetInvalid( pRequest->asyncLoadId );
		pRequest->pAsyncLoadBuffer = NULL;
		pRequest->pPropertyDataBegin = NULL;
		pRequest->pPropertyDataEnd = NULL;
		pRequest->pPersistentResourceDataBegin = NULL;
		pRequest->pPersistentResourceDataEnd = NULL;
		SetInvalid( pRequest->ownerLoadIndex );
		HELIUM_ASSERT( !pRequest->spOwner );
		pRequest->forceReload = forceReload;

		pRequest->flags = LOAD_FLAG_PRELOADED;

		size_t requestId = m_loadRequests.Add( pRequest );

		return requestId;
	}

	const Cache::Entry* pEntry = m_pCache->FindEntry( path, 0 );
	if( !pEntry )
	{
		HELIUM_TRACE(
			TraceLevels::Debug,
			( TXT( "CachePackageLoader::BeginLoadObject(): \"%s\" is not cached in this package.  No load " )
			TXT( "request added.\n" ) ),
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
		HELIUM_ASSERT( pRequest->pEntry != pEntry );
		if( pRequest->pEntry == pEntry )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				( TXT( "CachePackageLoader::BeginLoadObject(): Duplicate load request of \"%s\".  No load " )
				TXT( "request added.\n" ) ),
				*path.ToString() );

			return Invalid< size_t >();
		}
	}
#endif

	LoadRequest* pRequest = m_loadRequestPool.Allocate();
	HELIUM_ASSERT( pRequest );
	pRequest->pEntry = pEntry;
	pRequest->pResolver = pResolver;
	HELIUM_ASSERT( !pRequest->spObject );
	SetInvalid( pRequest->asyncLoadId );
	pRequest->pAsyncLoadBuffer = NULL;
	pRequest->pPropertyDataBegin = NULL;
	pRequest->pPropertyDataEnd = NULL;
	pRequest->pPersistentResourceDataBegin = NULL;
	pRequest->pPersistentResourceDataEnd = NULL;
	SetInvalid( pRequest->ownerLoadIndex );
	HELIUM_ASSERT( !pRequest->spOwner );
	pRequest->forceReload = forceReload;

	pRequest->flags = 0;

	// If a fully-loaded object already exists with the same name, do not attempt to re-load the object (just mark
	// the request as complete).
	pRequest->spObject = Asset::FindObject( pEntry->path );

	Asset* pObject = pRequest->spObject;
	if( pObject && pObject->IsFullyLoaded() )
	{
		HELIUM_TRACE(
			TraceLevels::Debug,
			( TXT( "CachePackageLoader::BeginLoadObject(): \"%s\" is already fully loaded.  Bypassing load " )
			TXT( "process.\n" ) ),
			*path.ToString() );

		pRequest->flags = LOAD_FLAG_PRELOADED;
	}
	else
	{
		HELIUM_ASSERT( !pObject || !pObject->GetAnyFlagSet( Asset::FLAG_LOADED | Asset::FLAG_LINKED ) );

		HELIUM_TRACE(
			TraceLevels::Debug,
			TXT( "CachePackageLoader::BeginLoadObject(): Issuing async load of property data for \"%s\".\n" ),
			*path.ToString() );

		size_t entrySize = pEntry->size;
		pRequest->pAsyncLoadBuffer = static_cast< uint8_t* >( DefaultAllocator().Allocate( entrySize ) );
		HELIUM_ASSERT( pRequest->pAsyncLoadBuffer );

		AsyncLoader& rLoader = AsyncLoader::GetStaticInstance();
		pRequest->asyncLoadId = rLoader.QueueRequest(
			pRequest->pAsyncLoadBuffer,
			m_pCache->GetCacheFileName(),
			pEntry->offset,
			entrySize );
		HELIUM_ASSERT( IsValid( pRequest->asyncLoadId ) );
	}

	size_t requestId = m_loadRequests.Add( pRequest );

	HELIUM_TRACE(
		TraceLevels::Debug,
		( TXT( "CachePackageLoader::BeginLoadObject(): Load request for \"%s\" added (ID: %" ) PRIuSZ
		TXT( ").\n" ) ),
		*path.ToString(),
		requestId );

	return requestId;
}

/// @copydoc PackageLoader::TryFinishLoadObject()
bool CachePackageLoader::TryFinishLoadObject( size_t requestId, AssetPtr& rspObject )
{
	HELIUM_ASSERT( requestId < m_loadRequests.GetSize() );
	HELIUM_ASSERT( m_loadRequests.IsElementValid( requestId ) );

	LoadRequest* pRequest = m_loadRequests[ requestId ];
	HELIUM_ASSERT( pRequest );
	if( !( pRequest->flags & LOAD_FLAG_PRELOADED ) )
	{
		return false;
	}

	// Sync on template and owner dependencies.
	AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
	HELIUM_ASSERT( pAssetLoader );

	if( IsValid( pRequest->ownerLoadIndex ) )
	{
		size_t linkLoadId = pRequest->ownerLoadIndex;
		if( IsValid( linkLoadId ) && !pAssetLoader->TryFinishLoad( linkLoadId, pRequest->spOwner ) )
		{
			return false;
		}

		SetInvalid( pRequest->ownerLoadIndex );
	}

	rspObject = pRequest->spObject;
	Asset* pObject = rspObject;
	if( pObject && ( pRequest->flags & LOAD_FLAG_ERROR ) )
	{
		pObject->SetFlags( Asset::FLAG_BROKEN );
	}

	if ( pObject->IsPackage() )
	{
		Package *pPackage = Reflect::AssertCast<Package>( pObject );
		pPackage->SetLoader( this );
	}

	pRequest->spObject.Release();

	HELIUM_ASSERT( IsInvalid( pRequest->asyncLoadId ) );
	HELIUM_ASSERT( !pRequest->pAsyncLoadBuffer );

	//pRequest->spTemplate.Release();
	pRequest->spOwner.Release();

	HELIUM_ASSERT( IsInvalid( pRequest->ownerLoadIndex ) );
	//HELIUM_ASSERT( IsInvalid( pRequest->templateLoadIndex ) );

	HELIUM_ASSERT( pObject || pRequest->pEntry );
	HELIUM_TRACE(
		TraceLevels::Debug,
		( TXT( "CachePackageLoader::TryFinishLoadObject(): Load request for \"%s\" (ID: %" ) PRIuSZ TXT( ") " )
		TXT( "synced.\n" ) ),
		*( pObject ? pObject->GetPath() : pRequest->pEntry->path ).ToString(),
		requestId );

	m_loadRequests.Remove( requestId );
	m_loadRequestPool.Release( pRequest );

	return true;
}

/// Update this package loader.
void CachePackageLoader::Tick()
{
	// Process pending load requests.
	size_t loadRequestSize = m_loadRequests.GetSize();
	for( size_t loadRequestIndex = 0; loadRequestIndex < loadRequestSize; ++loadRequestIndex )
	{
		if( !m_loadRequests.IsElementValid( loadRequestIndex ) )
		{
			continue;
		}

		LoadRequest* pRequest = m_loadRequests[ loadRequestIndex ];
		HELIUM_ASSERT( pRequest );

		if( !( pRequest->flags & LOAD_FLAG_PRELOADED ) )
		{
			if( IsValid( pRequest->asyncLoadId ) )
			{
				if( !TickCacheLoad( pRequest ) )
				{
					continue;
				}
			}

			// Preloaded flag may be set if the cache load step failed.
			if( !( pRequest->flags & LOAD_FLAG_PRELOADED ) )
			{
				if( !TickDeserialize( pRequest ) )
				{
					continue;
				}
			}
		}

		HELIUM_ASSERT( IsInvalid( pRequest->asyncLoadId ) );
		HELIUM_ASSERT( pRequest->pAsyncLoadBuffer == NULL );
	}
}

/// @copydoc PackageLoader::GetObjectCount()
size_t CachePackageLoader::GetObjectCount() const
{
	HELIUM_ASSERT( m_pCache );

	return m_pCache->GetEntryCount();
}

/// @copydoc PackageLoader::GetObjectPath()
AssetPath CachePackageLoader::GetAssetPath( size_t index ) const
{
	HELIUM_ASSERT( m_pCache );
	HELIUM_ASSERT( index < m_pCache->GetEntryCount() );

	const Cache::Entry& rEntry = m_pCache->GetEntry( static_cast< uint32_t >( index ) );

	return rEntry.path;
}

/// Tick the async loading of binary serialized data from the object cache for the given load request.
///
/// @param[in] pRequest  Load request.
///
/// @return  True if the cache load process has completed, false if it still requires processing.
bool CachePackageLoader::TickCacheLoad( LoadRequest* pRequest )
{
	HELIUM_ASSERT( pRequest );
	HELIUM_ASSERT( !( pRequest->flags & LOAD_FLAG_PRELOADED ) );

	AsyncLoader& rAsyncLoader = AsyncLoader::GetStaticInstance();

	size_t bytesRead = 0;
	if( !rAsyncLoader.TrySyncRequest( pRequest->asyncLoadId, bytesRead ) )
	{
		return false;
	}

	SetInvalid( pRequest->asyncLoadId );

	if( bytesRead == 0 || IsInvalid( bytesRead ) )
	{
		HELIUM_ASSERT( pRequest->pEntry );

		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "CachePackageLoader: Failed to read cache data for object \"%s\".\n" ),
			*pRequest->pEntry->path.ToString() );
	}
	else
	{
		uint8_t* pBufferEnd = pRequest->pAsyncLoadBuffer + bytesRead;
		pRequest->pPropertyDataEnd = pBufferEnd;
		pRequest->pPersistentResourceDataEnd = pBufferEnd;

		if( ReadCacheData( pRequest ) )
		{
			return true;
		}
	}

	// An error occurred attempting to load the property data, so mark any existing object as fully loaded (nothing
	// else will be done with the object itself from here on out).
	DefaultAllocator().Free( pRequest->pAsyncLoadBuffer );
	pRequest->pAsyncLoadBuffer = NULL;

	Asset* pObject = pRequest->spObject;
	if( pObject )
	{
		pObject->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );
		pObject->ConditionalFinalizeLoad();
	}

	pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;

	return true;
}

/// Tick the object deserialization process for the given object load request.
///
/// @param[in] pRequest  Load request.
///
/// @return  True if the deserialization process has completed, false if it still needs time to process.
bool CachePackageLoader::TickDeserialize( LoadRequest* pRequest )
{
	HELIUM_ASSERT( pRequest );
	HELIUM_ASSERT( !( pRequest->flags & LOAD_FLAG_PRELOADED ) );

	HELIUM_ASSERT( !pRequest->spObject );

	const Cache::Entry* pCacheEntry = pRequest->pEntry;
	HELIUM_ASSERT( pCacheEntry );

	// Wait for the template and owner objects to load.
	AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
	HELIUM_ASSERT( pAssetLoader );

	if( IsValid( pRequest->ownerLoadIndex ) )
	{
		size_t ownerLoadId = pRequest->ownerLoadIndex;
		if( IsValid( ownerLoadId ) && !pAssetLoader->TryFinishLoad( ownerLoadId, pRequest->spOwner ) )
		{
			return false;
		}

		SetInvalid( pRequest->ownerLoadIndex );

		if( !pRequest->spOwner )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "CachePackageLoader: Failed to load owner object for \"%s\".\n" ),
				*pCacheEntry->path.ToString() );

			DefaultAllocator().Free( pRequest->pAsyncLoadBuffer );
			pRequest->pAsyncLoadBuffer = NULL;

			pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;

			return true;
		}
	}

	Asset* pOwner = pRequest->spOwner;

	HELIUM_ASSERT( !pOwner || pOwner->IsFullyLoaded() );
	
	Reflect::ObjectPtr cached_object = Cache::ReadCacheObjectFromBuffer(
		pRequest->pPropertyDataBegin, 
		0, 
		pRequest->pPropertyDataEnd - pRequest->pPropertyDataBegin, 
		pRequest->pResolver);

	AssetPtr assetPtr = Reflect::AssertCast<Asset>(cached_object);

	Asset::RenameParameters params;
	params.instanceIndex = -1;
	params.name = pCacheEntry->path.GetName();
	params.spOwner = pRequest->spOwner;
	assetPtr->Rename(params);

	pRequest->spObject = assetPtr;

	Asset *pObject = assetPtr;
	HELIUM_ASSERT( pObject );
		
	if (!cached_object.ReferencesObject())
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "CachePackageLoader: Failed to deserialize object \"%s\".\n" ),
			*pCacheEntry->path.ToString() );

		pObject->SetFlags( Asset::FLAG_LINKED );
		pObject->ConditionalFinalizeLoad();

		pRequest->flags |= LOAD_FLAG_ERROR;
	}
	else
	{
		//cached_object->CopyTo(pObject);
				
		if( !pObject->IsDefaultTemplate() )
		{
			// Load persistent resource data.
			Resource* pResource = Reflect::SafeCast< Resource >( pObject );
			if( pResource )
			{
				Reflect::ObjectPtr cached_prd = Cache::ReadCacheObjectFromBuffer(
					pRequest->pPersistentResourceDataBegin, 
					0, 
					(pRequest->pPersistentResourceDataEnd - pRequest->pPersistentResourceDataBegin),
					pRequest->pResolver);

				if (!cached_prd.ReferencesObject())
				{
					HELIUM_TRACE(
						TraceLevels::Error,
						( TXT( "CachePackageLoader: Failed to deserialize persistent resource " )
						TXT( "data for \"%s\".\n" ) ),
						*pCacheEntry->path.ToString() );
				}
				else
				{
					pResource->LoadPersistentResourceObject(cached_prd);
				}
			}
		}
	}

	DefaultAllocator().Free( pRequest->pAsyncLoadBuffer );
	pRequest->pAsyncLoadBuffer = NULL;

	pObject->SetFlags( Asset::FLAG_PRELOADED );

	pRequest->flags |= LOAD_FLAG_PRELOADED;

	// Asset is now preloaded.
	return true;
}

/// Recursive function for resolving a package request.
///
/// @param[out] rspPackage   Resolved package.
/// @param[in]  packagePath  Package object path.
void CachePackageLoader::ResolvePackage( AssetPtr& rspPackage, AssetPath packagePath )
{
	HELIUM_ASSERT( !packagePath.IsEmpty() );

	rspPackage = Asset::FindObject( packagePath );
	if( !rspPackage )
	{
		AssetPtr spParent;
		AssetPath parentPath = packagePath.GetParent();
		if( !parentPath.IsEmpty() )
		{
			ResolvePackage( spParent, parentPath );
			HELIUM_ASSERT( spParent );
		}

		HELIUM_VERIFY( Asset::CreateObject(
			rspPackage,
			Package::GetStaticType(),
			packagePath.GetName(),
			spParent ) );
		HELIUM_ASSERT( rspPackage );
		HELIUM_ASSERT( rspPackage->IsA( Package::GetStaticType()->GetMetaClass() ) );
	}

	rspPackage->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED | Asset::FLAG_LOADED );
}

/// Deserialize the link tables for an object load.
///
/// @param[in] pRequest  Load request data.
bool CachePackageLoader::ReadCacheData( LoadRequest* pRequest )
{
	HELIUM_ASSERT( pRequest );

	uint8_t* pBufferCurrent = pRequest->pAsyncLoadBuffer;
	uint8_t* pPropertyDataEnd = pRequest->pPropertyDataEnd;
	HELIUM_ASSERT( pBufferCurrent );
	HELIUM_ASSERT( pPropertyDataEnd );
	HELIUM_ASSERT( pBufferCurrent <= pPropertyDataEnd );

	// We know the owner's path immediately just by looking at the path we're currently loading
	AssetPath parentPath = pRequest->pEntry->path.GetParent();
	pRequest->ownerLoadIndex = AssetLoader::GetStaticInstance()->BeginLoadObject( parentPath );

	if (IsInvalid<size_t>(pRequest->ownerLoadIndex))
	{
		HELIUM_TRACE(
			TraceLevels::Debug,
			TXT( "LoosePackageLoader: Failed to begin loading owning asset '%s' for '%s'.\n" ),
			*parentPath.ToString(),
			*pRequest->pEntry->path.ToString());
	}

	// Scan the property stream.. another null terminated string with character count first
	uint32_t propertyStreamSize = 0;
	if( pBufferCurrent + sizeof( propertyStreamSize ) > pPropertyDataEnd )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
			*pRequest->pEntry->path.ToString() );

		return false;
	}

	MemoryCopy( &propertyStreamSize, pBufferCurrent, sizeof( propertyStreamSize ) );
	pBufferCurrent += sizeof( propertyStreamSize );

	if( propertyStreamSize > static_cast< size_t >( pPropertyDataEnd - pBufferCurrent ) )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			( TXT( "CachePackageLoader: Property stream size (%" ) PRIu32 TXT( " bytes) for \"%s\" exceeds the " )
			TXT( "amount of data cached.  Value will be clamped.\n" ) ),
			propertyStreamSize,
			*pRequest->pEntry->path.ToString() );

		propertyStreamSize = static_cast< uint32_t >( pPropertyDataEnd - pBufferCurrent );
	}

	pRequest->pPropertyDataBegin = pBufferCurrent;
	pPropertyDataEnd = pBufferCurrent + propertyStreamSize;
	pRequest->pPropertyDataEnd = pPropertyDataEnd;

	// Verify that it's null terminated
	HELIUM_ASSERT( pRequest->pPropertyDataEnd[-1] == 0);

	// Adjust the end of the persistent resource data stream to account for the resource sub-data count padded on
	// the end (note that non-resources will not have this padding). The count is at the END of the buffer, and
	// if we have enough room for a size left, then assume persistant resource starts immediately after the property
	// data ends, and ends at the count.
	if( pRequest->pPersistentResourceDataEnd - pPropertyDataEnd >= sizeof( uint32_t ) )
	{
		pRequest->pPersistentResourceDataBegin = pPropertyDataEnd;
		pRequest->pPersistentResourceDataEnd -= sizeof( uint32_t );

		HELIUM_ASSERT( pRequest->pPersistentResourceDataEnd[-1] == 0 );
	}
	else
	{
		pRequest->pPersistentResourceDataBegin = pPropertyDataEnd;
		pRequest->pPersistentResourceDataEnd = pPropertyDataEnd;
	}

	return true;
}
