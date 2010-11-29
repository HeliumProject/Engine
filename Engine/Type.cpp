//----------------------------------------------------------------------------------------------------------------------
// Type.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/Type.h"

#include "Foundation/Container/ObjectPool.h"

namespace Lunar
{
    /// Static reference count proxy management data.
    struct TypeRefCountSupport::StaticData
    {
        /// Number of proxy objects to allocate per block for the proxy pool.
        static const size_t POOL_BLOCK_SIZE = 1024;

        /// Proxy object pool.
        ObjectPool< RefCountProxy< Type > > proxyPool;
#if HELIUM_ENABLE_MEMORY_TRACKING
        /// Active reference count proxies.
        ConcurrentHashSet< RefCountProxy< Type >* > activeProxySet;
#endif

        /// @name Construction/Destruction
        //@{
        StaticData();
        //@}
    };

    TypeRefCountSupport::StaticData* TypeRefCountSupport::sm_pStaticData = NULL;

    PackagePtr Type::sm_spTypePackage;
    Type::LookupMap* Type::sm_pLookupMap = NULL;

    /// Retrieve a reference count proxy from the global pool.
    ///
    /// @return  Pointer to a reference count proxy.
    ///
    /// @see Release()
    RefCountProxy< Type >* TypeRefCountSupport::Allocate()
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

        RefCountProxy< Type >* pProxy = pStaticData->proxyPool.Allocate();
        HELIUM_ASSERT( pProxy );

#if HELIUM_ENABLE_MEMORY_TRACKING
        ConcurrentHashSet< RefCountProxy< Type >* >::Accessor activeProxySetAccessor;
        HELIUM_VERIFY( pStaticData->activeProxySet.Insert( activeProxySetAccessor, pProxy ) );
#endif

        return pProxy;
    }

    /// Release a reference count proxy back to the global pool.
    ///
    /// @param[in] pProxy  Pointer to the reference count proxy to release.
    ///
    /// @see Allocate()
    void TypeRefCountSupport::Release( RefCountProxy< Type >* pProxy )
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
    void TypeRefCountSupport::Shutdown()
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
    size_t TypeRefCountSupport::GetActiveProxyCount()
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
    bool TypeRefCountSupport::GetFirstActiveProxy(
        ConcurrentHashSet< RefCountProxy< Type >* >::ConstAccessor& rAccessor )
    {
        HELIUM_ASSERT( sm_pStaticData );

        return sm_pStaticData->activeProxySet.First( rAccessor );
    }
#endif

    /// Constructor.
    TypeRefCountSupport::StaticData::StaticData()
        : proxyPool( POOL_BLOCK_SIZE )
    {
    }

    /// Constructor.
    Type::Type()
        : m_typeFlags( 0 )
    {
    }

    /// Destructor.
    Type::~Type()
    {
    }

    /// Get whether this type is a subtype of the given type.
    ///
    /// @param[in] pType  Type against which to check.
    bool Type::IsSubtypeOf( const Type* pType ) const
    {
        HELIUM_ASSERT( pType );

        for( const Type* pThisType = this; pThisType != NULL; pThisType = pThisType->GetTypeParent() )
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
    void Type::SetTypePackage( Package* pPackage )
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
    Type* Type::Create( Name name, Package* pTypePackage, Type* pParent, GameObject* pTemplate, uint32_t flags )
    {
        HELIUM_ASSERT( !name.IsEmpty() );
        HELIUM_ASSERT( pTypePackage );
        HELIUM_ASSERT( pTemplate );

        // Set up the template object name, and set this object as its parent.
        if( !pTemplate->SetOwner( pTypePackage ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "Type::Initialize(): Failed to set type \"%s\" template object owner.\n" ),
                *name );

            return false;
        }

        if( !pTemplate->SetName( name ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "Type::Initialize(): Failed to set type \"%s\" template object name.\n" ),
                *name );

            return false;
        }

        // Register the template object with the object system.
        if( !GameObject::RegisterObject( pTemplate ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "Type::Initialize(): Failed to register type \"%s\" template object.\n" ),
                *name );

            return false;
        }

        // Create the type object and store its parameters.
        Type* pType = new Type;
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
        LookupMap::Accessor typeAccessor;
        HELIUM_VERIFY( sm_pLookupMap->Insert(
            typeAccessor,
            std::pair< const Name, Type* >( pType->GetName(), pType ) ) );

        return pType;
    }

    /// Unregister a type.
    ///
    /// @param[in] pType  Type to unregister.  References to the parent type and the type template will be released as
    ///                   well.
    ///
    /// @see Register()
    void Type::Unregister( Type* pType )
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
    Type* Type::Find( Name typeName )
    {
        Type* pType = NULL;
        if( sm_pLookupMap )
        {
            LookupMap::ConstAccessor typeAccessor;
            if( sm_pLookupMap->Find( typeAccessor, typeName ) )
            {
                pType = typeAccessor->second;
                HELIUM_ASSERT( pType );
            }
        }

        return pType;
    }

    /// Begin iterating on all the registered types.
    ///
    /// @param[out] rIterator  Accessor set to reference the first registered type.
    ///
    /// @return  True if the iterator was set to the first type, false if no types are registered.
    bool Type::GetFirstType( ConstIterator& rIterator )
    {
        if( !sm_pLookupMap )
        {
            rIterator.m_accessor.Release();

            return false;
        }

        return sm_pLookupMap->First( rIterator.m_accessor );
    }

    /// Perform shutdown of the Type registration system.
    ///
    /// This releases all final references to objects and releases all allocated memory.  This should be called during
    /// the shutdown process prior to calling GameObject::Shutdown().
    ///
    /// @see GameObject::Shutdown()
    void Type::Shutdown()
    {
        HELIUM_TRACE( TRACE_INFO, TXT( "Shutting down Type registration.\n" ) );

        // Make sure the GameObject type is unregistered, as it does not get included in the unregistration of the Engine
        // type package.
        GameObject::ReleaseStaticType();

        delete sm_pLookupMap;
        sm_pLookupMap = NULL;

        // Release the reference to the main "Types" package.
        sm_spTypePackage.Release();

        HELIUM_TRACE( TRACE_INFO, TXT( "Type registration shutdown complete.\n" ) );
    }
}
