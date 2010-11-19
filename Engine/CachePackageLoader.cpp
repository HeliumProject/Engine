//----------------------------------------------------------------------------------------------------------------------
// CachePackageLoader.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/CachePackageLoader.h"

#include "Core/AsyncLoader.h"
#include "Engine/BinaryDeserializer.h"
#include "Engine/CacheManager.h"
#include "Engine/GameObjectLoader.h"
#include "Engine/NullLinker.h"
#include "Engine/Resource.h"

namespace Lunar
{
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
                TRACE_ERROR,
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
    size_t CachePackageLoader::BeginLoadObject( GameObjectPath path )
    {
        HELIUM_ASSERT( m_pCache );

        // Don't load packages from the cache, but instead create them dynamically.
        if( path.IsPackage() )
        {
            HELIUM_TRACE(
                TRACE_DEBUG,
                TXT( "CachePackageLoader::BeginLoadObject(): \"%s\" is a package, resolving immediately.\n" ),
                *path.ToString() );

            LoadRequest* pRequest = m_loadRequestPool.Allocate();
            HELIUM_ASSERT( pRequest );
            pRequest->pEntry = NULL;

            ResolvePackage( pRequest->spObject, path );
            HELIUM_ASSERT( pRequest->spObject );

            SetInvalid( pRequest->asyncLoadId );
            pRequest->pAsyncLoadBuffer = NULL;
            pRequest->pSerializedData = NULL;
            pRequest->pPropertyStreamEnd = NULL;
            pRequest->pPersistentResourceStreamEnd = NULL;
            HELIUM_ASSERT( pRequest->linkTable.IsEmpty() );
            HELIUM_ASSERT( !pRequest->spType );
            HELIUM_ASSERT( !pRequest->spTemplate );
            HELIUM_ASSERT( !pRequest->spOwner );
            SetInvalid( pRequest->typeLinkIndex );
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
                TRACE_DEBUG,
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
                    TRACE_ERROR,
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
        HELIUM_ASSERT( pRequest->linkTable.IsEmpty() );
        HELIUM_ASSERT( !pRequest->spType );
        HELIUM_ASSERT( !pRequest->spTemplate );
        HELIUM_ASSERT( !pRequest->spOwner );
        SetInvalid( pRequest->typeLinkIndex );
        SetInvalid( pRequest->templateLinkIndex );
        SetInvalid( pRequest->ownerLinkIndex );

        pRequest->flags = 0;

        // If a fully-loaded object already exists with the same name, do not attempt to re-load the object (just mark
        // the request as complete).
        pRequest->spObject = GameObject::FindObject( pEntry->path );

        GameObject* pObject = pRequest->spObject;
        if( pObject && pObject->IsFullyLoaded() )
        {
            HELIUM_TRACE(
                TRACE_DEBUG,
                ( TXT( "CachePackageLoader::BeginLoadObject(): \"%s\" is already fully loaded.  Bypassing load " )
                  TXT( "process.\n" ) ),
                *path.ToString() );

            pRequest->flags = LOAD_FLAG_PRELOADED;
        }
        else
        {
            HELIUM_ASSERT( !pObject || !pObject->GetAnyFlagSet( GameObject::FLAG_LOADED | GameObject::FLAG_LINKED ) );

            HELIUM_TRACE(
                TRACE_DEBUG,
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
            TRACE_DEBUG,
            ( TXT( "CachePackageLoader::BeginLoadObject(): Load request for \"%s\" added (ID: %" ) TPRIuSZ
              TXT( ").\n" ) ),
            *path.ToString(),
            requestId );

        return requestId;
    }

    /// @copydoc PackageLoader::TryFinishLoadObject()
    bool CachePackageLoader::TryFinishLoadObject(
        size_t requestId,
        GameObjectPtr& rspObject,
        DynArray< GameObjectLoader::LinkEntry >& rLinkTable )
    {
        HELIUM_ASSERT( requestId < m_loadRequests.GetSize() );
        HELIUM_ASSERT( m_loadRequests.IsElementValid( requestId ) );

        LoadRequest* pRequest = m_loadRequests[ requestId ];
        HELIUM_ASSERT( pRequest );
        if( !( pRequest->flags & LOAD_FLAG_PRELOADED ) )
        {
            return false;
        }

        // Sync on type, template, and owner dependencies.
        GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
        HELIUM_ASSERT( pObjectLoader );

        DynArray< size_t >& rInternalLinkTable = pRequest->linkTable;

        if( IsValid( pRequest->typeLinkIndex ) )
        {
            size_t linkLoadId = rInternalLinkTable[ pRequest->typeLinkIndex ];
            if( IsValid( linkLoadId ) && !pObjectLoader->TryFinishLoad( linkLoadId, pRequest->spType ) )
            {
                return false;
            }

            SetInvalid( pRequest->typeLinkIndex );
        }

        if( IsValid( pRequest->templateLinkIndex ) )
        {
            size_t linkLoadId = rInternalLinkTable[ pRequest->templateLinkIndex ];
            if( IsValid( linkLoadId ) && !pObjectLoader->TryFinishLoad( linkLoadId, pRequest->spTemplate ) )
            {
                return false;
            }

            SetInvalid( pRequest->templateLinkIndex );
        }

        if( IsValid( pRequest->ownerLinkIndex ) )
        {
            size_t linkLoadId = rInternalLinkTable[ pRequest->ownerLinkIndex ];
            if( IsValid( linkLoadId ) && !pObjectLoader->TryFinishLoad( linkLoadId, pRequest->spOwner ) )
            {
                return false;
            }

            SetInvalid( pRequest->ownerLinkIndex );
        }

        rspObject = pRequest->spObject;
        GameObject* pObject = rspObject;
        if( pObject && ( pRequest->flags & LOAD_FLAG_ERROR ) )
        {
            pObject->SetFlags( GameObject::FLAG_BROKEN );
        }

        pRequest->spObject.Release();

        size_t linkTableSize = rInternalLinkTable.GetSize();
        rLinkTable.Resize( 0 );
        rLinkTable.Reserve( linkTableSize );
        for( size_t linkIndex = 0; linkIndex < linkTableSize; ++linkIndex )
        {
            GameObjectLoader::LinkEntry* pEntry = rLinkTable.New();
            HELIUM_ASSERT( pEntry );
            pEntry->loadId = rInternalLinkTable[ linkIndex ];
            pEntry->spObject.Release();
        }

        rInternalLinkTable.Resize( 0 );

        HELIUM_ASSERT( IsInvalid( pRequest->asyncLoadId ) );
        HELIUM_ASSERT( !pRequest->pAsyncLoadBuffer );

        pRequest->spType.Release();
        pRequest->spTemplate.Release();
        pRequest->spOwner.Release();

        HELIUM_ASSERT( pObject || pRequest->pEntry );
        HELIUM_TRACE(
            TRACE_DEBUG,
            ( TXT( "CachePackageLoader::TryFinishLoadObject(): Load request for \"%s\" (ID: %" ) TPRIuSZ TXT( ") " )
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
    GameObjectPath CachePackageLoader::GetObjectPath( size_t index ) const
    {
        HELIUM_ASSERT( m_pCache );
        HELIUM_ASSERT( index < m_pCache->GetEntryCount() );

        const Cache::Entry& rEntry = m_pCache->GetEntry( static_cast< uint32_t >( index ) );

        return rEntry.path;
    }

    /// @copydoc PackageLoader::IsSourcePackageFile()
    bool CachePackageLoader::IsSourcePackageFile() const
    {
        return false;
    }

    /// @copydoc PackageLoader::GetFileTimestamp()
    int64_t CachePackageLoader::GetFileTimestamp() const
    {
        return INT64_MIN;
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
                TRACE_ERROR,
                TXT( "CachePackageLoader: Failed to read cache data for object \"%s\".\n" ),
                *pRequest->pEntry->path.ToString() );
        }
        else
        {
            uint8_t* pBufferEnd = pRequest->pAsyncLoadBuffer + bytesRead;
            pRequest->pPropertyStreamEnd = pBufferEnd;
            pRequest->pPersistentResourceStreamEnd = pBufferEnd;

            if( DeserializeLinkTable( pRequest ) )
            {
                return true;
            }
        }

        // An error occurred attempting to load the property data, so mark any existing object as fully loaded (nothing
        // else will be done with the object itself from here on out).
        DefaultAllocator().Free( pRequest->pAsyncLoadBuffer );
        pRequest->pAsyncLoadBuffer = NULL;

        GameObject* pObject = pRequest->spObject;
        if( pObject )
        {
            pObject->SetFlags( GameObject::FLAG_PRELOADED | GameObject::FLAG_LINKED );
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

        GameObject* pObject = pRequest->spObject;

        const Cache::Entry* pCacheEntry = pRequest->pEntry;
        HELIUM_ASSERT( pCacheEntry );

        // Wait for the type, template, and owner objects to load.
        GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
        HELIUM_ASSERT( pObjectLoader );

        if( IsValid( pRequest->typeLinkIndex ) )
        {
            HELIUM_ASSERT( pRequest->typeLinkIndex < pRequest->linkTable.GetSize() );
            size_t typeLoadId = pRequest->linkTable[ pRequest->typeLinkIndex ];
            if( IsValid( typeLoadId ) && !pObjectLoader->TryFinishLoad( typeLoadId, pRequest->spType ) )
            {
                return false;
            }

            SetInvalid( pRequest->typeLinkIndex );
        }

        Type* pType = DynamicCast< Type >( pRequest->spType.Get() );
        if( !pType )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "CachePackageLoader: Failed to load type object for \"%s\".\n" ),
                *pCacheEntry->path.ToString() );

            if( pObject )
            {
                pObject->SetFlags( GameObject::FLAG_PRELOADED | GameObject::FLAG_LINKED );
                pObject->ConditionalFinalizeLoad();
            }

            DefaultAllocator().Free( pRequest->pAsyncLoadBuffer );
            pRequest->pAsyncLoadBuffer = NULL;

            pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;

            return true;
        }

        if( IsValid( pRequest->templateLinkIndex ) )
        {
            HELIUM_ASSERT( pRequest->templateLinkIndex < pRequest->linkTable.GetSize() );
            size_t templateLoadId = pRequest->linkTable[ pRequest->templateLinkIndex ];
            if( IsValid( templateLoadId ) && !pObjectLoader->TryFinishLoad( templateLoadId, pRequest->spTemplate ) )
            {
                return false;
            }

            SetInvalid( pRequest->templateLinkIndex );

            if( !pRequest->spTemplate )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    TXT( "CachePackageLoader: Failed to load template object for \"%s\".\n" ),
                    *pCacheEntry->path.ToString() );

                if( pObject )
                {
                    pObject->SetFlags( GameObject::FLAG_PRELOADED | GameObject::FLAG_LINKED );
                    pObject->ConditionalFinalizeLoad();
                }

                DefaultAllocator().Free( pRequest->pAsyncLoadBuffer );
                pRequest->pAsyncLoadBuffer = NULL;

                pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;

                return true;
            }
        }

        GameObject* pTemplate = pRequest->spTemplate;

        if( IsValid( pRequest->ownerLinkIndex ) )
        {
            HELIUM_ASSERT( pRequest->ownerLinkIndex < pRequest->linkTable.GetSize() );
            size_t ownerLoadId = pRequest->linkTable[ pRequest->ownerLinkIndex ];
            if( IsValid( ownerLoadId ) && !pObjectLoader->TryFinishLoad( ownerLoadId, pRequest->spOwner ) )
            {
                return false;
            }

            SetInvalid( pRequest->ownerLinkIndex );

            if( !pRequest->spOwner )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    TXT( "CachePackageLoader: Failed to load owner object for \"%s\".\n" ),
                    *pCacheEntry->path.ToString() );

                if( pObject )
                {
                    pObject->SetFlags( GameObject::FLAG_PRELOADED | GameObject::FLAG_LINKED );
                    pObject->ConditionalFinalizeLoad();
                }

                DefaultAllocator().Free( pRequest->pAsyncLoadBuffer );
                pRequest->pAsyncLoadBuffer = NULL;

                pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;

                return true;
            }
        }

        GameObject* pOwner = pRequest->spOwner;

        HELIUM_ASSERT( pType->IsFullyLoaded() );
        HELIUM_ASSERT( !pOwner || pOwner->IsFullyLoaded() );
        HELIUM_ASSERT( !pTemplate || pTemplate->IsFullyLoaded() );

        // If we already had an existing object, make sure the type and template match.
        if( pObject )
        {
            Type* pExistingType = pObject->GetType();
            HELIUM_ASSERT( pExistingType );
            if( pExistingType != pType )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "CachePackageLoader: Cannot load \"%s\" using the existing object as the types do not " )
                      TXT( "match (existing type: \"%s\"; serialized type: \"%s\".\n" ) ),
                    *pCacheEntry->path.ToString(),
                    *pExistingType->GetPath().ToString(),
                    *pType->GetPath().ToString() );

                pObject->SetFlags( GameObject::FLAG_PRELOADED | GameObject::FLAG_LINKED );
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
            pRequest->spObject = GameObject::CreateObject( pType, pCacheEntry->path.GetName(), pOwner, pTemplate );
            pObject = pRequest->spObject;
            if( !pObject )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    TXT( "CachePackageLoader: Failed to create \"%s\" during loading.\n" ),
                    *pCacheEntry->path.ToString() );

                DefaultAllocator().Free( pRequest->pAsyncLoadBuffer );
                pRequest->pAsyncLoadBuffer = NULL;

                pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;

                return true;
            }
        }

        // Load the object properties.
        BinaryDeserializer deserializer;
        deserializer.Prepare(
            pRequest->pSerializedData,
            static_cast< size_t >( pRequest->pPropertyStreamEnd - pRequest->pSerializedData ) );

        if( !deserializer.Serialize( pObject ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "CachePackageLoader: Failed to deserialize object \"%s\".\n" ),
                *pCacheEntry->path.ToString() );

            // Clear out object references (object can now be considered fully loaded as well).
            NullLinker().Serialize( pObject );
            pObject->SetFlags( GameObject::FLAG_LINKED );
            pObject->ConditionalFinalizeLoad();

            pRequest->flags |= LOAD_FLAG_ERROR;
        }
        else
        {
            // Load persistent resource data.
            Resource* pResource = DynamicCast< Resource >( pObject );
            if( pResource )
            {
                deserializer.Prepare(
                    pRequest->pPropertyStreamEnd,
                    static_cast< size_t >( pRequest->pPersistentResourceStreamEnd - pRequest->pPropertyStreamEnd ) );

                deserializer.BeginSerialize();
                pResource->SerializePersistentResourceData( deserializer );
                if( !deserializer.EndSerialize() )
                {
                    HELIUM_TRACE(
                        TRACE_ERROR,
                        ( TXT( "CachePackageLoader: End of stream reached when deserializing persistent resource " )
                          TXT( "data for \"%s\".\n" ) ),
                        *pCacheEntry->path.ToString() );
                }
            }
        }

        DefaultAllocator().Free( pRequest->pAsyncLoadBuffer );
        pRequest->pAsyncLoadBuffer = NULL;

        pObject->SetFlags( GameObject::FLAG_PRELOADED );

        pRequest->flags |= LOAD_FLAG_PRELOADED;

        // GameObject is now preloaded.
        return true;
    }

    /// Recursive function for resolving a package request.
    ///
    /// @param[out] rspPackage   Resolved package.
    /// @param[in]  packagePath  Package object path.
    void CachePackageLoader::ResolvePackage( GameObjectPtr& rspPackage, GameObjectPath packagePath )
    {
        HELIUM_ASSERT( !packagePath.IsEmpty() );

        rspPackage = GameObject::FindObject( packagePath );
        if( !rspPackage )
        {
            GameObjectPtr spParent;
            GameObjectPath parentPath = packagePath.GetParent();
            if( !parentPath.IsEmpty() )
            {
                ResolvePackage( spParent, parentPath );
                HELIUM_ASSERT( spParent );
            }

            rspPackage = GameObject::Create< Package >( packagePath.GetName(), spParent );
            HELIUM_ASSERT( rspPackage );
        }

        rspPackage->SetFlags( GameObject::FLAG_PRELOADED | GameObject::FLAG_LINKED | GameObject::FLAG_LOADED );
    }

    /// Deserialize the link table for an object load.
    ///
    /// @param[in] pRequest  Load request data.
    bool CachePackageLoader::DeserializeLinkTable( LoadRequest* pRequest )
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
                TRACE_ERROR,
                TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
                *pRequest->pEntry->path.ToString() );

            return false;
        }

        MemoryCopy( &propertyStreamSize, pBufferCurrent, sizeof( propertyStreamSize ) );
        pBufferCurrent += sizeof( propertyStreamSize );

        if( propertyStreamSize > static_cast< size_t >( pPropertyStreamEnd - pBufferCurrent ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "CachePackageLoader: Property stream size (%" ) TPRIu32 TXT( " bytes) for \"%s\" exceeds the " )
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

        uint32_t linkTableSize = 0;
        if( pBufferCurrent + sizeof( linkTableSize ) > pPropertyStreamEnd )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
                *pRequest->pEntry->path.ToString() );

            return false;
        }

        MemoryCopy( &linkTableSize, pBufferCurrent, sizeof( linkTableSize ) );
        pBufferCurrent += sizeof( linkTableSize );

        pRequest->linkTable.Resize( 0 );
        pRequest->linkTable.Reserve( linkTableSize );

        StackMemoryHeap<>& rStackHeap = ThreadLocalStackAllocator::GetMemoryHeap();
        StackMemoryHeap<>::Marker stackMarker( rStackHeap );

        // Track the link table object paths so that we can use them for issuing additonal load requests for the object
        // type, template, and owner dependencies (this way, we can sync on those load requests during the preload
        // process while still providing load requests for the caller to resolve if necessary).
        GameObjectPath* pLinkTablePaths = static_cast< GameObjectPath* >( rStackHeap.Allocate(
            sizeof( GameObjectPath ) * linkTableSize ) );
        HELIUM_ASSERT( pLinkTablePaths );
        ArrayUninitializedFill( pLinkTablePaths, GameObjectPath( NULL_NAME ), linkTableSize );

        GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
        HELIUM_ASSERT( pObjectLoader );

        uint_fast32_t linkTableSizeFast = linkTableSize;
        for( uint_fast32_t linkTableIndex = 0; linkTableIndex < linkTableSizeFast; ++linkTableIndex )
        {
            uint32_t pathStringSize;
            if( pBufferCurrent + sizeof( pathStringSize ) > pPropertyStreamEnd )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
                    *pRequest->pEntry->path.ToString() );

                return false;
            }

            MemoryCopy( &pathStringSize, pBufferCurrent, sizeof( pathStringSize ) );
            pBufferCurrent += sizeof( pathStringSize );

            if( pBufferCurrent + sizeof( tchar_t ) * pathStringSize > pPropertyStreamEnd )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
                    *pRequest->pEntry->path.ToString() );

                return false;
            }

            StackMemoryHeap<>::Marker stackMarker( rStackHeap );
            tchar_t* pPathString = static_cast< tchar_t* >( rStackHeap.Allocate(
                sizeof( tchar_t ) * ( pathStringSize + 1 ) ) );
            HELIUM_ASSERT( pPathString );

            MemoryCopy( pPathString, pBufferCurrent, sizeof( tchar_t ) * pathStringSize );
            pBufferCurrent += sizeof( tchar_t ) * pathStringSize;

            pPathString[ pathStringSize ] = TXT( '\0' );

            size_t linkLoadId;
            SetInvalid( linkLoadId );

            GameObjectPath path;
            if( !path.Set( pPathString ) )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "CachePackageLoader: Invalid object path \"%s\" found in linker table when deserializing " )
                      TXT( "\"%s\".  Setting to null.\n" ) ),
                    pPathString,
                    *pRequest->pEntry->path.ToString() );

                pRequest->flags |= LOAD_FLAG_ERROR;
            }
            else
            {
                pLinkTablePaths[ linkTableIndex ] = path;

                // Begin loading the link table entry.
                linkLoadId = pObjectLoader->BeginLoadObject( path );
                if( IsInvalid( linkLoadId ) )
                {
                    HELIUM_TRACE(
                        TRACE_ERROR,
                        ( TXT( "CachePackageLoader: Failed to begin loading \"%s\" as a link dependency for \"%s\".  " )
                          TXT( "Setting to null.\n" ) ),
                        pPathString,
                        *pRequest->pEntry->path.ToString() );

                    pRequest->flags |= LOAD_FLAG_ERROR;
                }
            }

            pRequest->linkTable.Push( linkLoadId );
        }

        // Read the type link information.
        if( pBufferCurrent + sizeof( pRequest->typeLinkIndex ) > pPropertyStreamEnd )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
                *pRequest->pEntry->path.ToString() );

            return false;
        }

        MemoryCopy( &pRequest->typeLinkIndex, pBufferCurrent, sizeof( pRequest->typeLinkIndex ) );
        pBufferCurrent += sizeof( pRequest->typeLinkIndex );

        if( pRequest->typeLinkIndex >= linkTableSizeFast )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "CachePackageLoader: Invalid link table index for the type of \"%s\".\n" ),
                *pRequest->pEntry->path.ToString() );

            SetInvalid( pRequest->typeLinkIndex );

            return false;
        }

        size_t typeLoadId = pObjectLoader->BeginLoadObject( pLinkTablePaths[ pRequest->typeLinkIndex ] );
        HELIUM_ASSERT( typeLoadId == pRequest->linkTable[ pRequest->typeLinkIndex ] );
        HELIUM_UNREF( typeLoadId );

        // Read the template link information.
        if( pBufferCurrent + sizeof( pRequest->templateLinkIndex ) > pPropertyStreamEnd )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
                *pRequest->pEntry->path.ToString() );

            return false;
        }

        MemoryCopy( &pRequest->templateLinkIndex, pBufferCurrent, sizeof( pRequest->templateLinkIndex ) );
        pBufferCurrent += sizeof( pRequest->templateLinkIndex );

        if( IsValid( pRequest->templateLinkIndex ) )
        {
            if( pRequest->templateLinkIndex >= linkTableSizeFast )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    TXT( "CachePackageLoader: Invalid link table index for the template of \"%s\".\n" ),
                    *pRequest->pEntry->path.ToString() );

                SetInvalid( pRequest->templateLinkIndex );

                return false;
            }

            size_t templateLoadId = pObjectLoader->BeginLoadObject( pLinkTablePaths[ pRequest->templateLinkIndex ] );
            HELIUM_ASSERT( templateLoadId == pRequest->linkTable[ pRequest->templateLinkIndex ] );
            HELIUM_UNREF( templateLoadId );
        }

        // Read the owner link information.
        if( pBufferCurrent + sizeof( pRequest->ownerLinkIndex ) > pPropertyStreamEnd )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "CachePackageLoader: End of buffer reached when attempting to deserialize \"%s\".\n" ),
                *pRequest->pEntry->path.ToString() );

            return false;
        }

        MemoryCopy( &pRequest->ownerLinkIndex, pBufferCurrent, sizeof( pRequest->ownerLinkIndex ) );
        pBufferCurrent += sizeof( pRequest->ownerLinkIndex );

        if( IsValid( pRequest->ownerLinkIndex ) )
        {
            if( pRequest->ownerLinkIndex >= linkTableSizeFast )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    TXT( "CachePackageLoader: Invalid link table index for the owner of \"%s\".\n" ),
                    *pRequest->pEntry->path.ToString() );

                SetInvalid( pRequest->ownerLinkIndex );

                return false;
            }

            size_t ownerLoadId = pObjectLoader->BeginLoadObject( pLinkTablePaths[ pRequest->ownerLinkIndex ] );
            HELIUM_ASSERT( ownerLoadId == pRequest->linkTable[ pRequest->ownerLinkIndex ] );
            HELIUM_UNREF( ownerLoadId );
        }

        pRequest->pSerializedData = pBufferCurrent;

        return true;
    }
}
