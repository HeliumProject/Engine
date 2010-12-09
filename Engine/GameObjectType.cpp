//----------------------------------------------------------------------------------------------------------------------
// GameObjectType.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/GameObjectType.h"

#include "Foundation/Container/ObjectPool.h"

using namespace Lunar;

/// Static reference count proxy management data.
struct GameObjectTypeRefCountSupport::StaticData
{
    /// Number of proxy objects to allocate per block for the proxy pool.
    static const size_t POOL_BLOCK_SIZE = 1024;

    /// Proxy object pool.
    ObjectPool< RefCountProxy< GameObjectType > > proxyPool;
#if HELIUM_ENABLE_MEMORY_TRACKING
    /// Active reference count proxies.
    ConcurrentHashSet< RefCountProxy< GameObjectType >* > activeProxySet;
#endif

    /// @name Construction/Destruction
    //@{
    StaticData();
    //@}
};

GameObjectTypeRefCountSupport::StaticData* GameObjectTypeRefCountSupport::sm_pStaticData = NULL;

PackagePtr GameObjectType::sm_spTypePackage;
GameObjectType::LookupMap* GameObjectType::sm_pLookupMap = NULL;

/// Retrieve a reference count proxy from the global pool.
///
/// @return  Pointer to a reference count proxy.
///
/// @see Release()
RefCountProxy< GameObjectType >* GameObjectTypeRefCountSupport::Allocate()
{
    // Lazy initialization of the proxy management data.  Even though this isn't thread-safe, it should still be
    // fine as the proxy system should be initialized from the main thread before any sub-threads are spawned (i.e.
    // during startup type initialization).
    StaticData* pStaticData = sm_pStaticData;
    if( !pStaticData )
    {
        pStaticData = new StaticData;
        HELIUM_ASSERT( pStaticData );
        sm_pStaticData = pStaticData;
    }

    RefCountProxy< GameObjectType >* pProxy = pStaticData->proxyPool.Allocate();
    HELIUM_ASSERT( pProxy );

#if HELIUM_ENABLE_MEMORY_TRACKING
    ConcurrentHashSet< RefCountProxy< GameObjectType >* >::Accessor activeProxySetAccessor;
    HELIUM_VERIFY( pStaticData->activeProxySet.Insert( activeProxySetAccessor, pProxy ) );
#endif

    return pProxy;
}

/// Release a reference count proxy back to the global pool.
///
/// @param[in] pProxy  Pointer to the reference count proxy to release.
///
/// @see Allocate()
void GameObjectTypeRefCountSupport::Release( RefCountProxy< GameObjectType >* pProxy )
{
    HELIUM_ASSERT( pProxy );

    StaticData* pStaticData = sm_pStaticData;
    HELIUM_ASSERT( pStaticData );

#if HELIUM_ENABLE_MEMORY_TRACKING
    HELIUM_VERIFY( pStaticData->activeProxySet.Remove( pProxy ) );
#endif

    pStaticData->proxyPool.Release( pProxy );
}

/// Release the name table and free all allocated memory.
///
/// This should only be called immediately prior to application exit.
void GameObjectTypeRefCountSupport::Shutdown()
{
    delete sm_pStaticData;
    sm_pStaticData = NULL;
}

#if HELIUM_ENABLE_MEMORY_TRACKING
/// Get the number of active reference count proxies.
///
/// Be careful when using this function, as the number may change if other threads are actively setting and clearing
/// references to objects.  Unless all other threads have been halted or are otherwise no longer using any smart
/// pointers, you should not expect this value to match the number actually iterated when using functions such as
/// GetFirstActiveProxy().
///
/// @return  Current number of active smart pointer references.
///
/// @see GetFirstActiveProxy()
size_t GameObjectTypeRefCountSupport::GetActiveProxyCount()
{
    HELIUM_ASSERT( sm_pStaticData );

    return sm_pStaticData->activeProxySet.GetSize();
}

/// Initialize a constant accessor to the first active reference count proxy.
///
/// @param[in] rAccessor  Accessor to initialize.
///
/// @return  True if there are active reference count proxies and the accessor was successfully set to reference the
///          first one, false if not.
///
/// @see GetActiveProxyCount()
bool GameObjectTypeRefCountSupport::GetFirstActiveProxy(
    ConcurrentHashSet< RefCountProxy< GameObjectType >* >::ConstAccessor& rAccessor )
{
    HELIUM_ASSERT( sm_pStaticData );

    return sm_pStaticData->activeProxySet.First( rAccessor );
}
#endif

/// Constructor.
GameObjectTypeRefCountSupport::StaticData::StaticData()
: proxyPool( POOL_BLOCK_SIZE )
{
}

/// Constructor.
GameObjectType::GameObjectType()
: m_typeFlags( 0 )
{
}

/// Destructor.
GameObjectType::~GameObjectType()
{
}

/// Get whether this type is a subtype of the given type.
///
/// @param[in] pType  Type against which to check.
bool GameObjectType::IsSubtypeOf( const GameObjectType* pType ) const
{
    HELIUM_ASSERT( pType );

    for( const GameObjectType* pThisType = this; pThisType != NULL; pThisType = pThisType->GetTypeParent() )
    {
        if( pThisType == pType )
        {
            return true;
        }
    }

    return false;
}

/// Set the package in which all template object packages are stored.
///
/// @param[in] pPackage  Main type package.
///
/// @see GetTypePackage()
void GameObjectType::SetTypePackage( Package* pPackage )
{
    HELIUM_ASSERT( pPackage );

    // Only allow the type package to be set once.
    HELIUM_ASSERT( !sm_spTypePackage );

    sm_spTypePackage = pPackage;
}

/// Create a type object.
///
/// @param[in] name          Type name.
/// @param[in] pTypePackage  Package in which the template object should be stored.
/// @param[in] pParent       Parent type.
/// @param[in] pTemplate     Template object.
/// @param[in] flags         Type flags.
///
/// @return  Pointer to the type object if created successfully, null if not.
///
/// @see Unregister()
GameObjectType* GameObjectType::Create( Name name, Package* pTypePackage, GameObjectType* pParent, GameObject* pTemplate, uint32_t flags )
{
    HELIUM_ASSERT( !name.IsEmpty() );
    HELIUM_ASSERT( pTypePackage );
    HELIUM_ASSERT( pTemplate );

    // Set up the template object name, and set this object as its parent.
    if( !pTemplate->SetOwner( pTypePackage ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "GameObjectType::Initialize(): Failed to set type \"%s\" template object owner.\n" ),
            *name );

        return false;
    }

    if( !pTemplate->SetName( name ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "GameObjectType::Initialize(): Failed to set type \"%s\" template object name.\n" ),
            *name );

        return false;
    }

    // Flag the object as the default template object for the type being created.
    pTemplate->SetFlags( GameObject::FLAG_DEFAULT_TEMPLATE );

    // Register the template object with the object system.
    if( !GameObject::RegisterObject( pTemplate ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "GameObjectType::Initialize(): Failed to register type \"%s\" template object.\n" ),
            *name );

        return false;
    }

    // Create the type object and store its parameters.
    GameObjectType* pType = new GameObjectType;
    HELIUM_ASSERT( pType );
    pType->m_name = name;
    pType->m_spTypeParent = pParent;
    pType->m_spTypeTemplate = pTemplate;
    pType->m_typeFlags = flags;

    // Lazily initialize the lookup map.  Note that this is not inherently thread-safe, but there should always be
    // at least one type registered before any sub-threads are spawned.
    if( !sm_pLookupMap )
    {
        sm_pLookupMap = new LookupMap;
        HELIUM_ASSERT( sm_pLookupMap );
    }

    // Register the type (note that a type with the same name should not already exist in the lookup map).
    LookupMap::Iterator typeIterator;
    HELIUM_VERIFY( sm_pLookupMap->Insert( typeIterator, KeyValue< Name, GameObjectTypePtr >( pType->GetName(), pType ) ) );

    return pType;
}

/// Unregister a type.
///
/// @param[in] pType  Type to unregister.  References to the parent type and the type template will be released as well.
///
/// @see Register()
void GameObjectType::Unregister( GameObjectType* pType )
{
    HELIUM_ASSERT( pType );

    HELIUM_ASSERT( sm_pLookupMap );
    HELIUM_VERIFY( sm_pLookupMap->Remove( pType->GetName() ) );

    pType->m_spTypeParent.Release();
    pType->m_spTypeTemplate.Release();
}

/// Look up a type by name.
///
/// @param[in] typeName  Name of the type to look up.
///
/// @return  Pointer to the specified type if found, null pointer if not found.
GameObjectType* GameObjectType::Find( Name typeName )
{
    GameObjectType* pType = NULL;
    if( sm_pLookupMap )
    {
        LookupMap::ConstIterator typeIterator = sm_pLookupMap->Find( typeName );
        if( typeIterator != sm_pLookupMap->End() )
        {
            pType = typeIterator->Second();
            HELIUM_ASSERT( pType );
        }
    }

    return pType;
}

/// Get an iterator referencing the first registered type.
///
/// @return  Iterator referencing the first registered type.
///
/// @see GetTypeEnd()
GameObjectType::ConstIterator GameObjectType::GetTypeBegin()
{
    if( sm_pLookupMap )
    {
        return ConstIterator( sm_pLookupMap->Begin() );
    }

    ConstIterator nullIterator;
    MemoryZero( &nullIterator, sizeof( nullIterator ) );

    return nullIterator;
}

/// Get an iterator referencing the end of the type registration map.
///
/// @return  Iterator referencing the end of the type registration map (one past the last entry).
///
/// @see GetTypeBegin()
GameObjectType::ConstIterator GameObjectType::GetTypeEnd()
{
    if( sm_pLookupMap )
    {
        return ConstIterator( sm_pLookupMap->End() );
    }

    ConstIterator nullIterator;
    MemoryZero( &nullIterator, sizeof( nullIterator ) );

    return nullIterator;
}

/// Perform shutdown of the GameObjectType registration system.
///
/// This releases all final references to objects and releases all allocated memory.  This should be called during
/// the shutdown process prior to calling GameObject::Shutdown().
///
/// @see GameObject::Shutdown()
void GameObjectType::Shutdown()
{
    HELIUM_TRACE( TRACE_INFO, TXT( "Shutting down GameObjectType registration.\n" ) );

    // Make sure the GameObject type is unregistered, as it does not get included in the unregistration of the Engine
    // type package.
    GameObject::ReleaseStaticType();

    delete sm_pLookupMap;
    sm_pLookupMap = NULL;

    // Release the reference to the main "Types" package.
    sm_spTypePackage.Release();

    HELIUM_TRACE( TRACE_INFO, TXT( "GameObjectType registration shutdown complete.\n" ) );
}
