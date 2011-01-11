//----------------------------------------------------------------------------------------------------------------------
// GameObjectType.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/GameObjectType.h"

#include "Foundation/Container/ObjectPool.h"
#include "Foundation/Reflect/Registry.h"

using namespace Lunar;

PackagePtr GameObjectType::sm_spTypePackage;
GameObjectType::LookupMap* GameObjectType::sm_pLookupMap = NULL;

/// Constructor.
GameObjectType::GameObjectType()
    : m_pReleaseStaticTypeCallback( NULL )
    , m_flags( 0 )
{
}

/// Destructor.
GameObjectType::~GameObjectType()
{
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
const GameObjectType* GameObjectType::Create(
    Name name,
    Package* pTypePackage,
    const GameObjectType* pParent,
    GameObject* pTemplate,
    RELEASE_STATIC_TYPE_CALLBACK* pReleaseStaticTypeCallback,
    uint32_t flags )
{
    HELIUM_ASSERT( !name.IsEmpty() );
    HELIUM_ASSERT( pTypePackage );
    HELIUM_ASSERT( pTemplate );
    HELIUM_ASSERT( pReleaseStaticTypeCallback );

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

    // If the parent type is null, default to Reflect::Object, as the type should be deriving from it directly.
    const Reflect::Class* pBaseClass = pParent;
    if( !pBaseClass )
    {
        pBaseClass = Reflect::Object::s_Class;
        HELIUM_ASSERT( pBaseClass );
    }

    // Create the type object and store its parameters.
    GameObjectType* pType = new GameObjectType;
    HELIUM_ASSERT( pType );
    pType->m_Name = *name;
    pType->m_Size = static_cast< uint32_t >( pTemplate->GetInstanceSize() );
    pType->m_Base = pBaseClass;
    pType->m_Template = pTemplate;
    pType->m_TemplateObject = pTemplate;
    pType->m_cachedName = name;
    pType->m_pReleaseStaticTypeCallback = pReleaseStaticTypeCallback;
    pType->m_flags = flags;

    if( pParent )
    {
        pParent->AddDerived( pType );
    }

    // Lazily initialize the lookup map.  Note that this is not inherently thread-safe, but there should always be
    // at least one type registered before any sub-threads are spawned.
    if( !sm_pLookupMap )
    {
        sm_pLookupMap = new LookupMap;
        HELIUM_ASSERT( sm_pLookupMap );
    }

    // Register the type (note that a type with the same name should not already exist in the lookup map).
    LookupMap::Iterator typeIterator;
    HELIUM_VERIFY( sm_pLookupMap->Insert(
        typeIterator,
        KeyValue< Name, GameObjectTypePtr >( pType->GetName(), pType ) ) );

    Reflect::Registry* pRegistry = Reflect::Registry::GetInstance();
    HELIUM_ASSERT( pRegistry );
    pRegistry->RegisterType( pType );

    return pType;
}

/// Unregister a type.
///
/// Note that this should only be called by the static type release functions (i.e. Entity::ReleaseStaticType()).
///
/// @param[in] pType  Type to unregister.  References to the parent type and the type template will be released as well.
///
/// @see Register()
void GameObjectType::Unregister( const GameObjectType* pType )
{
    HELIUM_ASSERT( pType );

    // Unregister all child types first.
    const Composite* pDerived = pType->m_FirstDerived;
    while( pDerived )
    {
        HELIUM_ASSERT( pDerived->GetReflectionType() == Reflect::ReflectionTypes::GameObjectType );
        const GameObjectType* pDerivedGameObjectType = static_cast< const GameObjectType* >( pDerived );
        const Composite* pNextDerived = pDerived->m_NextSibling;

        RELEASE_STATIC_TYPE_CALLBACK* pReleaseStaticType = pDerivedGameObjectType->m_pReleaseStaticTypeCallback;
        HELIUM_ASSERT( pReleaseStaticType );
        pReleaseStaticType();

        pDerived = pNextDerived;
    }

    // Remove this type from all type registries.
    Reflect::Registry* pRegistry = Reflect::Registry::GetInstance();
    HELIUM_ASSERT( pRegistry );
    pRegistry->UnregisterType( pType );

    HELIUM_ASSERT( sm_pLookupMap );
    HELIUM_VERIFY( sm_pLookupMap->Remove( pType->GetName() ) );
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
