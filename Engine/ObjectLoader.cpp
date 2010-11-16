//----------------------------------------------------------------------------------------------------------------------
// ObjectLoader.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/ObjectLoader.h"

#include "Platform/Thread.h"
#include "Engine/PackageLoader.h"

/// Object cache name.
#define L_OBJECT_CACHE_NAME TXT( "Object" )

namespace Lunar
{
    ObjectLoader* ObjectLoader::sm_pInstance = NULL;

    /// Constructor.
    ObjectLoader::ObjectLoader()
        : m_loadRequestPool( LOAD_REQUEST_POOL_BLOCK_SIZE )
        , m_cacheName( L_OBJECT_CACHE_NAME )
    {
    }

    /// Destructor.
    ObjectLoader::~ObjectLoader()
    {
    }

    /// Begin asynchronous loading of an object.
    ///
    /// @param[in] path  Object path.
    ///
    /// @return  ID for the load request if started successfully, invalid index if not.
    ///
    /// @see TryFinishLoad(), FinishLoad()
    size_t ObjectLoader::BeginLoadObject( ObjectPath path )
    {
        // Search for an existing load request with the given path.
        ConcurrentHashMap< ObjectPath, LoadRequest* >::ConstAccessor requestConstAccessor;
        if( m_loadRequestMap.Find( requestConstAccessor, path ) )
        {
            LoadRequest* pRequest = requestConstAccessor->second;
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
                TRACE_ERROR,
                TXT( "ObjectLoader::BeginLoadObject(): Failed to locate package loader for \"%s\".\n" ),
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

        ConcurrentHashMap< ObjectPath, LoadRequest* >::Accessor requestAccessor;
        if( m_loadRequestMap.Insert( requestAccessor, std::pair< const ObjectPath, LoadRequest* >( path, pRequest ) ) )
        {
            // New load request was created, so tick it once to get the load process running.
            requestAccessor.Release();
            TickLoadRequest( pRequest );
        }
        else
        {
            // A matching request was added while we were building our request, so reuse it.
            m_loadRequestPool.Release( pRequest );

            pRequest = requestAccessor->second;
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
    bool ObjectLoader::TryFinishLoad( size_t id, ObjectPtr& rspObject )
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
        ObjectPath objectPath = pRequest->path;

        ConcurrentHashMap< ObjectPath, LoadRequest* >::Accessor requestAccessor;
        HELIUM_VERIFY( m_loadRequestMap.Find( requestAccessor, objectPath ) );
        HELIUM_ASSERT( requestAccessor->second == pRequest );

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
    void ObjectLoader::FinishLoad( size_t id, ObjectPtr& rspObject )
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
    /// @param[in]  path       Object path.
    /// @param[out] rspObject  Smart pointer set to the loaded object if loading has completed.  If the object failed to
    ///                        load, this will be set to a null reference.
    ///
    /// @return  True if the object load request was created successfully, false if not.  Note that this does not
    ///          signify a successful load (rspObject must be checked for a null reference to determine whether the load
    ///          was successful).
    ///
    /// @see PreloadPackage()
    bool ObjectLoader::LoadObject( ObjectPath path, ObjectPtr& rspObject )
    {
        size_t id = BeginLoadObject( path );
        if( IsInvalid( id ) )
        {
            return false;
        }

        FinishLoad( id, rspObject );

        return true;
    }

#if L_EDITOR
    /// Cache an object if it has been modified on disk.
    ///
    /// The object will be cached based on the current timestamp of its source package file and, if one exists, its
    /// source resource file.  As such, if changes have been made in memory to an object also stored in a source
    /// package, it is recommended to save the changes to the source package first so that its updated timestamp will be
    /// used in the cache.
    ///
    /// @param[in] pObject                                 Object to cache.
    /// @param[in] bEvictPlatformPreprocessedResourceData  If the object being cached is a Resource-based object,
    ///                                                    specifying true will free the raw preprocessed resource data
    ///                                                    for the current platform after caching, while false will keep
    ///                                                    it in memory.  Typically, this data doesn't need to be kept
    ///                                                    around, as it will have already been deserialized into the
    ///                                                    resource itself, but certain resource preprocessors may want
    ///                                                    to keep this data intact.
    ///
    /// @return  True if caching was successful, false if any errors occurred.
    bool ObjectLoader::CacheObject( Object* /*pObject*/, bool /*bEvictPlatformPreprocessedResourceData*/ )
    {
        // Caching only supported when using the editor object loader.
        return false;
    }
#endif  // L_EDITOR

    /// Update object loading.
    void ObjectLoader::Tick()
    {
        // Tick package loaders first.
        TickPackageLoaders();

        // Build the list of object load requests to update this tick, incrementing the request count on each to prevent
        // them from being released while we don't have a lock on the request hash map.
        HELIUM_ASSERT( m_loadRequestTickArray.IsEmpty() );

        ConcurrentHashMap< ObjectPath, LoadRequest* >::ConstAccessor loadRequestConstAccessor;
        if( m_loadRequestMap.First( loadRequestConstAccessor ) )
        {
            do
            {
                LoadRequest* pRequest = loadRequestConstAccessor->second;
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
                ConcurrentHashMap< ObjectPath, LoadRequest* >::Accessor loadRequestAccessor;
                if( m_loadRequestMap.Find( loadRequestAccessor, pRequest->path ) )
                {
                    pRequest = loadRequestAccessor->second;
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

        m_loadRequestTickArray.Resize( 0 );
    }

    /// Get the global object loader instance.
    ///
    /// An object loader instance must be initialized first through the interface of the ObjectLoader subclasses.
    ///
    /// @return  Object loader instance.  If an instance has not yet been initialized, this will return null.
    ///
    /// @see DestroyStaticInstance()
    ObjectLoader* ObjectLoader::GetStaticInstance()
    {
        return sm_pInstance;
    }

    /// Destroy the global object loader instance if one exists.
    ///
    /// @see GetStaticInstance()
    void ObjectLoader::DestroyStaticInstance()
    {
        delete sm_pInstance;
        sm_pInstance = NULL;
    }

    /// @fn PackageLoader* ObjectLoader::GetPackageLoader( ObjectPath path )
    /// Get the package loader to use for the specified object.
    ///
    /// @param[in] path  Object path.
    ///
    /// @return  Package loader to use for loading the specified object.

    /// @fn void ObjectLoader::TickPackageLoaders()
    /// Tick all package loaders for the current ObjectLoader tick.

    /// Perform work immediately prior to initiating resource precaching.
    ///
    /// @param[in] pObject         Object instance.
    /// @param[in] pPackageLoader  Package loader used to load the given object.
    void ObjectLoader::OnPrecacheReady( Object* /*pObject*/, PackageLoader* /*pPackageLoader*/ )
    {
    }

    /// Perform work upon completion of the load process for an object.
    ///
    /// @param[in] path            Object path.
    /// @param[in] pObject         Object instance (may be null if the object failed to load properly).
    /// @param[in] pPackageLoader  Package loader used to load the given object.
    void ObjectLoader::OnLoadComplete( ObjectPath /*path*/, Object* /*pObject*/, PackageLoader* /*pPackageLoader*/ )
    {
    }

    /// Update the given load request.
    ///
    /// @param[in] pRequest  Load request to update.
    ///
    /// @return  True if the load request has completed, false if it still requires time to process.
    bool ObjectLoader::TickLoadRequest( LoadRequest* pRequest )
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
    bool ObjectLoader::TickPreload( LoadRequest* pRequest )
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
            ObjectPath path = pRequest->path;
            pRequest->packageLoadRequestId = pPackageLoader->BeginLoadObject( path );
            if( IsInvalid( pRequest->packageLoadRequestId ) )
            {
                pRequest->spObject = Object::FindObject( path );
                Object* pObject = pRequest->spObject;
                if( pObject )
                {
                    HELIUM_TRACE(
                        TRACE_WARNING,
                        TXT( "ObjectLoader: Object \"%s\" is not serialized, but was found in memory.\n" ),
                        *path.ToString() );

                    // Make sure the object is preloaded and linked, but still perform resource caching and load
                    // finalization if necessary.
                    pObject->SetFlags( Object::FLAG_PRELOADED | Object::FLAG_LINKED );

                    AtomicOrRelease( pRequest->stateFlags, LOAD_FLAG_PRELOADED | LOAD_FLAG_LINKED );

                    return true;
                }

                HELIUM_TRACE(
                    TRACE_ERROR,
                    TXT( "ObjectLoader: Object \"%s\" is not serialized and does not exist in memory.\n" ),
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

    /// Update object reference linking for the given object load request.
    ///
    /// @param[in] pRequest  Load request to update.
    ///
    /// @return  True if linking still requires processing, false if it is complete.
    bool ObjectLoader::TickLink( LoadRequest* pRequest )
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
        Object* pObject = pRequest->spObject;
        if( pObject )
        {
            uint32_t objectFlags = pObject->GetFlags();
            if( !( objectFlags & Object::FLAG_LINKED ) )
            {
                if( !( objectFlags & Object::FLAG_BROKEN ) )
                {
                    Linker linker;
                    linker.Prepare( rLinkTable.GetData(), static_cast< uint32_t >( linkTableSize ) );
                    if( !linker.Serialize( pObject ) )
                    {
                        pObject->SetFlags( Object::FLAG_BROKEN );
                    }
                }

                pObject->SetFlags( Object::FLAG_LINKED );
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
    bool ObjectLoader::TickPrecache( LoadRequest* pRequest )
    {
        HELIUM_ASSERT( pRequest );
        HELIUM_ASSERT( !( pRequest->stateFlags & LOAD_FLAG_LOADED ) );

        Object* pObject = pRequest->spObject;
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

            // Perform any pre-precaching work.
            OnPrecacheReady( pObject, pRequest->pPackageLoader );

            if( !pObject->GetAnyFlagSet( Object::FLAG_BROKEN ) && pObject->NeedsPrecacheResourceData() )
            {
                if( !( pRequest->stateFlags & LOAD_FLAG_PRECACHE_STARTED ) )
                {
                    if( !pObject->BeginPrecacheResourceData() )
                    {
                        HELIUM_TRACE(
                            TRACE_ERROR,
                            TXT( "ObjectLoader: Failed to begin precaching object \"%s\".\n" ),
                            *pObject->GetPath().ToString() );

                        pObject->SetFlags( Object::FLAG_PRECACHED | Object::FLAG_BROKEN );
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

            pObject->SetFlags( Object::FLAG_PRECACHED );
        }

        AtomicOrRelease( pRequest->stateFlags, LOAD_FLAG_PRECACHED );

        return true;
    }

    /// Update loading finalization for the given object load request.
    ///
    /// @param[in] pRequest  Load request to update.
    ///
    /// @return  True if load finalization has completed, false if not.
    bool ObjectLoader::TickFinalizeLoad( LoadRequest* pRequest )
    {
        HELIUM_ASSERT( pRequest );

        Object* pObject = pRequest->spObject;
        if( pObject )
        {
            pObject->ConditionalFinalizeLoad();
        }

        // Loading now complete.
        OnLoadComplete( pRequest->path, pObject, pRequest->pPackageLoader );
        AtomicOrRelease( pRequest->stateFlags, LOAD_FLAG_LOADED );

        return true;
    }

    /// Constructor.
    ObjectLoader::Linker::Linker()
        : m_pLinkEntries( NULL )
        , m_linkEntryCount( 0 )
        , m_bError( false )
    {
    }

    /// Destructor.
    ObjectLoader::Linker::~Linker()
    {
    }

    /// Prepare for linking object references based on the given link table.
    ///
    /// @param[in] pLinkEntries    Array of link table entries.
    /// @param[in] linkEntryCount  Number of entries in the link table.
    void ObjectLoader::Linker::Prepare( const LinkEntry* pLinkEntries, uint32_t linkEntryCount )
    {
        HELIUM_ASSERT( pLinkEntries || linkEntryCount == 0 );

        m_pLinkEntries = pLinkEntries;
        m_linkEntryCount = linkEntryCount;
    }

    /// @copydoc Serializer::Serialize()
    bool ObjectLoader::Linker::Serialize( Object* pObject )
    {
        HELIUM_ASSERT( pObject );

        HELIUM_TRACE( TRACE_DEBUG, TXT( "ObjectLoader::Linker: Linking \"%s\".\n" ), *pObject->GetPath().ToString() );

        HELIUM_ASSERT( m_pLinkEntries || m_linkEntryCount == 0 );

        m_bError = false;
        pObject->Serialize( *this );

        return !m_bError;
    }

    /// @copydoc Serializer::GetMode()
    Serializer::EMode ObjectLoader::Linker::GetMode() const
    {
        return MODE_LINK;
    }

    /// @copydoc Serializer::SerializeTag()
    void ObjectLoader::Linker::SerializeTag( const Tag& /*rTag*/ )
    {
    }

    /// @copydoc Serializer::CanResolveTags()
    bool ObjectLoader::Linker::CanResolveTags() const
    {
        return false;
    }

    /// @name Serializer::SerializeBool()
    void ObjectLoader::Linker::SerializeBool( bool& /*rValue*/ )
    {
    }

    /// @name Serializer::SerializeInt8()
    void ObjectLoader::Linker::SerializeInt8( int8_t& /*rValue*/ )
    {
    }

    /// @name Serializer::SerializeUint8()
    void ObjectLoader::Linker::SerializeUint8( uint8_t& /*rValue*/ )
    {
    }

    /// @name Serializer::SerializeInt16()
    void ObjectLoader::Linker::SerializeInt16( int16_t& /*rValue*/ )
    {
    }

    /// @name Serializer::SerializeUint16()
    void ObjectLoader::Linker::SerializeUint16( uint16_t& /*rValue*/ )
    {
    }

    /// @name Serializer::SerializeInt32()
    void ObjectLoader::Linker::SerializeInt32( int32_t& /*rValue*/ )
    {
    }

    /// @name Serializer::SerializeUint32()
    void ObjectLoader::Linker::SerializeUint32( uint32_t& /*rValue*/ )
    {
    }

    /// @name Serializer::SerializeInt64()
    void ObjectLoader::Linker::SerializeInt64( int64_t& /*rValue*/ )
    {
    }

    /// @name Serializer::SerializeUint64()
    void ObjectLoader::Linker::SerializeUint64( uint64_t& /*rValue*/ )
    {
    }

    /// @name Serializer::SerializeFloat32()
    void ObjectLoader::Linker::SerializeFloat32( float32_t& /*rValue*/ )
    {
    }

    /// @name Serializer::SerializeFloat64()
    void ObjectLoader::Linker::SerializeFloat64( float64_t& /*rValue*/ )
    {
    }

    /// @name Serializer::SerializeBuffer()
    void ObjectLoader::Linker::SerializeBuffer( void* /*pBuffer*/, size_t /*elementSize*/, size_t /*count*/ )
    {
    }

    /// @name Serializer::SerializeEnum()
    void ObjectLoader::Linker::SerializeEnum(
        int32_t& /*rValue*/,
        uint32_t /*nameCount*/,
        const tchar_t* const* /*ppNames*/ )
    {
    }

    /// @name Serializer::SerializeCharName()
    void ObjectLoader::Linker::SerializeCharName( CharName& /*rValue*/ )
    {
    }

    /// @name Serializer::SerializeWideName()
    void ObjectLoader::Linker::SerializeWideName( WideName& /*rValue*/ )
    {
    }

    /// @name Serializer::SerializeCharString()
    void ObjectLoader::Linker::SerializeCharString( CharString& /*rValue*/ )
    {
    }

    /// @name Serializer::SerializeWideString()
    void ObjectLoader::Linker::SerializeWideString( WideString& /*rValue*/ )
    {
    }

    /// @name Serializer::SerializeObjectReference()
    void ObjectLoader::Linker::SerializeObjectReference( Type* pType, ObjectPtr& rspObject )
    {
        HELIUM_ASSERT( pType );

        uint32_t linkIndex = rspObject.GetLinkIndex();
        rspObject.ClearLinkIndex();

        if( IsInvalid( linkIndex ) )
        {
            return;
        }

        if( linkIndex >= m_linkEntryCount )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "ObjectLoader: Invalid link index %" ) TPRIu32 TXT( " encountered.  Setting null reference.\n" ),
                linkIndex );

            m_bError = true;

            return;
        }

        Object* pObject = m_pLinkEntries[ linkIndex ].spObject;
        if( pObject )
        {
            if( !pObject->IsA( pType ) )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    TXT( "ObjectLoader: Object reference \"%s\" is not of the correct type (\"%s\").\n" ),
                    *pObject->GetPath().ToString(),
                    *pType->GetPath().ToString() );

                m_bError = true;
            }
            else
            {
                rspObject = pObject;
            }
        }
    }
}
