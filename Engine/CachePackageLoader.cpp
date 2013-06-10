//----------------------------------------------------------------------------------------------------------------------
// CachePackageLoader.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

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
size_t CachePackageLoader::BeginLoadObject( AssetPath path, Reflect::ObjectResolver *pResolver )
{
	HELIUM_ASSERT( m_pCache );

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
		pRequest->pSerializedData = NULL;
		pRequest->pPropertyStreamEnd = NULL;
		pRequest->pPersistentResourceStreamEnd = NULL;
		HELIUM_ASSERT( pRequest->typeLinkTable.IsEmpty() );
		HELIUM_ASSERT( pRequest->objectLinkTable.IsEmpty() );
		HELIUM_ASSERT( !pRequest->spType );
		HELIUM_ASSERT( !pRequest->spTemplate );
		HELIUM_ASSERT( !pRequest->spOwner );
		SetInvalid( pRequest->templateLinkIndex );
		SetInvalid( pRequest->ownerLinkIndex );

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
	HELIUM_ASSERT( !pRequest->spObject );
	SetInvalid( pRequest->asyncLoadId );
	pRequest->pAsyncLoadBuffer = NULL;
	pRequest->pSerializedData = NULL;
	pRequest->pPropertyStreamEnd = NULL;
	pRequest->pPersistentResourceStreamEnd = NULL;
	HELIUM_ASSERT( pRequest->typeLinkTable.IsEmpty() );
	HELIUM_ASSERT( pRequest->objectLinkTable.IsEmpty() );
	HELIUM_ASSERT( !pRequest->spType );
	HELIUM_ASSERT( !pRequest->spTemplate );
	HELIUM_ASSERT( !pRequest->spOwner );
	SetInvalid( pRequest->templateLinkIndex );
	SetInvalid( pRequest->ownerLinkIndex );

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

	DynamicArray< size_t >& rInternalLinkTable = pRequest->objectLinkTable;

	if( IsValid( pRequest->templateLinkIndex ) )
	{
		size_t linkLoadId = rInternalLinkTable[ pRequest->templateLinkIndex ];
		if( IsValid( linkLoadId ) && !pAssetLoader->TryFinishLoad( linkLoadId, pRequest->spTemplate ) )
		{
			return false;
		}

		SetInvalid( pRequest->templateLinkIndex );
	}

	if( IsValid( pRequest->ownerLinkIndex ) )
	{
		size_t linkLoadId = rInternalLinkTable[ pRequest->ownerLinkIndex ];
		if( IsValid( linkLoadId ) && !pAssetLoader->TryFinishLoad( linkLoadId, pRequest->spOwner ) )
		{
			return false;
		}

		SetInvalid( pRequest->ownerLinkIndex );
	}

	rspObject = pRequest->spObject;
	Asset* pObject = rspObject;
	if( pObject && ( pRequest->flags & LOAD_FLAG_ERROR ) )
	{
		pObject->SetFlags( Asset::FLAG_BROKEN );
	}

	pRequest->spObject.Release();

	HELIUM_ASSERT( IsInvalid( pRequest->asyncLoadId ) );
	HELIUM_ASSERT( !pRequest->pAsyncLoadBuffer );

	pRequest->spType.Release();
	pRequest->spTemplate.Release();
	pRequest->spOwner.Release();
	pRequest->typeLinkTable.Resize( 0 );

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
		pRequest->pPropertyStreamEnd = pBufferEnd;
		pRequest->pPersistentResourceStreamEnd = pBufferEnd;

		if( DeserializeLinkTables( pRequest ) )
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

	Asset* pObject = pRequest->spObject;

	const Cache::Entry* pCacheEntry = pRequest->pEntry;
	HELIUM_ASSERT( pCacheEntry );

	// Wait for the template and owner objects to load.
	AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
	HELIUM_ASSERT( pAssetLoader );

	if( IsValid( pRequest->templateLinkIndex ) )
	{
		HELIUM_ASSERT( pRequest->templateLinkIndex < pRequest->objectLinkTable.GetSize() );
		size_t templateLoadId = pRequest->objectLinkTable[ pRequest->templateLinkIndex ];
		if( IsValid( templateLoadId ) && !pAssetLoader->TryFinishLoad( templateLoadId, pRequest->spTemplate ) )
		{
			return false;
		}

		SetInvalid( pRequest->templateLinkIndex );

		if( !pRequest->spTemplate )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "CachePackageLoader: Failed to load template object for \"%s\".\n" ),
				*pCacheEntry->path.ToString() );

			if( pObject )
			{
				pObject->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );
				pObject->ConditionalFinalizeLoad();
			}

			DefaultAllocator().Free( pRequest->pAsyncLoadBuffer );
			pRequest->pAsyncLoadBuffer = NULL;

			pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;

			return true;
		}
	}

	Asset* pTemplate = pRequest->spTemplate;

	if( IsValid( pRequest->ownerLinkIndex ) )
	{
		HELIUM_ASSERT( pRequest->ownerLinkIndex < pRequest->objectLinkTable.GetSize() );
		size_t ownerLoadId = pRequest->objectLinkTable[ pRequest->ownerLinkIndex ];
		if( IsValid( ownerLoadId ) && !pAssetLoader->TryFinishLoad( ownerLoadId, pRequest->spOwner ) )
		{
			return false;
		}

		SetInvalid( pRequest->ownerLinkIndex );

		if( !pRequest->spOwner )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "CachePackageLoader: Failed to load owner object for \"%s\".\n" ),
				*pCacheEntry->path.ToString() );

			if( pObject )
			{
				pObject->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );
				pObject->ConditionalFinalizeLoad();
			}

			DefaultAllocator().Free( pRequest->pAsyncLoadBuffer );
			pRequest->pAsyncLoadBuffer = NULL;

			pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;

			return true;
		}
	}

	Asset* pOwner = pRequest->spOwner;

	HELIUM_ASSERT( !pOwner || pOwner->IsFullyLoaded() );
	HELIUM_ASSERT( !pTemplate || pTemplate->IsFullyLoaded() );

	AssetType* pType = pRequest->spType;
	HELIUM_ASSERT( pType );

	// If we already had an existing object, make sure the type and template match.
	if( pObject )
	{
		const AssetType* pExistingType = pObject->GetAssetType();
		HELIUM_ASSERT( pExistingType );
		if( pExistingType != pType )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				( TXT( "CachePackageLoader: Cannot load \"%s\" using the existing object as the types do not " )
				TXT( "match (existing type: \"%s\"; serialized type: \"%s\".\n" ) ),
				*pCacheEntry->path.ToString(),
				*pExistingType->GetName(),
				*pType->GetName() );

			pObject->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );
			pObject->ConditionalFinalizeLoad();

			DefaultAllocator().Free( pRequest->pAsyncLoadBuffer );
			pRequest->pAsyncLoadBuffer = NULL;

			pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;

			return true;
		}
	}
	else
	{
		// Create the object.
		if( !Asset::CreateObject( pRequest->spObject, pType, pCacheEntry->path.GetName(), pOwner, pTemplate ) )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "CachePackageLoader: Failed to create \"%s\" during loading.\n" ),
				*pCacheEntry->path.ToString() );

			DefaultAllocator().Free( pRequest->pAsyncLoadBuffer );
			pRequest->pAsyncLoadBuffer = NULL;

			pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;

			return true;
		}

		pObject = pRequest->spObject;
		HELIUM_ASSERT( pObject );
	}
		
	Reflect::ObjectPtr cached_object = Cache::ReadCacheObjectFromBuffer(
		pRequest->pSerializedData, 
		0, 
		pRequest->pPropertyStreamEnd - pRequest->pSerializedData, 
		pRequest->pResolver);

	if (!cached_object.ReferencesObject())
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "CachePackageLoader: Failed to deserialize object \"%s\".\n" ),
			*pCacheEntry->path.ToString() );

		// Clear out object references (object can now be considered fully loaded as well).
		// pmd - Not sure that we need to do this.. but if we do, just use this visitor
		//ClearLinkIndicesFromObject clifo_visitor;
		//pObject->Accept(clifo_visitor);
		pObject->SetFlags( Asset::FLAG_LINKED );
		pObject->ConditionalFinalizeLoad();

		pRequest->flags |= LOAD_FLAG_ERROR;
	}
	else
	{
		cached_object->CopyTo(pObject);
				
		if( !pObject->IsDefaultTemplate() )
		{
			// Load persistent resource data.
			Resource* pResource = Reflect::SafeCast< Resource >( pObject );
			if( pResource )
			{
				Reflect::ObjectPtr cached_prd = Cache::ReadCacheObjectFromBuffer(
					pRequest->pPropertyStreamEnd, 
					0, 
					(pRequest->pPersistentResourceStreamEnd - pRequest->pPropertyStreamEnd),
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
		HELIUM_ASSERT( rspPackage->IsClass( Package::GetStaticType()->GetClass() ) );
	}

	rspPackage->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED | Asset::FLAG_LOADED );
}

/// Deserialize the link tables for an object load.
///
/// @param[in] pRequest  Load request data.
bool CachePackageLoader::DeserializeLinkTables( LoadRequest* pRequest )
{
	HELIUM_ASSERT( pRequest );

	uint8_t* pBufferCurrent = pRequest->pAsyncLoadBuffer;
	uint8_t* pPropertyStreamEnd = pRequest->pPropertyStreamEnd;
	HELIUM_ASSERT( pBufferCurrent );
	HELIUM_ASSERT( pPropertyStreamEnd );
	HELIUM_ASSERT( pBufferCurrent <= pPropertyStreamEnd );

	uint32_t propertyStreamSize = 0;
	if( pBufferCurrent + sizeof( propertyStreamSize ) > pPropertyStreamEnd )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
			*pRequest->pEntry->path.ToString() );

		return false;
	}

	MemoryCopy( &propertyStreamSize, pBufferCurrent, sizeof( propertyStreamSize ) );
	pBufferCurrent += sizeof( propertyStreamSize );

	if( propertyStreamSize > static_cast< size_t >( pPropertyStreamEnd - pBufferCurrent ) )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			( TXT( "CachePackageLoader: Property stream size (%" ) PRIu32 TXT( " bytes) for \"%s\" exceeds the " )
			TXT( "amount of data cached.  Value will be clamped.\n" ) ),
			propertyStreamSize,
			*pRequest->pEntry->path.ToString() );

		propertyStreamSize = static_cast< uint32_t >( pPropertyStreamEnd - pBufferCurrent );
	}

	pPropertyStreamEnd = pBufferCurrent + propertyStreamSize;
	pRequest->pPropertyStreamEnd = pPropertyStreamEnd;

	// Adjust the end of the persistent resource data stream to account for the resource sub-data count padded on
	// the end (note that non-resources will not have this padding).
	if( pRequest->pPersistentResourceStreamEnd - pPropertyStreamEnd >= sizeof( uint32_t ) )
	{
		pRequest->pPersistentResourceStreamEnd -= sizeof( uint32_t );
	}
	else
	{
		pRequest->pPersistentResourceStreamEnd = pPropertyStreamEnd;
	}

	StackMemoryHeap<>& rStackHeap = ThreadLocalStackAllocator::GetMemoryHeap();

	// Load the type link table.
	uint32_t typeLinkTableSize = 0;
	if( pBufferCurrent + sizeof( typeLinkTableSize ) > pPropertyStreamEnd )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
			*pRequest->pEntry->path.ToString() );

		return false;
	}

	MemoryCopy( &typeLinkTableSize, pBufferCurrent, sizeof( typeLinkTableSize ) );
	pBufferCurrent += sizeof( typeLinkTableSize );

	pRequest->typeLinkTable.Resize( 0 );
	pRequest->typeLinkTable.Reserve( typeLinkTableSize );

	uint_fast32_t typeLinkTableSizeFast = typeLinkTableSize;
	for( uint_fast32_t linkTableIndex = 0; linkTableIndex < typeLinkTableSizeFast; ++linkTableIndex )
	{
		uint32_t typeNameSize;
		if( pBufferCurrent + sizeof( typeNameSize ) > pPropertyStreamEnd )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
				*pRequest->pEntry->path.ToString() );

			return false;
		}

		MemoryCopy( &typeNameSize, pBufferCurrent, sizeof( typeNameSize ) );
		pBufferCurrent += sizeof( typeNameSize );

		if( pBufferCurrent + sizeof( char ) * typeNameSize > pPropertyStreamEnd )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
				*pRequest->pEntry->path.ToString() );

			return false;
		}

		StackMemoryHeap<>::Marker stackMarker( rStackHeap );
		char* pTypeNameString = static_cast< char* >( rStackHeap.Allocate(
			sizeof( char ) * ( typeNameSize + 1 ) ) );
		HELIUM_ASSERT( pTypeNameString );

		MemoryCopy( pTypeNameString, pBufferCurrent, sizeof( char ) * typeNameSize );
		pBufferCurrent += sizeof( char ) * typeNameSize;

		pTypeNameString[ typeNameSize ] = TXT( '\0' );

		Name typeName( pTypeNameString );

		AssetType* pType = AssetType::Find( typeName );
		if( !pType )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "CachePackageLoader: Failed to locate type \"%s\" when attempting to deserialize \"%s\".\n" ),
				pTypeNameString,
				*pRequest->pEntry->path.ToString() );
		}

		pRequest->typeLinkTable.Push( pType );
	}

	// Load the object link table.
	uint32_t objectLinkTableSize = 0;
	if( pBufferCurrent + sizeof( objectLinkTableSize ) > pPropertyStreamEnd )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
			*pRequest->pEntry->path.ToString() );

		return false;
	}

	MemoryCopy( &objectLinkTableSize, pBufferCurrent, sizeof( objectLinkTableSize ) );
	pBufferCurrent += sizeof( objectLinkTableSize );

	pRequest->objectLinkTable.Resize( 0 );
	pRequest->objectLinkTable.Reserve( objectLinkTableSize );

	StackMemoryHeap<>::Marker stackMarker( rStackHeap );

	// Track the link table object paths so that we can use them for issuing additional load requests for the object
	// template and owner dependencies (this way, we can sync on those load requests during the preload process
	// while still providing load requests for the caller to resolve if necessary).
	AssetPath* pObjectLinkTablePaths = static_cast< AssetPath* >( rStackHeap.Allocate(
		sizeof( AssetPath ) * objectLinkTableSize ) );
	HELIUM_ASSERT( pObjectLinkTablePaths );
	ArrayUninitializedFill( pObjectLinkTablePaths, AssetPath( NULL_NAME ), objectLinkTableSize );

	AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
	HELIUM_ASSERT( pAssetLoader );

	uint_fast32_t objectLinkTableSizeFast = objectLinkTableSize;
	for( uint_fast32_t linkTableIndex = 0; linkTableIndex < objectLinkTableSizeFast; ++linkTableIndex )
	{
		uint32_t pathStringSize;
		if( pBufferCurrent + sizeof( pathStringSize ) > pPropertyStreamEnd )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
				*pRequest->pEntry->path.ToString() );

			return false;
		}

		MemoryCopy( &pathStringSize, pBufferCurrent, sizeof( pathStringSize ) );
		pBufferCurrent += sizeof( pathStringSize );

		if( pBufferCurrent + sizeof( char ) * pathStringSize > pPropertyStreamEnd )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
				*pRequest->pEntry->path.ToString() );

			return false;
		}

		StackMemoryHeap<>::Marker stackMarker( rStackHeap );
		char* pPathString = static_cast< char* >( rStackHeap.Allocate(
			sizeof( char ) * ( pathStringSize + 1 ) ) );
		HELIUM_ASSERT( pPathString );

		MemoryCopy( pPathString, pBufferCurrent, sizeof( char ) * pathStringSize );
		pBufferCurrent += sizeof( char ) * pathStringSize;

		pPathString[ pathStringSize ] = TXT( '\0' );

		size_t linkLoadId;
		SetInvalid( linkLoadId );

		AssetPath path;
		if( !path.Set( pPathString ) )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				( TXT( "CachePackageLoader: Invalid object path \"%s\" found in linker table when deserializing " )
				TXT( "\"%s\".  Setting to null.\n" ) ),
				pPathString,
				*pRequest->pEntry->path.ToString() );

			pRequest->flags |= LOAD_FLAG_ERROR;
		}
		else
		{
			pObjectLinkTablePaths[ linkTableIndex ] = path;

			// Begin loading the link table entry.
			linkLoadId = pAssetLoader->BeginLoadObject( path );
			if( IsInvalid( linkLoadId ) )
			{
				HELIUM_TRACE(
					TraceLevels::Error,
					( TXT( "CachePackageLoader: Failed to begin loading \"%s\" as a link dependency for \"%s\".  " )
					TXT( "Setting to null.\n" ) ),
					pPathString,
					*pRequest->pEntry->path.ToString() );

				pRequest->flags |= LOAD_FLAG_ERROR;
			}
		}

		pRequest->objectLinkTable.Push( linkLoadId );
	}

	// Read the type link information.
	uint32_t typeLinkIndex;
	if( pBufferCurrent + sizeof( typeLinkIndex ) > pPropertyStreamEnd )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
			*pRequest->pEntry->path.ToString() );

		return false;
	}

	MemoryCopy( &typeLinkIndex, pBufferCurrent, sizeof( typeLinkIndex ) );
	pBufferCurrent += sizeof( typeLinkIndex );

	if( typeLinkIndex >= typeLinkTableSizeFast )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "CachePackageLoader: Invalid link table index for the type of \"%s\".\n" ),
			*pRequest->pEntry->path.ToString() );

		return false;
	}

	AssetType* pType = pRequest->typeLinkTable[ typeLinkIndex ];
	if( !pType )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "CachePackageLoader: Type not found for object \"%s\".\n" ),
			*pRequest->pEntry->path.ToString() );

		return false;
	}

	pRequest->spType = pType;

	// Read the template link information.
	if( pBufferCurrent + sizeof( pRequest->templateLinkIndex ) > pPropertyStreamEnd )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
			*pRequest->pEntry->path.ToString() );

		return false;
	}

	MemoryCopy( &pRequest->templateLinkIndex, pBufferCurrent, sizeof( pRequest->templateLinkIndex ) );
	pBufferCurrent += sizeof( pRequest->templateLinkIndex );

	if( IsValid( pRequest->templateLinkIndex ) )
	{
		if( pRequest->templateLinkIndex >= objectLinkTableSizeFast )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "CachePackageLoader: Invalid link table index for the template of \"%s\".\n" ),
				*pRequest->pEntry->path.ToString() );

			SetInvalid( pRequest->templateLinkIndex );

			return false;
		}

		size_t templateLoadId = pAssetLoader->BeginLoadObject(
			pObjectLinkTablePaths[ pRequest->templateLinkIndex ] );
		HELIUM_ASSERT( templateLoadId == pRequest->objectLinkTable[ pRequest->templateLinkIndex ] );
		HELIUM_UNREF( templateLoadId );
	}

	// Read the owner link information.
	if( pBufferCurrent + sizeof( pRequest->ownerLinkIndex ) > pPropertyStreamEnd )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
			*pRequest->pEntry->path.ToString() );

		return false;
	}

	MemoryCopy( &pRequest->ownerLinkIndex, pBufferCurrent, sizeof( pRequest->ownerLinkIndex ) );
	pBufferCurrent += sizeof( pRequest->ownerLinkIndex );

	if( IsValid( pRequest->ownerLinkIndex ) )
	{
		if( pRequest->ownerLinkIndex >= objectLinkTableSizeFast )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "CachePackageLoader: Invalid link table index for the owner of \"%s\".\n" ),
				*pRequest->pEntry->path.ToString() );

			SetInvalid( pRequest->ownerLinkIndex );

			return false;
		}

		size_t ownerLoadId = pAssetLoader->BeginLoadObject( pObjectLinkTablePaths[ pRequest->ownerLinkIndex ] );
		HELIUM_ASSERT( ownerLoadId == pRequest->objectLinkTable[ pRequest->ownerLinkIndex ] );
		HELIUM_UNREF( ownerLoadId );
	}

	pRequest->pSerializedData = pBufferCurrent;

	return true;
}
