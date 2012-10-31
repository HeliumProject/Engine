//----------------------------------------------------------------------------------------------------------------------
// CacheManager.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/CacheManager.h"

#include "Platform/Process.h"
#include "Foundation/FilePath.h"
#include "Foundation/File.h"

using namespace Helium;

CacheManager* CacheManager::sm_pInstance = NULL;

/// Constructor.
CacheManager::CacheManager( const Path& rBaseDirectory )
: m_cachePool( CACHE_POOL_BLOCK_SIZE )
{
    m_platformDataDirectories[ Cache::PLATFORM_PC ] = rBaseDirectory.c_str();
    m_platformDataDirectories[ Cache::PLATFORM_PC ] += TXT( "DataPC/" );
    m_platformDataDirectories[ Cache::PLATFORM_PC ].Trim();
}

/// Destructor.
CacheManager::~CacheManager()
{
}

/// Get the specified cache, creating the cache instance if necessary.
///
/// @param[in] name      Cache name.
/// @param[in] platform  Cache platform, or Cache::PLATFORM_INVALID to resolve the current platform's cache.
///
/// @return  Cache instance.
Cache* CacheManager::GetCache( Name name, Cache::EPlatform platform )
{
    HELIUM_ASSERT(
        platform == Cache::PLATFORM_INVALID ||
        static_cast< size_t >( platform ) < static_cast< size_t >( Cache::PLATFORM_MAX ) );

    // Default to the current platform if PLATFORM_INVALID was specified.
    if( platform == Cache::PLATFORM_INVALID )
    {
        platform = GetCurrentPlatform();
    }

    // Try to locate an existing cache instance first.
    ConcurrentHashMap< Name, Cache* >::Accessor cacheAccessor;
    if( m_cacheMaps[ platform ].Find( cacheAccessor, name ) )
    {
        Cache *pCache = cacheAccessor->Second();
        HELIUM_ASSERT( pCache );

        return pCache;
    }

    // Cache instance wasn't found, so create a new one and add it.
    Cache* pCache = m_cachePool.Allocate();
    HELIUM_ASSERT( pCache );

    const String& rPlatformDataDirectory = GetPlatformDataDirectory( platform );

#if HELIUM_TOOLS
    // If the cache directory doesn't exist, attempt to create it.
    Path platformDataPath( rPlatformDataDirectory.GetData() );
    platformDataPath.MakePath();
#endif

    String cacheFileName = rPlatformDataDirectory;
    cacheFileName += *name;

    String tocFileName = cacheFileName;
    tocFileName += TXT( "." ) HELIUM_CACHE_TOC_EXTENSION;

    cacheFileName += TXT( "." ) HELIUM_CACHE_EXTENSION;

    if( !pCache->Initialize( name, platform, *tocFileName, *cacheFileName ) )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "CacheManager: Failed to initialize cache \"%s\".\n" ), *name );

        return NULL;
    }

    if( !m_cacheMaps[ platform ].Insert( cacheAccessor, KeyValue< Name, Cache* >( name, pCache ) ) )
    {
        // Cache instance was added while we were trying to create a new one, so release the one we allocated and
        // use the existing instance.
        pCache->Shutdown();
        m_cachePool.Release( pCache );

        pCache = cacheAccessor->Second();
        HELIUM_ASSERT( pCache );
    }

    return pCache;
}

/// Get the cache data directory for the specified platform.
///
/// @param[in] platform  Target platform, or Cache::PLATFORM_INVALID name to use the current platform.
///
/// @return  Cache data directory path.
const String& CacheManager::GetPlatformDataDirectory( Cache::EPlatform platform )
{
    HELIUM_ASSERT(
        platform == Cache::PLATFORM_INVALID ||
        static_cast< size_t >( platform ) < static_cast< size_t >( Cache::PLATFORM_MAX ) );

    if( platform == Cache::PLATFORM_INVALID )
    {
        platform = GetCurrentPlatform();
    }

    return m_platformDataDirectories[ platform ];
}

/// Initialize the singleton CacheManager instance.  You must call this function
/// before calling GetStaticInstance().  This function should be called once and
/// only once.
///
/// @return  Reference to the CacheManager instance.
///
/// @see DestroyStaticInstance()
bool CacheManager::InitializeStaticInstance( const Path& rBaseDirectory )
{
    HELIUM_ASSERT( sm_pInstance == NULL );
    sm_pInstance = new CacheManager( rBaseDirectory );
    HELIUM_ASSERT( sm_pInstance );

    return sm_pInstance != NULL;
}

/// Get the singleton CacheManager instance, creating it if necessary.
///
/// @return  Reference to the CacheManager instance.
///
/// @see DestroyStaticInstance()
CacheManager& CacheManager::GetStaticInstance()
{
    HELIUM_ASSERT( sm_pInstance );
    return *sm_pInstance;
}

/// Destroy the singleton CacheManager instance.
///
/// @see GetStaticInstance()
void CacheManager::DestroyStaticInstance()
{
    HELIUM_ASSERT( sm_pInstance );
    delete sm_pInstance;
    sm_pInstance = NULL;
}

/// Get the identifier for the current cache platform.
///
/// @return  Current cache platform.
Cache::EPlatform CacheManager::GetCurrentPlatform()
{
#if HELIUM_OS_WIN
    return Cache::PLATFORM_PC;
#else
#error CacheManager: Unsupported platform.
#endif
}
