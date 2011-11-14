//----------------------------------------------------------------------------------------------------------------------
// ObjectPreprocessor.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "PcSupportPch.h"
#include "PcSupport/ObjectPreprocessor.h"

#include "Platform/Stat.h"
#include "Foundation/Stream/BufferedStream.h"
#include "Foundation/Stream/DynamicMemoryStream.h"
#include "Foundation/Stream/ByteSwappingStream.h"
#include "Foundation/File/File.h"
#include "Foundation/File/Path.h"
#include "Foundation/Stream/FileStream.h"
#include "Engine/BinaryDeserializer.h"
#include "Engine/BinarySerializer.h"
#include "Engine/CacheManager.h"
#include "Engine/GameObjectLoader.h"
#include "Engine/Resource.h"
#include "PcSupport/PlatformPreprocessor.h"
#include "PcSupport/ResourceHandler.h"
#include "Foundation/Numeric.h"


using namespace Helium;

ObjectPreprocessor* ObjectPreprocessor::sm_pInstance = NULL;

/// Constructor.
ObjectPreprocessor::ObjectPreprocessor()
{
    MemoryZero( m_pPlatformPreprocessors, sizeof( m_pPlatformPreprocessors ) );
}

/// Destructor.
ObjectPreprocessor::~ObjectPreprocessor()
{
    for( size_t platformIndex = 0; platformIndex < HELIUM_ARRAY_COUNT( m_pPlatformPreprocessors ); ++platformIndex )
    {
        delete m_pPlatformPreprocessors[ platformIndex ];
    }
}

/// Set the platform preprocessor to use for caching objects and processing resources for a specific platform.
///
/// @param[in] platform       Target platform.
/// @param[in] pPreprocessor  Platform preprocessor to register.  Note that this will assume ownership of the
///                           preprocessor instance after setting, deleting it automatically once the object
///                           preprocessor is destroyed.
///
/// @see GetPlatformPreprocessor()
void ObjectPreprocessor::SetPlatformPreprocessor( Cache::EPlatform platform, PlatformPreprocessor* pPreprocessor )
{
    HELIUM_ASSERT( static_cast< size_t >( platform ) < static_cast< size_t >( Cache::PLATFORM_MAX ) );
    HELIUM_ASSERT( pPreprocessor );

    m_pPlatformPreprocessors[ platform ] = pPreprocessor;
}

/// Cache an object for all registered platforms.
///
/// @param[in] pObject                                 GameObject to cache.
/// @param[in] timestamp                               GameObject timestamp.
/// @param[in] bEvictPlatformPreprocessedResourceData  If the object being cached is a Resource-based object,
///                                                    specifying true will free the raw preprocessed resource data
///                                                    for the current platform after caching, while false will keep
///                                                    it in memory.  Typically, this data doesn't need to be kept
///                                                    around, as it will have already been deserialized into the
///                                                    resource itself, but certain resource preprocessors may want
///                                                    to keep this data intact.
///
/// @return  True if object caching was successful, false if not.
bool ObjectPreprocessor::CacheObject(
    GameObject* pObject,
    int64_t timestamp,
    bool bEvictPlatformPreprocessedResourceData )
{
#if HELIUM_TOOLS

    HELIUM_ASSERT( pObject );

    bool bCacheFailure = false;

    DynArray< uint8_t > objectStreamBuffer;

    Helium::DynamicMemoryStream directStream;
    Helium::ByteSwappingStream byteSwappingStream( &directStream );

    GameObjectPath objectPath = pObject->GetPath();

    // Only worry about resource data caching if the object is a Resource type that's not the default template
    // object for its specific type.
    Resource* pResource = ( !pObject->IsDefaultTemplate() ? Reflect::SafeCast< Resource >( pObject ) : NULL );

    CacheManager& rCacheManager = CacheManager::GetStaticInstance();

    GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
    HELIUM_ASSERT( pObjectLoader );
    Name objectCacheName = pObjectLoader->GetCacheName();

    bool bUpdatedAnyCache = false;

    for( size_t platformIndex = 0; platformIndex < HELIUM_ARRAY_COUNT( m_pPlatformPreprocessors ); ++platformIndex )
    {
        // Don't cache on platforms for which we don't have a preprocessor.
        PlatformPreprocessor* pPreprocessor = m_pPlatformPreprocessors[ platformIndex ];
        if( !pPreprocessor )
        {
            continue;
        }

        // Retrieve the cache for the current platform.
        Cache* pCache = rCacheManager.GetCache( objectCacheName, static_cast< Cache::EPlatform >( platformIndex ) );
        HELIUM_ASSERT( pCache );
        pCache->EnforceTocLoad();

        // Don't recache the object if an up-to-date cache entry already exists for it.
        const Cache::Entry* pEntry = pCache->FindEntry( objectPath, 0 );
        if( pEntry && pEntry->timestamp == timestamp )
        {
            continue;
        }

        HELIUM_TRACE(
            TRACE_INFO,
            TXT( "ObjectPreprocessor: Object \"%s\" is out of date.  Recaching...\n" ),
            *objectPath.ToString() );

        bUpdatedAnyCache = true;

        // Prepare for writing out the property and persistent resource data for the current platform.
        objectStreamBuffer.Resize( 0 );
        directStream.Open( &objectStreamBuffer );

        bool bSwapBytes = pPreprocessor->SwapBytes();
        Stream& rObjectStream =
            ( bSwapBytes ? static_cast< Stream& >( byteSwappingStream ) : static_cast< Stream& >( directStream ) );
        
        DynArray<uint8_t> data_buffer;
        Cache::WriteCacheObjectToBuffer(*pObject, data_buffer);

        if (!data_buffer.IsEmpty())
        {
            HELIUM_ASSERT(data_buffer.GetSize() <= Helium::NumericLimits<uint32_t>::Maximum);
            uint32_t data_size = static_cast<uint32_t>(data_buffer.GetSize());
            rObjectStream.Write(&data_size, sizeof(data_size), 1);
            rObjectStream.Write(&data_buffer[0], sizeof(data_buffer[0]), data_size);
        }
        else
        {
            uint32_t data_size = 0;
            rObjectStream.Write(&data_size, sizeof(data_size), 1);
        }
		
        // Serialize persistent resource data and the number of chunks of sub-data.
        if( pResource )
        {
            const Resource::PreprocessedData& rResourceData = pResource->GetPreprocessedData(
                static_cast< Cache::EPlatform >( platformIndex ) );
            if( !rResourceData.bLoaded )
            {
                HELIUM_TRACE(
                    TRACE_WARNING,
                    ( TXT( "ObjectPreprocessor::CacheObject(): Cannot cache resource data for \"%s\" for " )
                    TXT( "platform index %" ) TPRIuSZ TXT( " as the resource data is not in memory.  Make sure " )
                    TXT( "ObjectPreprocessor::LoadResourceData() has been called on the object prior to " )
                    TXT( "caching.\n" ) ),
                    *objectPath.ToString(),
                    platformIndex );
            }
            else
            {
                rObjectStream.Write(
                    rResourceData.persistentDataBuffer.GetData(),
                    1,
                    rResourceData.persistentDataBuffer.GetSize() );

                size_t subDataCountActual = rResourceData.subDataBuffers.GetSize();
                HELIUM_ASSERT( subDataCountActual <= UINT32_MAX );

                uint32_t subDataCount = static_cast< uint32_t >( subDataCountActual );
                rObjectStream.Write( &subDataCount, sizeof( subDataCount ), 1 );
            }
        }

        directStream.Close();

        // Cache the object data stream.
        size_t objectDataSize = objectStreamBuffer.GetSize();
        HELIUM_ASSERT( objectDataSize <= UINT32_MAX );

        bool bCacheResult = pCache->CacheEntry(
            objectPath,
            0,
            objectStreamBuffer.GetData(),
            timestamp,
            static_cast< uint32_t >( objectDataSize ) );
        if( !bCacheResult )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "ObjectPreprocessor: Failed to cache object \"%s\".\n" ),
                *objectPath.ToString() );

            bCacheFailure = true;
        }

        // Finish resource data caching.
        if( pResource )
        {
            Resource::PreprocessedData& rResourceData = pResource->GetPreprocessedData(
                static_cast< Cache::EPlatform >( platformIndex ) );
            if( rResourceData.bLoaded )
            {
                const DynArray< DynArray< uint8_t > >& rSubDataBuffers = rResourceData.subDataBuffers;
                size_t subDataBufferCount = rSubDataBuffers.GetSize();
                if( subDataBufferCount != 0 )
                {
                    // Cache resource sub-data.
                    Name resourceCacheName = pResource->GetCacheName();
                    HELIUM_ASSERT( !resourceCacheName.IsEmpty() );

                    Cache* pResourceCache = rCacheManager.GetCache(
                        resourceCacheName,
                        static_cast< Cache::EPlatform >( platformIndex ) );
                    HELIUM_ASSERT( pResourceCache );
                    pResourceCache->EnforceTocLoad();

                    for( size_t subDataBufferIndex = 0;
                        subDataBufferIndex < subDataBufferCount;
                        ++subDataBufferIndex )
                    {
                        const DynArray< uint8_t >& rSubData = rSubDataBuffers[ subDataBufferIndex ];

                        bCacheResult = pResourceCache->CacheEntry(
                            objectPath,
                            static_cast< uint32_t >( subDataBufferIndex ),
                            rSubData.GetData(),
                            timestamp,
                            static_cast< uint32_t >( rSubData.GetSize() ) );
                        if( !bCacheResult )
                        {
                            HELIUM_TRACE(
                                TRACE_ERROR,
                                ( TXT( "ObjectPreprocessor: Failed to cache resource sub-data %" ) TPRIuSZ
                                TXT( " for resource \"%s\".\n" ) ),
                                subDataBufferIndex,
                                *objectPath.ToString() );

                            bCacheFailure = true;
                        }
                    }
                }

                // Since all resource data has now been recached for the current platform, we can evict the current
                // platform data from memory.
                if( bEvictPlatformPreprocessedResourceData )
                {
                    rResourceData.persistentDataBuffer.Clear();
                    rResourceData.subDataBuffers.Clear();
                    rResourceData.bLoaded = false;
                }
            }
        }
    }

    // Notify the object that it has been cached.
    if( bUpdatedAnyCache )
    {
        pObject->PostSave();
    }

    return !bCacheFailure;

#else  // HELIUM_TOOLS

    HELIUM_UNREF( pObject );
    HELIUM_UNREF( timestamp );
    HELIUM_UNREF( bEvictPlatformPreprocessedResourceData );

    return false;

#endif  // HELIUM_TOOLS
}

/// Load data for the specified resource into memory, preprocessing it from source data if it is out-of-date.
///
/// @param[in] pResource        Resource to load.
/// @param[in] objectTimestamp  Timestamp of the object data stored on disk.  This will be combined with the source
///                             asset timestamp to get the timestamp value with which to compare against the cached
///                             data.
void ObjectPreprocessor::LoadResourceData( Resource* pResource, int64_t objectTimestamp )
{
#if HELIUM_TOOLS

    HELIUM_ASSERT( pResource );

    GameObjectPath resourcePath = pResource->GetPath();

    // Locate the source asset file of the source template resource and combine its timestamp with the object timestamp.
    Resource* pTemplateResource = pResource;
    GameObject* pTestTemplate = Reflect::AssertCast< GameObject >( pResource->GetTemplate() );
    while( pTestTemplate && !pTestTemplate->IsDefaultTemplate() )
    {
        pTemplateResource = Reflect::AssertCast< Resource >( pTestTemplate );
        pTestTemplate = Reflect::AssertCast< GameObject >( pTemplateResource->GetTemplate() );
    }

    GameObjectPath parentPath = pTemplateResource->GetPath();
    GameObjectPath baseResourcePath;
    do
    {
        baseResourcePath = parentPath;
        parentPath = parentPath.GetParent();
    } while( !parentPath.IsEmpty() && !parentPath.IsPackage() );

    Path sourceFilePath;
    if ( !File::GetDataDirectory( sourceFilePath ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "ObjectPreprocessor::LoadResourceData(): Could not retrieve data directory.\n" ) );

        return;
    }

    sourceFilePath += baseResourcePath.ToFilePathString().GetData();

    Helium::Stat stat;
    sourceFilePath.Stat( stat );

    int64_t sourceFileTimestamp = stat.m_ModifiedTime;

    int64_t timestamp = Max( objectTimestamp, sourceFileTimestamp );

    // Check if data is loaded for each supported platform, attempting to load the data from the cache if it exists
    // and is up-to-date.
    size_t platformIndex;
    for( platformIndex = 0; platformIndex < HELIUM_ARRAY_COUNT( m_pPlatformPreprocessors ); ++platformIndex )
    {
        // Skip platforms for which we don't have preprocessing support.
        PlatformPreprocessor* pPreprocessor = m_pPlatformPreprocessors[ platformIndex ];
        if( !pPreprocessor )
        {
            continue;
        }

        // Check if we already have loaded resource data.
        const Resource::PreprocessedData& rPreprocessedData = pResource->GetPreprocessedData(
            static_cast< Cache::EPlatform >( platformIndex ) );
        if( rPreprocessedData.bLoaded )
        {
            continue;
        }

        // Retrieve the timestamp of the cached data using the object cache.
        GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
        HELIUM_ASSERT( pObjectLoader );

        CacheManager& rCacheManager = CacheManager::GetStaticInstance();
        Cache* pCache = rCacheManager.GetCache(
            pObjectLoader->GetCacheName(),
            static_cast< Cache::EPlatform >( platformIndex ) );
        HELIUM_ASSERT( pCache );
        pCache->EnforceTocLoad();

        const Cache::Entry* pCacheEntry = pCache->FindEntry( resourcePath, 0 );
        if( !pCacheEntry || pCacheEntry->timestamp != timestamp )
        {
            HELIUM_TRACE(
                TRACE_INFO,
                ( TXT( "ObjectPreprocessor::LoadResourceData(): Cached resource data not found or is out-of-date " )
                TXT( "for resource \"%s\".  Resource will be preprocessed.\n" ) ),
                *resourcePath.ToString() );

            break;
        }

        // Cached data should be up-to-date, so attempt to load the data from the cache.
        if( !LoadCachedResourceData( pResource, static_cast< Cache::EPlatform >( platformIndex ) ) )
        {
            HELIUM_TRACE(
                TRACE_WARNING,
                ( TXT( "ObjectPreprocessor::LoadResourceData(): Failed to load cached resource data for " )
                TXT( "\"%s\".  Resource will be preprocessed again.\n" ) ),
                *resourcePath.ToString() );

            break;
        }
    }

    if( platformIndex >= HELIUM_ARRAY_COUNT( m_pPlatformPreprocessors ) )
    {
        // All supported platforms loaded successfully, so nothing else needs to be done.
        return;
    }

    // Preprocess all resources for each supported platform.
    if( !PreprocessResource( pResource, String( sourceFilePath.c_str() ) ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "ObjectPreprocessor::LoadResourceData(): Preprocessing of resource \"%s\" failed.\n" ),
            *resourcePath.ToString() );
    }

#else  // HELIUM_TOOLS

    HELIUM_UNREF( pResource );
    HELIUM_UNREF( objectTimestamp );

#endif  // HELIUM_TOOLS
}

/// Load the persistent resource data for the specified resource from the object cache.
///
/// @param[in]  resourcePath           Path of the resource object.
/// @param[in]  platform               Platform for which to retrieve the cached data.
/// @param[out] rPersistentDataBuffer  Buffer in which the persistent resource data should be stored.
///
/// @return  Number of resource sub-data chunks if loaded successfully, Invalid< uint32_t >() if not loaded
///          successfully.
uint32_t ObjectPreprocessor::LoadPersistentResourceData(
    GameObjectPath resourcePath,
    Cache::EPlatform platform,
    DynArray< uint8_t >& rPersistentDataBuffer )
{
    HELIUM_ASSERT( !resourcePath.IsEmpty() );
    HELIUM_ASSERT( static_cast< size_t >( platform ) < static_cast< size_t >( Cache::PLATFORM_MAX ) );

    rPersistentDataBuffer.Resize( 0 );

    // Get the platform preprocessor.
    PlatformPreprocessor* pPreprocessor = m_pPlatformPreprocessors[ platform ];
    if( !pPreprocessor )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "ObjectPreprocessor::LoadPersistentResourceData(): Platform preprocessor does not exist for " )
            TXT( "platform index %" ) TPRId32 TXT( ".\n" ) ),
            static_cast< int32_t >( platform ) );

        return Invalid< uint32_t >();
    }

    // Retrieve the object cache for the specified platform.
    GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
    HELIUM_ASSERT( pObjectLoader );

    CacheManager& rCacheManager = CacheManager::GetStaticInstance();
    Cache* pCache = rCacheManager.GetCache( pObjectLoader->GetCacheName(), platform );
    HELIUM_ASSERT( pCache );
    pCache->EnforceTocLoad();

    // Locate the cache entry for the resource.
    const Cache::Entry* pCacheEntry = pCache->FindEntry( resourcePath, 0 );
    if( !pCacheEntry )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "ObjectPreprocessor::LoadPersistentResourceData(): Failed to locate cached persistent " )
            TXT( "resource data for \"%s\".\n" ) ),
            *resourcePath.ToString() );

        return Invalid< uint32_t >();
    }

    if( pCacheEntry->size < sizeof( uint32_t ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "ObjectPreprocessor::LoadPersistentResourceData(): GameObject cache entry for \"%s\" is smaller " )
            TXT( "than the size needed to provide the property data stream byte count.\n" ) ),
            *resourcePath.ToString() );

        return Invalid< uint32_t >();
    }

    FileStream* pFileStream = File::Open( pCache->GetCacheFileName(), FileStream::MODE_READ );
    if( !pFileStream )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "ObjectPreprocessor::LoadPersistentResourceData(): Failed to open cache file \"%s\" for " )
            TXT( "retrieving cached object data for \"%s\".\n" ) ),
            *pCache->GetCacheFileName(),
            *resourcePath.ToString() );

        return Invalid< uint32_t >();
    }

    BufferedStream bufferedStream( pFileStream );

    int64_t seekLocation = bufferedStream.Seek( pCacheEntry->offset, SeekOrigins::SEEK_ORIGIN_BEGIN );
    if( static_cast< uint64_t >( seekLocation ) != pCacheEntry->offset )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "ObjectPreprocessor::LoadPersistentResourceData(): Failed to seek to byte offset %" ) TPRIu64
            TXT( " in cache file \"%s\" for retrieving cached object data for \"%s\".\n" ) ),
            pCacheEntry->offset,
            *pCache->GetCacheFileName(),
            *resourcePath.ToString() );

        bufferedStream.Close();
        delete pFileStream;

        return Invalid< uint32_t >();
    }

    ByteSwappingStream byteSwapStream( &bufferedStream );
    Stream* pReadStream =
        ( pPreprocessor->SwapBytes()
        ? static_cast< Stream* >( &byteSwapStream )
        : static_cast< Stream* >( &bufferedStream ) );

    uint32_t propertyDataSize = 0;
    size_t readCount = pReadStream->Read( &propertyDataSize, sizeof( propertyDataSize ), 1 );
    if( readCount != 1 )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "ObjectPreprocessor::LoadPersistentResourceData(): Failed to read the size of the object " )
            TXT( "property stream for \"%s\" from cache \"%s\".\n" ) ),
            *resourcePath.ToString(),
            *pCache->GetCacheFileName() );

        byteSwapStream.Close();
        bufferedStream.Close();
        delete pFileStream;

        return Invalid< uint32_t >();
    }

    if( propertyDataSize > pCacheEntry->size - sizeof( propertyDataSize ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "ObjectPreprocessor::LoadPersistentResourceData(): Property data stream for \"%s\" (%" ) TPRIu32
            TXT( " bytes) extends past the end of its cached object data stream (%" ) TPRIu32 TXT( " bytes).  " )
            TXT( "Size will be clamped.\n" ) ),
            *resourcePath.ToString(),
            propertyDataSize,
            pCacheEntry->size );

        propertyDataSize = pCacheEntry->size - sizeof( propertyDataSize );
    }

    if( pCacheEntry->size - sizeof( propertyDataSize ) - propertyDataSize < sizeof( uint32_t ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "ObjectPreprocessor::LoadPersistentResourceData(): Property data stream for \"%s\" is not " )
            TXT( "large enough to provide the resource sub-data count.\n" ) ),
            *resourcePath.ToString() );

        byteSwapStream.Close();
        bufferedStream.Close();
        delete pFileStream;

        return Invalid< uint32_t >();
    }

    uint64_t newOffset = pCacheEntry->offset + sizeof( propertyDataSize ) + propertyDataSize;
    seekLocation = bufferedStream.Seek( newOffset, SeekOrigins::SEEK_ORIGIN_BEGIN );
    if( static_cast< uint64_t >( seekLocation ) != newOffset )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "ObjectPreprocessor::LoadPersistentResourceData(): Failed to seek to byte offset %" ) TPRIu64
            TXT( " in cache file \"%s\" for the cached persistent resource data for \"%s\".\n" ) ),
            newOffset,
            *pCache->GetCacheFileName(),
            *resourcePath.ToString() );

        byteSwapStream.Close();
        bufferedStream.Close();
        delete pFileStream;

        return Invalid< uint32_t >();
    }

    size_t resourceDataStreamSize =
        pCacheEntry->size - sizeof( propertyDataSize ) - propertyDataSize - sizeof( uint32_t );

    rPersistentDataBuffer.Reserve( resourceDataStreamSize );
    rPersistentDataBuffer.Resize( resourceDataStreamSize );

    size_t bytesRead = bufferedStream.Read( rPersistentDataBuffer.GetData(), 1, resourceDataStreamSize );
    if( bytesRead != resourceDataStreamSize )
    {
        HELIUM_TRACE(
            TRACE_WARNING,
            ( TXT( "ObjectPreprocessor::LoadPersistentResourceData(): Attempted to load %" ) TPRIuSZ
            TXT( " bytes from offset %" ) TPRIu64 TXT( " in cache file \"%s\", but only %" ) TPRIuSZ
            TXT( " bytes could be read.\n" ) ),
            resourceDataStreamSize,
            newOffset,
            *pCache->GetCacheFileName(),
            bytesRead );

        rPersistentDataBuffer.Resize( bytesRead );
    }

    rPersistentDataBuffer.Trim();

    uint32_t subDataCount = 0;
    readCount = bufferedStream.Read( &subDataCount, sizeof( subDataCount ), 1 );
    if( readCount != 1 )
    {
        HELIUM_TRACE(
            TRACE_WARNING,
            ( TXT( "ObjectPreprocessor::LoadPersistentResourceData(): Failed to read resource sub-data count for " )
            TXT( "\"%s\" from the end of its object data stream.\n" ) ),
            *resourcePath.ToString() );
    }

    byteSwapStream.Close();
    bufferedStream.Close();
    delete pFileStream;

    return subDataCount;
}

/// Create the singleton ObjectPreprocessor instance.
///
/// @return  Pointer to the created instance.
///
/// @see DestroyStaticInstance(), GetStaticInstance()
ObjectPreprocessor* ObjectPreprocessor::CreateStaticInstance()
{
    if( !sm_pInstance )
    {
        sm_pInstance = new ObjectPreprocessor;
        HELIUM_ASSERT( sm_pInstance );
    }

    return sm_pInstance;
}

/// Destroy the singleton ObjectPreprocessor instance.
///
/// @see CreateStaticInstance(), GetStaticInstance()
void ObjectPreprocessor::DestroyStaticInstance()
{
    delete sm_pInstance;
    sm_pInstance = NULL;
}

/// Get the singleton ObjectPreprocessor instance.
///
/// Note that the ObjectPreprocessor instance is not created automatically.  One must explicitly be created using
/// CreateStaticInstance() to avoid unnecessary creation when not running in editor mode.
///
/// @return  Pointer to the ObjectPreprocessor instance if one exists, null if not.
///
/// @see CreateStaticInstance(), DestroyStaticInstance()
ObjectPreprocessor* ObjectPreprocessor::GetStaticInstance()
{
    return sm_pInstance;
}

#if HELIUM_TOOLS
/// Helper function for loading the cached resource data for a specific platform.
///
/// @param[in] pResource  Resource to load.  The data will be loaded into the proper Resource::PreprocessedData
///                       structure stored in memory with the resource.
/// @param[in] platform   Platform for which to load the resource data.
///
/// @return  True if the resource data was loaded successfully, false if loading failed.
bool ObjectPreprocessor::LoadCachedResourceData( Resource* pResource, Cache::EPlatform platform )
{
    HELIUM_ASSERT( pResource );
    HELIUM_ASSERT( static_cast< size_t >( platform ) < static_cast< size_t >( Cache::PLATFORM_MAX ) );

    GameObjectPath resourcePath = pResource->GetPath();

    Resource::PreprocessedData& rPreprocessedData = pResource->GetPreprocessedData( platform );
    rPreprocessedData.bLoaded = false;

    // Load the persistent resource data and retrieve the number of sub-data chunks.
    uint32_t subDataCount = LoadPersistentResourceData(
        resourcePath,
        platform,
        rPreprocessedData.persistentDataBuffer );
    if( IsInvalid( subDataCount ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "ObjectPreprocessor::LoadCachedResourceData(): Failed to load persistent resource data for " )
            TXT( "\"%s\".\n" ) ),
            *resourcePath.ToString() );

        return false;
    }

    // Load the sub-data from the resource cache.
    if( subDataCount != 0 )
    {
        Name resourceCacheName = pResource->GetCacheName();
        HELIUM_ASSERT( !resourceCacheName.IsEmpty() );

        CacheManager& rCacheManager = CacheManager::GetStaticInstance();
        Cache* pResourceCache = rCacheManager.GetCache( resourceCacheName, platform );
        HELIUM_ASSERT( pResourceCache );
        pResourceCache->EnforceTocLoad();

        Path resourceCachePath( pResourceCache->GetCacheFileName().GetData() );
        if( !resourceCachePath.Exists() )
        {
            HELIUM_TRACE(
                TRACE_INFO,
                ( TXT( "ObjectPreprocessor::LoadCachedResourceData(): Cache file for cache \"%s\" does not exist " )
                TXT( "when loading resource data for \"%s\".\n" ) ),
                *resourceCacheName,
                *resourcePath.ToString() );

            return false;
        }

        FileStream* pFileStream = File::Open( resourceCachePath.c_str(), FileStream::MODE_READ );
        if( !pFileStream )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "ObjectPreprocessor::LoadCachedResourceData(): Failed to open cache file for cache \"%s\" " )
                TXT( "for resource \"%s\".\n" ) ),
                *resourceCacheName,
                *resourcePath.ToString() );

            return false;
        }

        DynArray< DynArray< uint8_t > >& rSubDataBuffers = rPreprocessedData.subDataBuffers;
        rSubDataBuffers.Reserve( subDataCount );
        rSubDataBuffers.Resize( subDataCount );

        for( uint32_t subDataIndex = 0; subDataIndex < subDataCount; ++subDataIndex )
        {
            const Cache::Entry* pResourceCacheEntry = pResourceCache->FindEntry( resourcePath, subDataIndex );
            if( !pResourceCacheEntry )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "ObjectPreprocessor::LoadCachedResourceData(): Failed to locate sub-data %" ) TPRIu32
                    TXT( " of resource \"%s\" in cache \"%s\".\n" ) ),
                    subDataIndex,
                    *resourcePath.ToString(),
                    *resourceCacheName );

                delete pFileStream;

                return false;
            }

            int64_t newOffset = pFileStream->Seek( pResourceCacheEntry->offset, SeekOrigins::SEEK_ORIGIN_BEGIN );
            if( static_cast< uint64_t >( newOffset ) != pResourceCacheEntry->offset )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "ObjectPreprocessor::LoadCachedResourceData(): Failed to seek to offset %" ) TPRIu64
                    TXT( " in cache \"%s\" when loading sub-data %" ) TPRIu32 TXT( " of resource \"%s\".\n" ) ),
                    pResourceCacheEntry->offset,
                    *resourceCacheName,
                    subDataIndex,
                    *resourcePath.ToString() );

                delete pFileStream;

                return false;
            }

            uint32_t subDataSize = pResourceCacheEntry->size;

            DynArray< uint8_t >& rSubData = rSubDataBuffers[ subDataIndex ];
            rSubData.Reserve( subDataSize );
            rSubData.Resize( subDataSize );
            rSubData.Trim();

            size_t bytesRead = pFileStream->Read( rSubData.GetData(), 1, subDataSize );
            if( bytesRead != subDataSize )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "ObjectPreprocessor::LoadCachedResourceData(): Failed to read %" ) TPRIu32
                    TXT( " bytes from cache \"%s\" for sub-data %" ) TPRIu32 TXT( " of resource \"%s\" (only %" )
                    TPRIuSZ TXT( " bytes read).\n" ) ),
                    subDataSize,
                    *resourceCacheName,
                    subDataIndex,
                    *resourcePath.ToString(),
                    bytesRead );

                delete pFileStream;

                return false;
            }
        }

        delete pFileStream;
    }

    // Loaded.
    rPreprocessedData.bLoaded = true;

    return true;
}

/// Preprocess a resource for all enabled platforms, storing the resource data in memory with the resource.
///
/// @param[in] pResource        Resource to preprocess.
/// @param[in] rSourceFilePath  Path name of the source resource data file.
///
/// @return  True if preprocessing was successful, false if not.
bool ObjectPreprocessor::PreprocessResource( Resource* pResource, const String& rSourceFilePath )
{
    HELIUM_ASSERT( pResource );
    HELIUM_ASSERT( !pResource->IsDefaultTemplate() );

    HELIUM_TRACE(
        TRACE_INFO,
        TXT( "ObjectPreprocessor::PreprocessResource(): Preprocessing resource \"%s\".\n" ),
        *pResource->GetPath().ToString() );

    // Clear out all existing resource data.
    for( size_t platformIndex = 0; platformIndex < static_cast< size_t >( Cache::PLATFORM_MAX ); ++platformIndex )
    {
        Resource::PreprocessedData& rPreprocessedData = pResource->GetPreprocessedData(
            static_cast< Cache::EPlatform >( platformIndex ) );
        rPreprocessedData.persistentDataBuffer.Clear();
        rPreprocessedData.subDataBuffers.Clear();
        rPreprocessedData.bLoaded = false;
    }

    // Locate a resource handler for the resource type.
    const GameObjectType* pResourceType = pResource->GetGameObjectType();
    HELIUM_ASSERT( pResourceType );
    ResourceHandler* pResourceHandler = ResourceHandler::FindResourceHandlerForType( pResourceType );
    if( !pResourceHandler )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "ObjectPreprocessor::PreprocessResource(): Failed to locate resource handler for resource " )
            TXT( "\"%s\" of type \"%s\".\n" ) ),
            *pResource->GetPath().ToString(),
            *pResourceType->GetName() );

        return false;
    }

    // Preprocess and cache the resource for the each enabled platform.
    if( !pResourceHandler->CacheResource( this, pResource, rSourceFilePath ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "ObjectPreprocessor::PreprocessResource(): Failed to preprocess resource \"%s\".\n" ),
            *pResource->GetPath().ToString() );

        return false;
    }

    // Reserialize the current platform's persistent resource data.
    CacheManager& rCacheManager = CacheManager::GetStaticInstance();
    Cache::EPlatform platform = rCacheManager.GetCurrentPlatform();
    HELIUM_ASSERT( static_cast< size_t >( platform ) < HELIUM_ARRAY_COUNT( m_pPlatformPreprocessors ) );
    PlatformPreprocessor* pPlatformPreprocessor = m_pPlatformPreprocessors[ platform ];
    if( pPlatformPreprocessor )
    {
        const Resource::PreprocessedData& rPreprocessedData = pResource->GetPreprocessedData( platform );
        if( rPreprocessedData.bLoaded )
        {
            const DynArray< uint8_t >& rPersistentDataBuffer = rPreprocessedData.persistentDataBuffer;
            size_t persistentDataBufferSize = rPersistentDataBuffer.GetSize();
            if( persistentDataBufferSize != 0 )
            {
                Reflect::ObjectPtr persistent_data = Cache::ReadCacheObjectFromBuffer(rPersistentDataBuffer);
                
                pResource->LoadPersistentResourceObject(persistent_data);
            }
        }
    }

    return true;
}
#endif  // HELIUM_TOOLS
