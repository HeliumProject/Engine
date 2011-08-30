//----------------------------------------------------------------------------------------------------------------------
// Resource.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/Resource.h"

#include "Foundation/AsyncLoader.h"
#include "Engine/CacheManager.h"

using namespace Helium;

HELIUM_IMPLEMENT_OBJECT( Resource, Engine, 0 );

/// Constructor.
Resource::Resource()
{
#if HELIUM_TOOLS
    for( size_t preprocessedDataIndex = 0;
        preprocessedDataIndex < HELIUM_ARRAY_COUNT( m_preprocessedData );
        ++preprocessedDataIndex )
    {
        m_preprocessedData[ preprocessedDataIndex ].bLoaded = false;
    }
#endif
}

/// Destructor.
Resource::~Resource()
{
}

/// Serialize the persistent data for this resource using the given serializer.
///
/// @param[in] s  Serializer with which to serialize.
void Resource::SerializePersistentResourceData( Serializer& /*s*/ )
{
}

/// Get the name of the resource cache to use for this resource.
///
/// @return  Resource cache name.
Name Resource::GetCacheName() const
{
    return Name( NULL_NAME );
}

/// Get the size of the specified sub-data of this resource.
///
/// @param[in] subDataIndex  Resource sub-data index.
///
/// @return  Sub-data size, or an invalid index if the resource data could not be located.
///
/// @see BeginLoadSubData(), TryLoadSubData()
size_t Resource::GetSubDataSize( uint32_t subDataIndex ) const
{
    CacheManager& rCacheManager = CacheManager::GetStaticInstance();

#if HELIUM_TOOLS
    // Check for in-memory data first.
    Cache::EPlatform platform = rCacheManager.GetCurrentPlatform();
    const PreprocessedData& rPreprocessedData = GetPreprocessedData( platform );
    if( rPreprocessedData.bLoaded )
    {
        const DynArray< DynArray< uint8_t > >& rSubDataBuffers = rPreprocessedData.subDataBuffers;

        return ( subDataIndex < rSubDataBuffers.GetSize()
            ? rSubDataBuffers[ subDataIndex ].GetSize()
            : Invalid< size_t >() );
    }
#endif

    // Search for the sub-data in this resource's cache.
    Name cacheName = GetCacheName();
    HELIUM_ASSERT( !cacheName.IsEmpty() );

    Cache* pCache = rCacheManager.GetCache( cacheName );
    HELIUM_ASSERT( pCache );
    pCache->EnforceTocLoad();

    GameObjectPath resourcePath = GetPath();
    const Cache::Entry* pCacheEntry = pCache->FindEntry( resourcePath, subDataIndex );

    return ( pCacheEntry ? pCacheEntry->size : Invalid< size_t >() );
}

/// Begin asynchronous loading of the specified resource sub-data.
///
/// @param[in] pBuffer       Buffer in which to load the resource sub-data.  This must be at least as large as the
///                          size returned by GetSubDataSize()
/// @param[in] subDataIndex  Resource sub-data index.
/// @param[in] loadSizeMax   Maximum load request size.  If the actual cached resource sub-data is larger than this,
///                          the amount of bytes actually loaded will be clamped.
///
/// @return  ID associated with the load request, or an invalid index if the load request failed to be queued.
///
/// @see TryFinishLoadSubData(), GetSubDataSize()
size_t Resource::BeginLoadSubData( void* pBuffer, uint32_t subDataIndex, size_t loadSizeMax )
{
    HELIUM_ASSERT( pBuffer );

    CacheManager& rCacheManager = CacheManager::GetStaticInstance();

#if HELIUM_TOOLS
    // Check for in-memory data first.
    Cache::EPlatform platform = rCacheManager.GetCurrentPlatform();
    const PreprocessedData& rPreprocessedData = GetPreprocessedData( platform );
    if( rPreprocessedData.bLoaded )
    {
        const DynArray< DynArray< uint8_t > >& rSubDataBuffers = rPreprocessedData.subDataBuffers;
        if( subDataIndex >= rSubDataBuffers.GetSize() )
        {
            return Invalid< size_t >();
        }

        // Copy the sub-data immediately and assign a dummy ID.
        const DynArray< uint8_t >& rSubData = rSubDataBuffers[ subDataIndex ];

        size_t subDataSize = rSubData.GetSize();
        size_t copySize = Min( subDataSize, loadSizeMax );

        MemoryCopy( pBuffer, rSubData.GetData(), copySize );

        return static_cast< size_t >( -2 );
    }
#endif

    // Search for the sub-data in this resource's cache.
    Name cacheName = GetCacheName();
    HELIUM_ASSERT( !cacheName.IsEmpty() );

    Cache* pCache = rCacheManager.GetCache( cacheName );
    HELIUM_ASSERT( pCache );
    pCache->EnforceTocLoad();

    GameObjectPath resourcePath = GetPath();
    const Cache::Entry* pCacheEntry = pCache->FindEntry( resourcePath, subDataIndex );
    if( !pCacheEntry )
    {
        return Invalid< size_t >();
    }

    // Begin an asynchronous load.
    size_t subDataSize = pCacheEntry->size;
    size_t loadSize = Min( subDataSize, loadSizeMax );

    AsyncLoader& rAsyncLoader = AsyncLoader::GetStaticInstance();
    size_t loadId = rAsyncLoader.QueueRequest( pBuffer, pCache->GetCacheFileName(), pCacheEntry->offset, loadSize );

    return loadId;
}

/// Test for completion of an asynchronous sub-data load request.
///
/// @param[in] loadId  ID associated with the load request.
///
/// @return  True if the load request has completed, false if not.
bool Resource::TryFinishLoadSubData( size_t loadId )
{
    HELIUM_ASSERT( IsValid( loadId ) );

#if HELIUM_TOOLS
    // If the load request was an in-memory request, we don't need to sync as they are performed immediately.
    if( loadId == static_cast< size_t >( -2 ) )
    {
        return true;
    }
#endif

    // Check the async load request.
    AsyncLoader& rAsyncLoader = AsyncLoader::GetStaticInstance();

    size_t bytesRead;
    bool bFinished = rAsyncLoader.TrySyncRequest( loadId, bytesRead );

    return bFinished;
}
