#include "EnginePch.h"
#include "Engine/GameObjectLoader.h"

#include "Platform/Thread.h"
#include "Engine/GameObjectType.h"
#include "Engine/GameObject.h"
#include "Engine/Package.h"
#include "Engine/PackageLoader.h"

#include "Engine/GameObjectPointerData.h"

/// GameObject cache name.
#define HELIUM_OBJECT_CACHE_NAME TXT( "GameObject" )

using namespace Helium;

GameObjectLoader* GameObjectLoader::sm_pInstance = NULL;

/// Constructor.
GameObjectLoader::GameObjectLoader()
: m_loadRequestPool( LOAD_REQUEST_POOL_BLOCK_SIZE )
, m_cacheName( HELIUM_OBJECT_CACHE_NAME )
{
}

/// Destructor.
GameObjectLoader::~GameObjectLoader()
{
}

/// Begin asynchronous loading of an object.
///
/// @param[in] path  GameObject path.
///
/// @return  ID for the load request if started successfully, invalid index if not.
///
/// @see TryFinishLoad(), FinishLoad()
size_t GameObjectLoader::BeginLoadObject( GameObjectPath path )
{
	// Search for an existing load request with the given path.
	ConcurrentHashMap< GameObjectPath, LoadRequest* >::ConstAccessor requestConstAccessor;
	if( m_loadRequestMap.Find( requestConstAccessor, path ) )
	{
		LoadRequest* pRequest = requestConstAccessor->Second();
		HELIUM_ASSERT( pRequest );
		AtomicIncrementRelease( pRequest->requestCount );

		// We can release now, as the request shouldn't get released now that we've incremented its reference count.
		requestConstAccessor.Release();

		return m_loadRequestPool.GetIndex( pRequest );
	}

	// Get the package loader to use for the given object.
	PackageLoader* pPackageLoader = GetPackageLoader( path );
	if( !pPackageLoader )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "GameObjectLoader::BeginLoadObject(): Failed to locate package loader for \"%s\".\n" ),
			*path.ToString() );

		return Invalid< size_t >();
	}

	// Add the load request.
	LoadRequest* pRequest = m_loadRequestPool.Allocate();
	pRequest->path = path;
	HELIUM_ASSERT( !pRequest->spObject );
	pRequest->pPackageLoader = pPackageLoader;
	SetInvalid( pRequest->packageLoadRequestId );
	HELIUM_ASSERT( pRequest->linkTable.IsEmpty() );
	pRequest->stateFlags = 0;
	pRequest->requestCount = 1;

	ConcurrentHashMap< GameObjectPath, LoadRequest* >::Accessor requestAccessor;
	if( m_loadRequestMap.Insert( requestAccessor, KeyValue< GameObjectPath, LoadRequest* >( path, pRequest ) ) )
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
bool GameObjectLoader::TryFinishLoad( size_t id, GameObjectPtr& rspObject )
{
	HELIUM_ASSERT( IsValid( id ) );

	// Retrieve the load request and test whether it has completed.
	LoadRequest* pRequest = m_loadRequestPool.GetObject( id );
	HELIUM_ASSERT( pRequest );
	rspObject = pRequest->spObject;

	if( ( pRequest->stateFlags & LOAD_FLAG_FULLY_LOADED ) != LOAD_FLAG_FULLY_LOADED )
	{
		return false;
	}

	// Acquire an exclusive lock to the request entry.
	GameObjectPath objectPath = pRequest->path;

	ConcurrentHashMap< GameObjectPath, LoadRequest* >::Accessor requestAccessor;
	HELIUM_VERIFY( m_loadRequestMap.Find( requestAccessor, objectPath ) );
	HELIUM_ASSERT( requestAccessor->Second() == pRequest );

	// Decrement the reference count on the load request, releasing it if the reference count reaches zero.
	int32_t newRequestCount = AtomicDecrementRelease( pRequest->requestCount );
	if( newRequestCount == 0 )
	{
		pRequest->spObject.Release();
		pRequest->linkTable.Resize( 0 );

		m_loadRequestMap.Remove( requestAccessor );
		m_loadRequestPool.Release( pRequest );
	}

	requestAccessor.Release();

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
void GameObjectLoader::FinishLoad( size_t id, GameObjectPtr& rspObject )
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
/// @param[in]  path       GameObject path.
/// @param[out] rspObject  Smart pointer set to the loaded object if loading has completed.  If the object failed to
///                        load, this will be set to a null reference.
///
/// @return  True if the object load request was created successfully, false if not.  Note that this does not
///          signify a successful load (rspObject must be checked for a null reference to determine whether the load
///          was successful).
///
/// @see PreloadPackage()
bool GameObjectLoader::LoadObject( GameObjectPath path, GameObjectPtr& rspObject )
{
	size_t id = BeginLoadObject( path );
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
/// @param[in] pObject                                 GameObject to cache.
/// @param[in] bEvictPlatformPreprocessedResourceData  If the object being cached is a Resource-based object,
///                                                    specifying true will free the raw preprocessed resource data
///                                                    for the current platform after caching, while false will keep
///                                                    it in memory.  Typically, this data doesn't need to be kept
///                                                    around, as it will have already been deserialized into the
///                                                    resource itself, but certain resource preprocessors may want
///                                                    to keep this data intact.
///
/// @return  True if caching was successful, false if any errors occurred.
bool GameObjectLoader::CacheObject( GameObject* /*pObject*/, bool /*bEvictPlatformPreprocessedResourceData*/ )
{
	// Caching only supported when using the editor object loader.
	return false;
}
#endif  // HELIUM_TOOLS

/// Update object loading.
void GameObjectLoader::Tick()
{
	// Tick package loaders first.
	TickPackageLoaders();

	// Build the list of object load requests to update this tick, incrementing the request count on each to prevent
	// them from being released while we don't have a lock on the request hash map.
	//HELIUM_ASSERT( m_loadRequestTickArray.IsEmpty() );
	/// List of load requests to update in the current tick.
	DynArray< LoadRequest* > m_loadRequestTickArray;

	ConcurrentHashMap< GameObjectPath, LoadRequest* >::ConstAccessor loadRequestConstAccessor;
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

		TickLoadRequest( pRequest );

		int32_t newRequestCount = AtomicDecrementRelease( pRequest->requestCount );
		if( newRequestCount == 0 )
		{
			ConcurrentHashMap< GameObjectPath, LoadRequest* >::Accessor loadRequestAccessor;
			if( m_loadRequestMap.Find( loadRequestAccessor, pRequest->path ) )
			{
				pRequest = loadRequestAccessor->Second();
				HELIUM_ASSERT( pRequest );
				if( pRequest->requestCount == 0 )
				{
					HELIUM_ASSERT( ( pRequest->stateFlags & LOAD_FLAG_FULLY_LOADED ) == LOAD_FLAG_FULLY_LOADED );

					pRequest->spObject.Release();
					pRequest->linkTable.Resize( 0 );

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
/// An object loader instance must be initialized first through the interface of the GameObjectLoader subclasses.
///
/// @return  GameObject loader instance.  If an instance has not yet been initialized, this will return null.
///
/// @see DestroyStaticInstance()
GameObjectLoader* GameObjectLoader::GetStaticInstance()
{
	return sm_pInstance;
}

/// Destroy the global object loader instance if one exists.
///
/// @see GetStaticInstance()
void GameObjectLoader::DestroyStaticInstance()
{
	delete sm_pInstance;
	sm_pInstance = NULL;
}

/// @fn PackageLoader* GameObjectLoader::GetPackageLoader( GameObjectPath path )
/// Get the package loader to use for the specified object.
///
/// @param[in] path  GameObject path.
///
/// @return  Package loader to use for loading the specified object.

/// @fn void GameObjectLoader::TickPackageLoaders()
/// Tick all package loaders for the current GameObjectLoader tick.

/// Perform work immediately prior to initiating resource precaching.
///
/// @param[in] pObject         GameObject instance.
/// @param[in] pPackageLoader  Package loader used to load the given object.
void GameObjectLoader::OnPrecacheReady( GameObject* /*pObject*/, PackageLoader* /*pPackageLoader*/ )
{
}

/// Perform work upon completion of the load process for an object.
///
/// @param[in] path            GameObject path.
/// @param[in] pObject         GameObject instance (may be null if the object failed to load properly).
/// @param[in] pPackageLoader  Package loader used to load the given object.
void GameObjectLoader::OnLoadComplete( GameObjectPath /*path*/, GameObject* /*pObject*/, PackageLoader* /*pPackageLoader*/ )
{
}

/// Update the given load request.
///
/// @param[in] pRequest  Load request to update.
///
/// @return  True if the load request has completed, false if it still requires time to process.
bool GameObjectLoader::TickLoadRequest( LoadRequest* pRequest )
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
	}

	if( !( pRequest->stateFlags & LOAD_FLAG_LINKED ) )
	{
		LOCK_TICK();

		if( !TickLink( pRequest ) )
		{
			UNLOCK_TICK();

			return false;
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
	}

	if( !( pRequest->stateFlags & LOAD_FLAG_LOADED ) )
	{
		LOCK_TICK();

		if( !TickFinalizeLoad( pRequest ) )
		{
			UNLOCK_TICK();

			return false;
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
bool GameObjectLoader::TickPreload( LoadRequest* pRequest )
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
		GameObjectPath path = pRequest->path;
		pRequest->packageLoadRequestId = pPackageLoader->BeginLoadObject( path );
		if( IsInvalid( pRequest->packageLoadRequestId ) )
		{
			pRequest->spObject = GameObject::FindObject( path );
			GameObject* pObject = pRequest->spObject;
			if( pObject )
			{
				HELIUM_TRACE(
					TraceLevels::Warning,
					TXT( "GameObjectLoader: GameObject \"%s\" is not serialized, but was found in memory.\n" ),
					*path.ToString() );

				// Make sure the object is preloaded and linked, but still perform resource caching and load
				// finalization if necessary.
				pObject->SetFlags( GameObject::FLAG_PRELOADED | GameObject::FLAG_LINKED );

				AtomicOrRelease( pRequest->stateFlags, LOAD_FLAG_PRELOADED | LOAD_FLAG_LINKED );

				return true;
			}

			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "GameObjectLoader: GameObject \"%s\" is not serialized and does not exist in memory.\n" ),
				*path.ToString() );

			AtomicOrRelease( pRequest->stateFlags, LOAD_FLAG_FULLY_LOADED | LOAD_FLAG_ERROR );

			return true;
		}
	}

	HELIUM_ASSERT( IsValid( pRequest->packageLoadRequestId ) );

	bool bFinished = pPackageLoader->TryFinishLoadObject(
		pRequest->packageLoadRequestId,
		pRequest->spObject,
		pRequest->linkTable );
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

namespace Helium
{
    class PopulateObjectFromLinkTable : public Reflect::Visitor
    {
    private:
        GameObject &m_Owner;
        DynArray<GameObjectLoader::LinkEntry>& m_LinkTable;
        bool m_bError;
        
    public:
        PopulateObjectFromLinkTable(GameObject &_owner, DynArray<GameObjectLoader::LinkEntry> &_link_table)
            :   m_Owner(_owner),
                m_LinkTable( _link_table ),
                m_bError(false)
        {
        }

        virtual ~PopulateObjectFromLinkTable()
        {
        }
        
        virtual bool VisitPointer(Reflect::ObjectPtr& _pointer)
        {
            if (_pointer.HasLinkIndex())
            {
                size_t link_index = _pointer.GetLinkIndex();
                _pointer.ClearLinkIndex();

                if( link_index >= m_LinkTable.GetSize() )
                {
                    HELIUM_TRACE(
                        TraceLevels::Error,
                        TXT( "GameObjectLoader: Invalid link index %" ) TPRIu32 TXT( " encountered.  Setting null reference.\n" ),
                        link_index );

                    m_bError = true;

                    return false;
                }

                GameObject* pObject = m_LinkTable[ link_index ].spObject;
                if( pObject )
                {
//                     HELIUM_ASSERT(field->m_Type->HasReflectionType(Reflect::ReflectionTypes::GameObjectType));
//                     const GameObjectType *go_type = static_cast<const GameObjectType *>(field->m_Type);
//                     if( !pObject->IsClass( go_type ) )
//                     {
//                         HELIUM_TRACE(
//                             TraceLevels::Error,
//                             TXT( "GameObjectLoader: GameObject reference \"%s\" is not of the correct type (\"%s\").\n" ),
//                             *pObject->GetPath().ToString(),
//                             *go_type->GetName() );
// 
//                         m_bError = true;
//                     }
//                     else
//                     {
////                             if (field->m_Flags & Reflect::FieldFlags::Share)
////                             {
                            _pointer.Set(pObject);
////                             }
////                             else
////                             {
////                                 GameObjectPtr new_object_ptr;
////                                 bool success = GameObject::CreateObject(new_object_ptr, pObject->GetGameObjectType(), pObject->GetName(), &m_Owner, pObject, true);
//// 
////                                 if (!success)
////                                 {
////                                     HELIUM_TRACE(
////                                         TraceLevels::Error,
////                                         TXT( "GameObjectLoader: Could not create GameObject for non-shared GameObject reference \"%s\" .\n" ),
////                                         *pObject->GetPath().ToString());
//// 
////                                     m_bError = true;
////                                 }
//// 
////                                 go_data->m_Data->Set(new_object_ptr);
////                             }
//                    }
                }
            }

            return true;
        }

        virtual bool VisitField(void* instance, const Reflect::Field* field) HELIUM_OVERRIDE
        {
            return true;
        }
    };
}

/// Update object reference linking for the given object load request.
///
/// @param[in] pRequest  Load request to update.
///
/// @return  True if linking still requires processing, false if it is complete.
bool GameObjectLoader::TickLink( LoadRequest* pRequest )
{
	HELIUM_ASSERT( pRequest );
	HELIUM_ASSERT( !( pRequest->stateFlags & ( LOAD_FLAG_PRECACHED | LOAD_FLAG_LOADED ) ) );

	// Make sure each dependency has finished its preload process.
	bool bHavePendingLinkEntries = false;

	DynArray< LinkEntry >& rLinkTable = pRequest->linkTable;
	size_t linkTableSize = rLinkTable.GetSize();
	for( size_t linkIndex = 0; linkIndex < linkTableSize; ++linkIndex )
	{
		LinkEntry& rLinkEntry = rLinkTable[ linkIndex ];
		if( IsValid( rLinkEntry.loadId ) )
		{
			LoadRequest* pLinkRequest = m_loadRequestPool.GetObject( rLinkEntry.loadId );
			HELIUM_ASSERT( pLinkRequest );
			if( pLinkRequest->stateFlags & LOAD_FLAG_PRELOADED )
			{
				rLinkEntry.spObject = pLinkRequest->spObject;
			}
			else
			{
				bHavePendingLinkEntries = true;
			}
		}
	}

	if( bHavePendingLinkEntries )
	{
		return false;
	}

	// Ready to link.
	GameObject* pObject = pRequest->spObject;
	if( pObject )
	{
		uint32_t objectFlags = pObject->GetFlags();
		if( !( objectFlags & GameObject::FLAG_LINKED ) )
		{
			if( !( objectFlags & GameObject::FLAG_BROKEN ) )
			{
				PopulateObjectFromLinkTable visitor(*pObject, rLinkTable);
				pObject->Accept(visitor);
			}

			pObject->SetFlags( GameObject::FLAG_LINKED );
		}
	}

	AtomicOrRelease( pRequest->stateFlags, LOAD_FLAG_LINKED );

	return true;
}

/// Update resource precaching for the given object load request.
///
/// @param[in] pRequest  Load request to update.
///
/// @return  True if resource precaching still requires processing, false if not.
bool GameObjectLoader::TickPrecache( LoadRequest* pRequest )
{
	HELIUM_ASSERT( pRequest );
	HELIUM_ASSERT( !( pRequest->stateFlags & LOAD_FLAG_LOADED ) );

	GameObject* pObject = pRequest->spObject;
	if( pObject )
	{
		// Wait for all link dependencies to fully load first.
		DynArray< LinkEntry >& rLinkTable = pRequest->linkTable;
		size_t linkTableSize = rLinkTable.GetSize();
		for( size_t linkIndex = 0; linkIndex < linkTableSize; ++linkIndex )
		{
			LinkEntry& rLinkEntry = rLinkTable[ linkIndex ];
			if( IsValid( rLinkEntry.loadId ) )
			{
				if( !TryFinishLoad( rLinkEntry.loadId, rLinkEntry.spObject ) )
				{
					return false;
				}

				SetInvalid( rLinkEntry.loadId );
				rLinkEntry.spObject.Release();
			}
		}

		rLinkTable.Resize( 0 );

		// Perform any pre-precaching work (note that we don't precache anything for the default template object for
		// a given type).
		OnPrecacheReady( pObject, pRequest->pPackageLoader );

		if( !pObject->GetAnyFlagSet( GameObject::FLAG_BROKEN ) &&
			!pObject->IsDefaultTemplate() &&
			pObject->NeedsPrecacheResourceData() )
		{
			if( !( pRequest->stateFlags & LOAD_FLAG_PRECACHE_STARTED ) )
			{
				if( !pObject->BeginPrecacheResourceData() )
				{
					HELIUM_TRACE(
						TraceLevels::Error,
						TXT( "GameObjectLoader: Failed to begin precaching object \"%s\".\n" ),
						*pObject->GetPath().ToString() );

					pObject->SetFlags( GameObject::FLAG_PRECACHED | GameObject::FLAG_BROKEN );
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

		pObject->SetFlags( GameObject::FLAG_PRECACHED );
	}

	AtomicOrRelease( pRequest->stateFlags, LOAD_FLAG_PRECACHED );

	return true;
}

/// Update loading finalization for the given object load request.
///
/// @param[in] pRequest  Load request to update.
///
/// @return  True if load finalization has completed, false if not.
bool GameObjectLoader::TickFinalizeLoad( LoadRequest* pRequest )
{
	HELIUM_ASSERT( pRequest );

	GameObject* pObject = pRequest->spObject;
	if( pObject )
	{
		pObject->ConditionalFinalizeLoad();
	}

	// Loading now complete.
	OnLoadComplete( pRequest->path, pObject, pRequest->pPackageLoader );
	AtomicOrRelease( pRequest->stateFlags, LOAD_FLAG_LOADED );

	return true;
}

///// Constructor.
//GameObjectLoader::Linker::Linker()
//: m_pLinkEntries( NULL )
//, m_linkEntryCount( 0 )
//, m_bError( false )
//{
//}
//
///// Destructor.
//GameObjectLoader::Linker::~Linker()
//{
//}
//
///// Prepare for linking object references based on the given link table.
/////
///// @param[in] pLinkEntries    Array of link table entries.
///// @param[in] linkEntryCount  Number of entries in the link table.
//void GameObjectLoader::Linker::Prepare( const LinkEntry* pLinkEntries, uint32_t linkEntryCount )
//{
//    HELIUM_ASSERT( pLinkEntries || linkEntryCount == 0 );
//
//    m_pLinkEntries = pLinkEntries;
//    m_linkEntryCount = linkEntryCount;
//}
//
///// @copydoc Serializer::Serialize()
//bool GameObjectLoader::Linker::Serialize( GameObject* pObject )
//{
//    HELIUM_ASSERT( pObject );
//
//    HELIUM_TRACE( TraceLevels::Debug, TXT( "GameObjectLoader::Linker: Linking \"%s\".\n" ), *pObject->GetPath().ToString() );
//
//    HELIUM_ASSERT( m_pLinkEntries || m_linkEntryCount == 0 );
//
//    m_bError = false;
//    pObject->Serialize( *this );
//
//    return !m_bError;
//}
//
///// @copydoc Serializer::GetMode()
//Serializer::EMode GameObjectLoader::Linker::GetMode() const
//{
//    return MODE_LINK;
//}
//
///// @copydoc Serializer::SerializeTag()
//void GameObjectLoader::Linker::SerializeTag( const Tag& /*rTag*/ )
//{
//}
//
///// @copydoc Serializer::CanResolveTags()
//bool GameObjectLoader::Linker::CanResolveTags() const
//{
//    return false;
//}
//
///// @name Serializer::SerializeBool()
//void GameObjectLoader::Linker::SerializeBool( bool& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeInt8()
//void GameObjectLoader::Linker::SerializeInt8( int8_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeUint8()
//void GameObjectLoader::Linker::SerializeUint8( uint8_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeInt16()
//void GameObjectLoader::Linker::SerializeInt16( int16_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeUint16()
//void GameObjectLoader::Linker::SerializeUint16( uint16_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeInt32()
//void GameObjectLoader::Linker::SerializeInt32( int32_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeUint32()
//void GameObjectLoader::Linker::SerializeUint32( uint32_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeInt64()
//void GameObjectLoader::Linker::SerializeInt64( int64_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeUint64()
//void GameObjectLoader::Linker::SerializeUint64( uint64_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeFloat32()
//void GameObjectLoader::Linker::SerializeFloat32( float32_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeFloat64()
//void GameObjectLoader::Linker::SerializeFloat64( float64_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeBuffer()
//void GameObjectLoader::Linker::SerializeBuffer( void* /*pBuffer*/, size_t /*elementSize*/, size_t /*count*/ )
//{
//}
//
///// @name Serializer::SerializeEnum()
//void GameObjectLoader::Linker::SerializeEnum(
//    int32_t& /*rValue*/,
//    uint32_t /*nameCount*/,
//    const tchar_t* const* /*ppNames*/ )
//{
//}
//
///// @name Serializer::SerializeEnum()
//void GameObjectLoader::Linker::SerializeEnum(
//    int32_t& /*rValue*/,
//    const Helium::Reflect::Enumeration* /*pEnumeration*/ )
//{
//}
//
///// @name Serializer::SerializeName()
//void GameObjectLoader::Linker::SerializeName( Name& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeString()
//void GameObjectLoader::Linker::SerializeString( String& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeObjectReference()
//void GameObjectLoader::Linker::SerializeObjectReference( const GameObjectType* pType, GameObjectPtr& rspObject )
//{
//    HELIUM_ASSERT( pType );
//
//    uint32_t linkIndex = rspObject.GetLinkIndex();
//    rspObject.ClearLinkIndex();
//
//    if( IsInvalid( linkIndex ) )
//    {
//        return;
//    }
//
//    if( linkIndex >= m_linkEntryCount )
//    {
//        HELIUM_TRACE(
//            TraceLevels::Error,
//            TXT( "GameObjectLoader: Invalid link index %" ) TPRIu32 TXT( " encountered.  Setting null reference.\n" ),
//            linkIndex );
//
//        m_bError = true;
//
//        return;
//    }
//
//    GameObject* pObject = m_pLinkEntries[ linkIndex ].spObject;
//    if( pObject )
//    {
//        if( !pObject->IsClass( pType ) )
//        {
//            HELIUM_TRACE(
//                TraceLevels::Error,
//                TXT( "GameObjectLoader: GameObject reference \"%s\" is not of the correct type (\"%s\").\n" ),
//                *pObject->GetPath().ToString(),
//                *pType->GetName() );
//
//            m_bError = true;
//        }
//        else
//        {
//            rspObject = pObject;
//        }
//    }
//}
