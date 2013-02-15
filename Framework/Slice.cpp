#include "FrameworkPch.h"
#include "Framework/Slice.h"

#include "Framework/World.h"

HELIUM_IMPLEMENT_OBJECT( Helium::Slice, Framework, 0 );

using namespace Helium;

/// Constructor.
Slice::Slice()
: m_worldIndex( Invalid< size_t >() )
{
}

/// Destructor.
Slice::~Slice()
{
    HELIUM_ASSERT( !m_spWorld );
}

//PMDTODO: Implement this
///// @copydoc GameObject::Serialize()
//void Slice::Serialize( Serializer& s )
//{
//    HELIUM_SERIALIZE_BASE( s );
//
//    s << HELIUM_TAGGED( m_spPackage );
//
//    // Serialize entities manually when linking so that we can update their slice references at the same time.
//    s << Serializer::Tag( TXT( "m_entities" ) );
//
//    bool bLinking = ( s.GetMode() == Serializer::MODE_LINK );
//    if( bLinking )
//    {
//        s.BeginDynamicArray();
//
//        uint32_t entityCount = static_cast< uint32_t >( m_entities.GetSize() );
//        s << entityCount;
//        HELIUM_ASSERT( entityCount == m_entities.GetSize() );
//
//        for( uint32_t entityIndex = 0; entityIndex < entityCount; ++entityIndex )
//        {
//            EntityPtr& rspEntity = m_entities[ entityIndex ];
//            s << rspEntity;
//
//            Entity* pEntity = rspEntity;
//            if( pEntity )
//            {
//                HELIUM_ASSERT( pEntity->GetSlice().Get() == NULL );
//                pEntity->SetSliceInfo( this, entityIndex );
//            }
//        }
//
//        s.EndDynamicArray();
//    }
//    else
//    {
//        s << Serializer::WrapDynamicArray( m_entities );
//    }
//
//#if HELIUM_DEBUG
//    size_t entityCount = m_entities.GetSize();
//    HELIUM_UNREF( entityCount );
//    StripNonPackageEntities();
//    HELIUM_ASSERT_MSG( entityCount == m_entities.GetSize(), TXT( "Slice contained non-package entities." ) );
//#endif
//}

/// Bind a package with this slice.
///
/// When a package is initially bound, all entities currently within the package will also be registered with the
/// slice.  Entities belonging to the any previously bound package will be unregistered.  Nothing will happen if the
/// given package is already bound to this slice.
///
/// @param[in] pPackage  Package to bind.
///
/// @see GetPackage()
void Slice::BindPackage( Package* pPackage )
{
    if( m_spPackage.Get() == pPackage )
    {
        return;
    }

    m_spPackage = pPackage;
    AddPackageEntities();
}

/// Create an entity within this slice and store it in the entity list.
///
/// @param[in] pType                 Type of entity to create.
/// @param[in] rPosition             Entity position.
/// @param[in] rRotation             Entity rotation.
/// @param[in] rScale                Entity scale.
/// @param[in] pTemplate             Template from which to create the entity.
/// @param[in] name                  Object name to assign to the entity, or a null name to automatically generate a
///                                  name based on the entity type.
/// @param[in] bAssignInstanceIndex  True to assign an instance index to the entity, false to not include an
///                                  instance index.
///
/// @return  Pointer to the entity instance if created successfully, null if not.
///
/// @see DestroyEntity()
Entity* Slice::CreateEntity(
    const GameObjectType* pType,
    const Simd::Vector3& rPosition,
    const Simd::Quat& rRotation,
    const Simd::Vector3& rScale,
    Entity* pTemplate,
    Name name,
    bool bAssignInstanceIndex )
{
    HELIUM_ASSERT( m_spPackage );
    if( !m_spPackage )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "Slice::CreateEntity(): Slice \"%s\" is not bound to a package.\n" ),
            *GetPath().ToString() );

        return NULL;
    }

    HELIUM_ASSERT( pType );
    if( !pType )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "Slice::CreateEntity(): No entity type specified.\n" ) );

        return NULL;
    }

    bool bIsEntityType = pType->GetClass()->IsType( Entity::GetStaticType()->GetClass() );
    HELIUM_ASSERT( bIsEntityType );
    if( !bIsEntityType )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "Slice::CreateEntity(): GameObjectType \"%s\" specified is not an entity type.\n" ),
            *pType->GetName() );

        return NULL;
    }

    if( name.IsEmpty() )
    {
        name = pType->GetName();
    }

    GameObjectPtr spObject;
    if( !GameObject::CreateObject( spObject, pType, name, m_spPackage, pTemplate, bAssignInstanceIndex ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "Slice::CreateEntity(): Failed to create entity \"%s\" of type \"%s\" in slice package \"%s\" " )
              TXT( "(template: %s; assign instance index: %s).\n" ) ),
            *name,
            *pType->GetName(),
            *m_spPackage->GetPath().ToString(),
            ( pTemplate ? *pTemplate->GetPath().ToString() : TXT( "none" ) ),
            ( bAssignInstanceIndex ? TXT( "yes" ) : TXT( "no" ) ) );

        return NULL;
    }

    Entity* pEntity = Reflect::AssertCast< Entity >( spObject.Get() );
    HELIUM_ASSERT( pEntity );

    pEntity->SetPosition( rPosition );
    pEntity->SetRotation( rRotation );
    pEntity->SetScale( rScale );

    size_t sliceIndex = m_entities.Push( pEntity );
    HELIUM_ASSERT( IsValid( sliceIndex ) );
    pEntity->SetSliceInfo( this, sliceIndex );

    return pEntity;
}

/// Destroy an entity in this slice.
///
/// @param[in] pEntity  Entity to destroy.
///
/// @return  True if entity destruction was successful, false if not.
///
/// @see CreateEntity()
bool Slice::DestroyEntity( Entity* pEntity )
{
    HELIUM_ASSERT( pEntity );

    // Make sure the entity is part of this slice.
    if( pEntity->GetSlice().Get() != this )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "Slice::DestroyEntity(): Entity \"%s\" is not part of slice \"%s\".\n" ),
            *pEntity->GetPath().ToString(),
            *GetPath().ToString() );

        return false;
    }

    // Clear the entity's references back to this slice and remove it from the entity list.
    size_t index = pEntity->GetSliceIndex();
    HELIUM_ASSERT( index < m_entities.GetSize() );

    pEntity->ClearSliceInfo();
    m_entities.RemoveSwap( index );

    // Update the index of the entity which has been moved to fill the entity list entry we just removed.
    size_t entityCount = m_entities.GetSize();
    if( index < entityCount )
    {
        Entity* pMovedEntity = m_entities[ index ];
        HELIUM_ASSERT( pMovedEntity );
        HELIUM_ASSERT( pMovedEntity->GetSliceIndex() == entityCount );
        pMovedEntity->SetSliceIndex( index );
    }

    return true;
}

/// Set the world to which this slice is currently bound, along with the index of this slice within the world.
///
/// @param[in] pWorld      World to set.
/// @param[in] worldIndex  Index within the world to set.
///
/// @see SetWorldIndex(), GetWorld(), GetWorldIndex(), ClearWorldInfo()
void Slice::SetWorldInfo( World* pWorld, size_t worldIndex )
{
    HELIUM_ASSERT( pWorld );
    HELIUM_ASSERT( IsValid( worldIndex ) );

    m_spWorld = pWorld;
    m_worldIndex = worldIndex;
}

/// Update the index of this slice within its world.
///
/// @param[in] worldIndex  Index within the world to set.
///
/// @see SetWorldInfo(), GetWorld(), GetWorldIndex(), ClearWorldInfo()
void Slice::SetWorldIndex( size_t worldIndex )
{
    HELIUM_ASSERT( m_spWorld );
    HELIUM_ASSERT( IsValid( worldIndex ) );

    m_worldIndex = worldIndex;
}

/// Clear out any currently set world binding information.
///
/// @see SetWorldInfo(), SetWorldIndex(), GetWorld(), GetWorldIndex()
void Slice::ClearWorldInfo()
{
    m_spWorld.Release();
    SetInvalid( m_worldIndex );
}

/// Register entities with this slice that are directly part of the bound package.
void Slice::AddPackageEntities()
{
    // Clear out all existing entities.
    size_t entityCount = m_entities.GetSize();
    for( size_t entityIndex = 0; entityIndex < entityCount; ++entityIndex )
    {
        Entity* pEntity = m_entities[ entityIndex ];
        HELIUM_ASSERT( pEntity );
        pEntity->ClearSliceInfo();
    }

    m_entities.Clear();

    // If no package is bound, no entities should be added.
    Package* pPackage = m_spPackage;
    if( !pPackage )
    {
        return;
    }

    // Add package entities.
    for( GameObject* pChild = pPackage->GetFirstChild(); pChild != NULL; pChild = pChild->GetNextSibling() )
    {
        EntityPtr spEntity( Reflect::SafeCast< Entity >( pChild ) );
        if( spEntity )
        {
            HELIUM_ASSERT( spEntity->GetSlice().Get() == NULL );

            size_t entityIndex = m_entities.Push( spEntity );
            HELIUM_ASSERT( IsValid( entityIndex ) );
            spEntity->SetSliceInfo( this, entityIndex );
        }
    }
}

/// Unregister entities in this slice than are not directly part of the bound package.
void Slice::StripNonPackageEntities()
{
    // If no package is bound, no entities should be bound.
    Package* pPackage = m_spPackage;
    if( !pPackage )
    {
        if( !m_entities.IsEmpty() )
        {
            HELIUM_TRACE(
                TraceLevels::Warning,
                ( TXT( "Slice::StripNonPackageEntities(): Slice contains %" ) TPRIuSZ TXT( " entities, but has " )
                TXT( "no package bound.  Entities will be removed.\n" ) ),
                m_entities.GetSize() );
        }

        m_entities.Clear();

        return;
    }

    // Remove entities that are not part of the package.
    size_t entityCount = m_entities.GetSize();
    for( size_t entityIndex = 0; entityIndex < entityCount; ++entityIndex )
    {
        Entity* pEntity = m_entities[ entityIndex ];
        HELIUM_ASSERT( pEntity );
        GameObject* pOwner = pEntity->GetOwner();
        if( pOwner != pPackage )
        {
            HELIUM_TRACE(
                TraceLevels::Warning,
                ( TXT( "Slice::StripNonPackageEntities(): Entity \"%s\" is not directly part of the bound " )
                TXT( "package \"%s\".  Entity will be removed.\n" ) ),
                *pEntity->GetPath().ToString(),
                *pPackage->GetPath().ToString() );

            pEntity->ClearSliceInfo();
            m_entities.RemoveSwap( entityIndex );

            --entityIndex;
            --entityCount;
        }
    }
}
