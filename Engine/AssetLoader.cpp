#include "EnginePch.h"
#include "Engine/AssetLoader.h"

#include "Platform/Thread.h"
#include "Engine/Asset.h"
#include "Engine/PackageLoader.h"
#include "Engine/FileLocations.h"

/// Asset cache name.

using namespace Helium;

AssetLoader* AssetLoader::sm_pInstance = NULL;

#if HELIUM_TOOLS
AssetTracker* AssetTracker::sm_pInstance = NULL;
#endif

/// Constructor.
AssetLoader::AssetLoader()
: m_loadRequestPool( LOAD_REQUEST_POOL_BLOCK_SIZE )
{
}

/// Destructor.
AssetLoader::~AssetLoader()
{
}

/// Begin asynchronous loading of an object.
///
/// @param[in] path  Asset path.
///
/// @return  ID for the load request if started successfully, invalid index if not.
///
/// @see TryFinishLoad(), FinishLoad()
size_t AssetLoader::BeginLoadObject( AssetPath path, bool forceReload )
{
	HELIUM_TRACE( TraceLevels::Info, TXT(" AssetLoader::BeginLoadObject - Loading path %s\n"), *path.ToString() );
	HELIUM_ASSERT( !path.GetName().IsEmpty() );

	// Search for an existing load request with the given path.
	ConcurrentHashMap< AssetPath, LoadRequest* >::ConstAccessor requestConstAccessor;
	if( m_loadRequestMap.Find( requestConstAccessor, path ) )
	{
		LoadRequest* pRequest = requestConstAccessor->Second();
		HELIUM_ASSERT( pRequest );
		AtomicIncrementRelease( pRequest->requestCount );

		// We can release now, as the request shouldn't get released now that we've incremented its reference count.
		requestConstAccessor.Release();

		return m_loadRequestPool.GetIndex( pRequest );
	}

	Asset *pAsset = NULL;

	if ( !forceReload )
	{
		pAsset = Asset::Find<Asset>( path );
		if ( pAsset && !pAsset->GetAllFlagsSet( Asset::FLAG_LOADED ) )
		{
			pAsset = NULL;
		}
	}


	PackageLoader *pPackageLoader = 0;
	if ( pAsset )
	{
			HELIUM_TRACE(
				TraceLevels::Info,
				TXT( "AssetLoader::BeginLoadObject(): Object \"%s\" already loaded.\n" ),
				*path.ToString() );
	} 
	else
	{
		// Get the package loader to use for the given object.
		pPackageLoader = GetPackageLoader( path );
		if( !pPackageLoader )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "AssetLoader::BeginLoadObject(): Failed to locate package loader for \"%s\".\n" ),
				*path.ToString() );

			return Invalid< size_t >();
		}
	}

	// Add the load request.
	LoadRequest* pRequest = m_loadRequestPool.Allocate();
	pRequest->path = path;
	pRequest->pPackageLoader = pPackageLoader;
	SetInvalid( pRequest->packageLoadRequestId );
	pRequest->stateFlags = pAsset ? 
		(pAsset->GetFlags() & Asset::FLAG_BROKEN ? LOAD_FLAG_FULLY_LOADED | LOAD_FLAG_ERROR : LOAD_FLAG_FULLY_LOADED ) : 
		0;
	pRequest->requestCount = 1;
	HELIUM_ASSERT( !pRequest->spObject );
	pRequest->spObject = pAsset;
	pRequest->forceReload = forceReload;

	ConcurrentHashMap< AssetPath, LoadRequest* >::Accessor requestAccessor;
	if( m_loadRequestMap.Insert( requestAccessor, KeyValue< AssetPath, LoadRequest* >( path, pRequest ) ) )
	{
		// New load request was created, so tick it once to get the load process running.
		requestAccessor.Release();
		TickLoadRequest( pRequest );
	}
	else
	{
		// A matching request was added while we were building our request, so reuse it.
		m_loadRequestPool.Release( pRequest );

		pRequest = requestAccessor->Second();
		HELIUM_ASSERT( pRequest );
		AtomicIncrementRelease( pRequest->requestCount );

		// We can release now, as the request shouldn't get released now that we've incremented its reference count.
		requestAccessor.Release();
	}

	return m_loadRequestPool.GetIndex( pRequest );
}

/// Test whether an object load request has completed, getting the result object if so.
///
/// Note that after a load request has completed (this function returns true), the request ID should no longer be
/// considered valid.
///
/// @param[in]  id         Load request ID.
/// @param[out] rspObject  Smart pointer set to the loaded object if loading has completed.  Note that this will be
///                        set to the object instance even if the object isn't finished loading (i.e. still being
///                        linked, etc.).
///
/// @return  True if the load request has completed, false if it is still being processed.
///
/// @see FinishLoad(), BeginLoadObject()
bool AssetLoader::TryFinishLoad( size_t id, AssetPtr& rspObject )
{
	HELIUM_ASSERT( IsValid( id ) );

	// Retrieve the load request and test whether it has completed.
	LoadRequest* pRequest = m_loadRequestPool.GetObject( id );
	HELIUM_ASSERT( pRequest );

	if( ( pRequest->stateFlags & LOAD_FLAG_FULLY_LOADED ) != LOAD_FLAG_FULLY_LOADED )
	{
		return false;
	}

	HELIUM_TRACE(
		TraceLevels::Debug,
		"AssetLoader::TryFinishLoad - Completed load for asset %s\n",
		*pRequest->path.ToString());

	HELIUM_ASSERT(  !pRequest->spObject.Get() || pRequest->spObject->IsFullyLoaded() || ( pRequest->spObject->GetFlags() & Asset::FLAG_BROKEN ) );

	rspObject = pRequest->spObject;

	// Acquire an exclusive lock to the request entry.
	AssetPath objectPath = pRequest->path;

	ConcurrentHashMap< AssetPath, LoadRequest* >::Accessor requestAccessor;
	HELIUM_VERIFY( m_loadRequestMap.Find( requestAccessor, objectPath ) );
	HELIUM_ASSERT( requestAccessor->Second() == pRequest );


	// Decrement the reference count on the load request, releasing it if the reference count reaches zero.
	int32_t newRequestCount = AtomicDecrementRelease( pRequest->requestCount );
	if( newRequestCount == 0 )
	{
		pRequest->spObject.Release();
		pRequest->resolver.Clear();

		m_loadRequestMap.Remove( requestAccessor );
		m_loadRequestPool.Release( pRequest );
	}

	requestAccessor.Release();

#if HELIUM_TOOLS
	if (rspObject)
	{
		if ( !(rspObject->SetFlags(Asset::FLAG_LOAD_EVENT_FIRED) & Asset::FLAG_LOAD_EVENT_FIRED) )
		{
			AssetTracker::GetStaticInstance()->NotifyAssetLoaded( rspObject.Get() );
		}
	}
#endif

	return true;
}

/// Block the current thread while waiting for an object load request or package pre-load request to complete.
///
/// Note that after a load request has completed, the request ID will no longer be valid.
///
/// @param[in]  id         Load request ID.
/// @param[out] rspObject  Smart pointer set to the loaded object if loading has completed.  If the object failed to
///                        load, this will be set to a null reference.
///
/// @see TryFinishLoad(), BeginLoadObject(), BeginPreloadPackage()
void AssetLoader::FinishLoad( size_t id, AssetPtr& rspObject )
{
	while( !TryFinishLoad( id, rspObject ) )
	{
		Tick();
		Thread::Yield();
	}
}

/// Load an object non-asynchronously.
///
/// This is equivalent to calling BeginLoadObject() followed by FinishLoad() on the returned request ID.
///
/// @param[in]  path       Asset path.
/// @param[out] rspObject  Smart pointer set to the loaded object if loading has completed.  If the object failed to
///                        load, this will be set to a null reference.
///
/// @return  True if the object load request was created successfully, false if not.  Note that this does not
///          signify a successful load (rspObject must be checked for a null reference to determine whether the load
///          was successful).
///
/// @see PreloadPackage()
bool AssetLoader::LoadObject( AssetPath path, AssetPtr& rspObject, bool forceReload )
{
	size_t id = BeginLoadObject( path, forceReload );
	if( IsInvalid( id ) )
	{
		return false;
	}

	FinishLoad( id, rspObject );

	return true;
}

#if HELIUM_TOOLS
/// Cache an object if it has been modified on disk.
///
/// The object will be cached based on the current timestamp of its source package file and, if one exists, its
/// source resource file.  As such, if changes have been made in memory to an object also stored in a source
/// package, it is recommended to save the changes to the source package first so that its updated timestamp will be
/// used in the cache.
///
/// @param[in] pObject                                 Asset to cache.
/// @param[in] bEvictPlatformPreprocessedResourceData  If the object being cached is a Resource-based object,
///                                                    specifying true will free the raw preprocessed resource data
///                                                    for the current platform after caching, while false will keep
///                                                    it in memory.  Typically, this data doesn't need to be kept
///                                                    around, as it will have already been deserialized into the
///                                                    resource itself, but certain resource preprocessors may want
///                                                    to keep this data intact.
///
/// @return  True if caching was successful, false if any errors occurred.
bool AssetLoader::CacheObject( Asset* /*pObject*/, bool /*bEvictPlatformPreprocessedResourceData*/ )
{
	// Caching only supported when using the editor object loader.
	return false;
}
#endif  // HELIUM_TOOLS

/// Update object loading.
void AssetLoader::Tick()
{
	// Tick package loaders first.
	TickPackageLoaders();

	// Build the list of object load requests to update this tick, incrementing the request count on each to prevent
	// them from being released while we don't have a lock on the request hash map.
	//HELIUM_ASSERT( m_loadRequestTickArray.IsEmpty() );
	/// List of load requests to update in the current tick.
	DynamicArray< LoadRequest* > m_loadRequestTickArray;

	ConcurrentHashMap< AssetPath, LoadRequest* >::ConstAccessor loadRequestConstAccessor;
	if( m_loadRequestMap.First( loadRequestConstAccessor ) )
	{
		do
		{
			LoadRequest* pRequest = loadRequestConstAccessor->Second();
			HELIUM_ASSERT( pRequest );
			AtomicIncrementUnsafe( pRequest->requestCount );
			m_loadRequestTickArray.Add( pRequest );

			++loadRequestConstAccessor;
		} while( loadRequestConstAccessor.IsValid() );
	}

	// Tick object load requests.
	size_t loadRequestCount = m_loadRequestTickArray.GetSize();
	for( size_t requestIndex = 0; requestIndex < loadRequestCount; ++requestIndex )
	{
		LoadRequest* pRequest = m_loadRequestTickArray[ requestIndex ];
		HELIUM_ASSERT( pRequest );

		//HELIUM_TRACE( TraceLevels::Info, TXT(  "Ticking pRequest %s %x\n"), *pRequest->path.ToString(), pRequest->stateFlags );

		TickLoadRequest( pRequest );

		int32_t newRequestCount = AtomicDecrementRelease( pRequest->requestCount );
		if( newRequestCount == 0 )
		{
			ConcurrentHashMap< AssetPath, LoadRequest* >::Accessor loadRequestAccessor;
			if( m_loadRequestMap.Find( loadRequestAccessor, pRequest->path ) )
			{
				pRequest = loadRequestAccessor->Second();
				HELIUM_ASSERT( pRequest );
				if( pRequest->requestCount == 0 )
				{
					HELIUM_ASSERT( ( pRequest->stateFlags & LOAD_FLAG_FULLY_LOADED ) == LOAD_FLAG_FULLY_LOADED );

					pRequest->spObject.Release();
					pRequest->resolver.Clear();

					m_loadRequestMap.Remove( loadRequestAccessor );
					m_loadRequestPool.Release( pRequest );
				}
			}
		}
	}

	//m_loadRequestTickArray.Resize( 0 );
}

/// Get the global object loader instance.
///
/// An object loader instance must be initialized first through the interface of the AssetLoader subclasses.
///
/// @return  Asset loader instance.  If an instance has not yet been initialized, this will return null.
///
/// @see DestroyStaticInstance()
AssetLoader* AssetLoader::GetStaticInstance()
{
	return sm_pInstance;
}

/// Destroy the global object loader instance if one exists.
///
/// @see GetStaticInstance()
void AssetLoader::DestroyStaticInstance()
{
	delete sm_pInstance;
	sm_pInstance = NULL;
}

/// @fn PackageLoader* AssetLoader::GetPackageLoader( AssetPath path )
/// Get the package loader to use for the specified object.
///
/// @param[in] path  Asset path.
///
/// @return  Package loader to use for loading the specified object.

/// @fn void AssetLoader::TickPackageLoaders()
/// Tick all package loaders for the current AssetLoader tick.

/// Perform work immediately prior to initiating resource precaching.
///
/// @param[in] pObject         Asset instance.
/// @param[in] pPackageLoader  Package loader used to load the given object.
void AssetLoader::OnPrecacheReady( const AssetPath & /*path*/, Asset* /*pObject*/, PackageLoader* /*pPackageLoader*/ )
{
}

/// Perform work upon completion of the load process for an object.
///
/// @param[in] path            Asset path.
/// @param[in] pObject         Asset instance (may be null if the object failed to load properly).
/// @param[in] pPackageLoader  Package loader used to load the given object.
void AssetLoader::OnLoadComplete( const AssetPath & /*path*/, Asset* /*pObject*/, PackageLoader* /*pPackageLoader*/ )
{
}

/// Update the given load request.
///
/// @param[in] pRequest  Load request to update.
///
/// @return  True if the load request has completed, false if it still requires time to process.
bool AssetLoader::TickLoadRequest( LoadRequest* pRequest )
{
	HELIUM_ASSERT( pRequest );

	// Update preloading.
	bool bLockedTick = false;

#define LOCK_TICK() \
	if( !bLockedTick ) \
	{ \
	if( AtomicOrAcquire( pRequest->stateFlags, LOAD_FLAG_IN_TICK ) & LOAD_FLAG_IN_TICK ) \
	{ \
	return false; \
	} \
	\
	bLockedTick = true; \
	}

#define UNLOCK_TICK() AtomicAndRelease( pRequest->stateFlags, ~LOAD_FLAG_IN_TICK )

	if( !( pRequest->stateFlags & LOAD_FLAG_PRELOADED ) )
	{
		LOCK_TICK();

		if( !TickPreload( pRequest ) )
		{
			UNLOCK_TICK();

			return false;
		}
		else
		{
			HELIUM_ASSERT( !pRequest->spObject.Get() || (pRequest->spObject->GetFlags() & Asset::FLAG_PRELOADED) );
		}
	}

	if( !( pRequest->stateFlags & LOAD_FLAG_LINKED ) )
	{
		LOCK_TICK();

		if( !TickLink( pRequest ) )
		{
			UNLOCK_TICK();

			return false;
		}
		else
		{
			HELIUM_ASSERT( !pRequest->spObject.Get() || pRequest->spObject->GetFlags() & Asset::FLAG_LINKED );
		}
	}

	if( !( pRequest->stateFlags & LOAD_FLAG_PRECACHED ) )
	{
		LOCK_TICK();

		if( !TickPrecache( pRequest ) )
		{
			UNLOCK_TICK();

			return false;
		}
		else
		{
			HELIUM_ASSERT( !pRequest->spObject.Get() || pRequest->spObject->GetFlags() & Asset::FLAG_PRECACHED );
		}
	}

	if( !( pRequest->stateFlags & LOAD_FLAG_LOADED ) )
	{
		LOCK_TICK();

		if( !TickFinalizeLoad( pRequest ) )
		{
			UNLOCK_TICK();

			return false;
		}
		else
		{
			HELIUM_ASSERT( !pRequest->spObject.Get() ||  pRequest->spObject->GetFlags() & Asset::FLAG_LOADED );
		}
	}

	if( bLockedTick )
	{
		UNLOCK_TICK();
	}

#undef LOCK_TICK
#undef UNLOCK_TICK

	return true;
}

/// Update property preloading for the given object load request.
///
/// @param[in] pRequest  Load request to update.
///
/// @return  True if preloading still needs processing, false if it is complete.
bool AssetLoader::TickPreload( LoadRequest* pRequest )
{
	HELIUM_ASSERT( pRequest );
	HELIUM_ASSERT( !( pRequest->stateFlags & ( LOAD_FLAG_LINKED | LOAD_FLAG_PRECACHED | LOAD_FLAG_LOADED ) ) );

	PackageLoader* pPackageLoader = pRequest->pPackageLoader;
	HELIUM_ASSERT( pPackageLoader );

	if( IsInvalid( pRequest->packageLoadRequestId ) )
	{
		if( !pPackageLoader->TryFinishPreload() )
		{
			// Still waiting for package loader preload.
			return false;
		}

		// Add an object load request.
		AssetPath path = pRequest->path;
		pRequest->packageLoadRequestId = pPackageLoader->BeginLoadObject( path, &pRequest->resolver, pRequest->forceReload );
		if( IsInvalid( pRequest->packageLoadRequestId ) )
		{
			pRequest->spObject = Asset::FindObject( path );
			Asset* pObject = pRequest->spObject;
			if( pObject )
			{
				HELIUM_TRACE(
					TraceLevels::Info,
					TXT( "AssetLoader: Asset \"%s\" is not serialized, but was found in memory.\n" ),
					*path.ToString() );

				// Make sure the object is preloaded and linked, but still perform resource caching and load
				// finalization if necessary.
				pObject->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );

				AtomicOrRelease( pRequest->stateFlags, LOAD_FLAG_PRELOADED | LOAD_FLAG_LINKED );

				return true;
			}

			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "AssetLoader: Asset \"%s\" is not serialized and does not exist in memory.\n" ),
				*path.ToString() );

			AtomicOrRelease( pRequest->stateFlags, LOAD_FLAG_FULLY_LOADED | LOAD_FLAG_ERROR );

			return true;
		}
	}

	HELIUM_ASSERT( IsValid( pRequest->packageLoadRequestId ) );

	bool bFinished = pPackageLoader->TryFinishLoadObject(
		pRequest->packageLoadRequestId,
		pRequest->spObject );
	if( !bFinished )
	{
		// Still waiting for object to load.
		return false;
	}

	// Preload complete.
	SetInvalid( pRequest->packageLoadRequestId );

	AtomicOrRelease( pRequest->stateFlags, LOAD_FLAG_PRELOADED );

	return true;
}

/// Update object reference linking for the given object load request.
///
/// @param[in] pRequest  Load request to update.
///
/// @return  True if linking still requires processing, false if it is complete.
bool AssetLoader::TickLink( LoadRequest* pRequest )
{
	HELIUM_ASSERT( pRequest );
	HELIUM_ASSERT( !( pRequest->stateFlags & ( LOAD_FLAG_PRECACHED | LOAD_FLAG_LOADED ) ) );

	if ( pRequest->spObject.ReferencesObject() )
	{
		if( !pRequest->resolver.ReadyToApplyFixups() )
		{
			return false;
		}
		
		HELIUM_TRACE( TraceLevels::Info, TXT( "Resolving references for %s\n"), *pRequest->path.ToString());

		pRequest->resolver.ApplyFixups();
		pRequest->spObject->SetFlags( Asset::FLAG_LINKED );
	}

	AtomicOrRelease( pRequest->stateFlags, LOAD_FLAG_LINKED );

	return true;
}

/// Update resource precaching for the given object load request.
///
/// @param[in] pRequest  Load request to update.
///
/// @return  True if resource precaching still requires processing, false if not.
bool AssetLoader::TickPrecache( LoadRequest* pRequest )
{
	HELIUM_ASSERT( pRequest );
	HELIUM_ASSERT( !( pRequest->stateFlags & LOAD_FLAG_LOADED ) );

	Asset* pObject = pRequest->spObject;
	if( pObject )
	{
		// TODO: SHouldn't this be in the linking phase?
		if ( !pRequest->resolver.TryFinishPrecachingDependencies() )
		{
			return false;
		}

		pRequest->resolver.Clear();

		// Perform any pre-precaching work (note that we don't precache anything for the default template object for
		// a given type).
		OnPrecacheReady( pRequest->path, pObject, pRequest->pPackageLoader );

		if( !pObject->GetAnyFlagSet( Asset::FLAG_BROKEN ) &&
			!pObject->IsDefaultTemplate() &&
			pObject->NeedsPrecacheResourceData() )
		{
			if( !( pRequest->stateFlags & LOAD_FLAG_PRECACHE_STARTED ) )
			{
				if( !pObject->BeginPrecacheResourceData() )
				{
					HELIUM_TRACE(
						TraceLevels::Error,
						TXT( "AssetLoader: Failed to begin precaching object \"%s\".\n" ),
						*pObject->GetPath().ToString() );

					pObject->SetFlags( Asset::FLAG_PRECACHED | Asset::FLAG_BROKEN );
					AtomicOrRelease( pRequest->stateFlags, LOAD_FLAG_PRECACHED | LOAD_FLAG_ERROR );

					return true;
				}

				AtomicOrRelease( pRequest->stateFlags, LOAD_FLAG_PRECACHE_STARTED );
			}

			if( !pObject->TryFinishPrecacheResourceData() )
			{
				return false;
			}
		}

		pObject->SetFlags( Asset::FLAG_PRECACHED );
	}

	AtomicOrRelease( pRequest->stateFlags, LOAD_FLAG_PRECACHED );

	return true;
}

/// Update loading finalization for the given object load request.
///
/// @param[in] pRequest  Load request to update.
///
/// @return  True if load finalization has completed, false if not.
bool AssetLoader::TickFinalizeLoad( LoadRequest* pRequest )
{
	HELIUM_ASSERT( pRequest );

	Asset* pObject = pRequest->spObject;
	if( pObject )
	{
		pObject->ConditionalFinalizeLoad();
	}

	// Loading now complete.
	OnLoadComplete( pRequest->path, pObject, pRequest->pPackageLoader );
	AtomicOrRelease( pRequest->stateFlags, LOAD_FLAG_LOADED );

	return true;
}

#if HELIUM_TOOLS

void AssetLoader::EnumerateRootPackages( DynamicArray< AssetPath > &packagePaths )
{
	HELIUM_BREAK_MSG("We tried to enumerate root packages with an asset loader that doesn't support doing that!");
}

int64_t AssetLoader::GetAssetFileTimestamp( const AssetPath &path )
{
	Package *pPackage = Asset::Find<Package>( path.GetParent() );
	HELIUM_ASSERT( pPackage );

	PackageLoader *pLoader = pPackage->GetLoader();
	HELIUM_ASSERT( pLoader );

	return pLoader->GetAssetFileSystemTimestamp( path );
}

#endif

bool Helium::AssetIdentifier::Identify( Reflect::Object* object, Name& identity )
{
	Asset *pAsset = Reflect::SafeCast<Asset>(object);

	if ( pAsset )
	{
		identity.Set(pAsset->GetPath().ToString());
		HELIUM_TRACE( TraceLevels::Info, TXT( "Identifying object [%s]\n" ), identity.Get() );
		return true;
	}
	else if ( object )
	{
		HELIUM_TRACE( TraceLevels::Info, TXT( "Deferring identification of object of type [%s]\n" ), object->GetMetaClass()->m_Name );
	}

	return false;
}

bool Helium::AssetResolver::Resolve( const Name& identity, Reflect::ObjectPtr& pointer, const Reflect::MetaClass* pointerClass )
{
	// Paths begin with /
	if (!identity.IsEmpty() && (*identity)[0] == '/')
	{
		HELIUM_TRACE( TraceLevels::Info, TXT( "Resolving object [%s]\n" ), identity.Get() );

		AssetPath p;
		p.Set(*identity);

		size_t loadRequestId = AssetLoader::GetStaticInstance()->BeginLoadObject(p);
		m_Fixups.Push( Fixup( pointer, pointerClass, loadRequestId ) );

		return true;
	}
	else
	{
#if HELIUM_TOOLS
		// Some extra checking to make friendly error messages
		String str ( identity.Get() );
		uint32_t index = Invalid< uint32_t >();
		int parseSuccessful = str.Parse( "%d", &index );

		if (!parseSuccessful)
		{
			HELIUM_TRACE(
				TraceLevels::Warning,
				"AssetResolver::Resolve - Identity '%s' is not a number, but doesn't start with '/'. If this is a path, it must begin with '/'!\n", 
				*str);
		}
#endif

		HELIUM_TRACE( TraceLevels::Debug, TXT( "Deferring resolution of [%s] to archive\n" ), identity.Get() );
	}

	return false;
}

bool Helium::AssetResolver::ReadyToApplyFixups()
{
	for ( DynamicArray< Fixup >::Iterator iter = m_Fixups.Begin();
		iter != m_Fixups.End(); ++iter)
	{
		// Retrieve the load request and test whether it has completed.
		AssetLoader::LoadRequest* pRequest = AssetLoader::GetStaticInstance()->m_loadRequestPool.GetObject( iter->m_LoadRequestId );

		if ( !( pRequest->stateFlags & AssetLoader::LOAD_FLAG_PRELOADED ) )
		{
			return false;
		}
	}

	return true;
}

void Helium::AssetResolver::ApplyFixups()
{
	for ( DynamicArray< Fixup >::Iterator iter = m_Fixups.Begin();
		iter != m_Fixups.End(); ++iter)
	{
		// Retrieve the load request and test whether it has completed.
		AssetLoader::LoadRequest* pRequest = AssetLoader::GetStaticInstance()->m_loadRequestPool.GetObject( iter->m_LoadRequestId );

		HELIUM_ASSERT( pRequest->stateFlags & AssetLoader::LOAD_FLAG_PRELOADED );
		if( !pRequest->spObject.ReferencesObject() )
		{
			HELIUM_TRACE( TraceLevels::Warning, TXT( "Reference to %s could not be found\n"), 
				*pRequest->path.ToString());
		}

		iter->m_Pointer.Set(pRequest->spObject);
	}
}

void Helium::AssetResolver::Clear()
{
	m_Fixups.Clear();
}

bool Helium::AssetResolver::TryFinishPrecachingDependencies()
{
	for ( DynamicArray< Fixup >::Iterator iter = m_Fixups.Begin();
		iter != m_Fixups.End(); ++iter)
	{
		if ( IsValid( iter->m_LoadRequestId ) )
		{
			AssetPtr asset;
			if( !AssetLoader::GetStaticInstance()->TryFinishLoad( iter->m_LoadRequestId, asset ) )
			{
				return false;
			}
		
			SetInvalid( iter->m_LoadRequestId );
		}

	}

	return true;
}

#if HELIUM_TOOLS


AssetTracker* AssetTracker::GetStaticInstance()
{
	if (!sm_pInstance)
	{
		sm_pInstance = new AssetTracker();
	}

	return sm_pInstance;
}

void AssetTracker::DestroyStaticInstance()
{
	delete sm_pInstance;
	sm_pInstance = NULL;
}

void Helium::AssetTracker::NotifyAssetLoaded( Asset *pAsset )
{
	pAsset->e_Changed.AddMethod( this, &AssetTracker::OnAssetChanged );
	e_AssetLoaded.Raise( AssetEventArgs( pAsset ) );
}

void Helium::AssetTracker::NotifyAssetCreatedExternally( const AssetPath &pAsset )
{
	//e_AssetCreatedExternally.Raise( AssetEventArgs( pAsset ) );
}

void Helium::AssetTracker::NotifyAssetChangedExternally( const AssetPath &pAsset )
{
	//e_AssetChangedExternally.Raise( AssetEventArgs( pAsset ) );
}

void AssetTracker::OnAssetChanged( const Reflect::ObjectChangeArgs &args )
{
	Asset *pAsset = const_cast<Asset *>(Reflect::AssertCast< Asset >( args.m_Object ));
	pAsset->SetFlags( Asset::FLAG_CHANGED_SINCE_LOADED );

	e_AssetChanged.Raise( AssetEventArgs( pAsset ) );
}

#endif
