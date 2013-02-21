#include "EnginePch.h"
#include "Engine/AssetType.h"

#include "Foundation/ObjectPool.h"
#include "Reflect/Registry.h"
#include "Engine/AssetPointerData.h"

#include "Engine/Package.h"

using namespace Helium;

PackagePtr AssetType::sm_spTypePackage;
AssetType::LookupMap* AssetType::sm_pLookupMap = NULL;

/// Constructor.
AssetType::AssetType()
    : m_class( NULL )
    , m_flags( 0 )
{
}

/// Destructor.
AssetType::~AssetType()
{
}

/// Set the package in which all template object packages are stored.
///
/// @param[in] pPackage  Main type package.
///
/// @see GetTypePackage()
void AssetType::SetTypePackage( Package* pPackage )
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
// PMD: Removing const because:
// - Objects must be able to have properties of the same type as the outer type (i.e. Asset has reference to Asset that is the template)
// - So, s_Class must be set before calling PopulateComposite
// - So, this function must return a pointer that PopulateComposite can work on, rather than calling PopulateComposite directly
//   - If not for this restriction, I'd want to see if we could call Class::Create and Composite::Create, rather than doing duplicate set-up work here
// - To prevent un-consting parameter to PopulateComposite, making AssetType return non-const
AssetType* AssetType::Create(
    const Reflect::Class* pClass,
    Package* pTypePackage,
    const AssetType* pParent,
    Asset* pTemplate,
    uint32_t flags )
{
    HELIUM_ASSERT( pClass );
    HELIUM_ASSERT( pTypePackage );
    HELIUM_ASSERT( pTemplate );

    Name name;
    name.Set( pClass->m_Name );
    HELIUM_ASSERT( !name.IsEmpty() );

    // Register the template object with the object system.
    if( !Asset::RegisterObject( pTemplate ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "AssetType::Initialize(): Failed to register type \"%s\" template object.\n" ),
            *name );

        return false;
    }

    // Set up the template object name, and set this object as its parent.
    Asset::RenameParameters nameParameters;
    nameParameters.name = name;
    nameParameters.spOwner = pTypePackage;
    if( !pTemplate->Rename( nameParameters ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "AssetType::Initialize(): Failed to set type \"%s\" template object name and owner.\n" ),
            *name );

        Asset::UnregisterObject( pTemplate );

        return false;
    }

    // Flag the object as the default template object for the type being created.
    pTemplate->SetFlags( Asset::FLAG_DEFAULT_TEMPLATE );

    // Create the type object and store its parameters.
    AssetType* pType = new AssetType;
    HELIUM_ASSERT( pType );
    pType->m_class = pClass;
    pClass->m_Tag = pType;
    const_cast< Reflect::Class* >( pType->m_class )->m_Default = pTemplate;
    const_cast< Reflect::Class* >( pType->m_class )->Composite::m_Default = pTemplate;
    pType->m_name = name;
    pType->m_flags = flags;

    // Lazily initialize the lookup map.  Note that this is not inherently thread-safe, but there should always be
    // at least one type registered before any sub-threads are spawned.
    if( !sm_pLookupMap )
    {
        sm_pLookupMap = new LookupMap;
        HELIUM_ASSERT( sm_pLookupMap );
    }

    // Register the type (note that a type with the same name should not already exist in the lookup map).
    LookupMap::Iterator typeIterator;
    HELIUM_VERIFY( sm_pLookupMap->Insert( typeIterator, KeyValue< Name, AssetTypePtr >( pType->GetName(), pType ) ) );

    return pType;
}

/// Unregister a type.
///
/// Note that this should only be called by the static type release functions (i.e. Entity::ReleaseStaticType()).
///
/// @param[in] pType  Type to unregister.  References to the parent type and the type template will be released as well.
///
/// @see Register()
void AssetType::Unregister( const AssetType* pType )
{
    HELIUM_ASSERT( pType );

    HELIUM_ASSERT( sm_pLookupMap );
    HELIUM_VERIFY( sm_pLookupMap->Remove( pType->GetName() ) );
}

/// Look up a type by name.
///
/// @param[in] typeName  Name of the type to look up.
///
/// @return  Pointer to the specified type if found, null pointer if not found.
AssetType* AssetType::Find( Name typeName )
{
    AssetType* pType = NULL;
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
AssetType::ConstIterator AssetType::GetTypeBegin()
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
AssetType::ConstIterator AssetType::GetTypeEnd()
{
    if( sm_pLookupMap )
    {
        return ConstIterator( sm_pLookupMap->End() );
    }

    ConstIterator nullIterator;
    MemoryZero( &nullIterator, sizeof( nullIterator ) );

    return nullIterator;
}

/// Perform shutdown of the AssetType registration system.
///
/// This releases all final references to objects and releases all allocated memory.  This should be called during
/// the shutdown process prior to calling Asset::Shutdown().
///
/// @see Asset::Shutdown()
void AssetType::Shutdown()
{
    HELIUM_TRACE( TraceLevels::Info, TXT( "Shutting down AssetType registration.\n" ) );

    // Make sure the Asset type is unregistered, as it does not get included in the unregistration of the Engine
    // type package.
    Asset::ReleaseStaticType();

    delete sm_pLookupMap;
    sm_pLookupMap = NULL;

    // Release the reference to the main "Types" package.
    sm_spTypePackage.Release();

    HELIUM_TRACE( TraceLevels::Info, TXT( "AssetType registration shutdown complete.\n" ) );
}