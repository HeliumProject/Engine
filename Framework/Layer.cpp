//----------------------------------------------------------------------------------------------------------------------
// Layer.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/Layer.h"

#include "Framework/World.h"

using namespace Lunar;

L_IMPLEMENT_OBJECT( Layer, Framework, 0 );

/// Constructor.
Layer::Layer()
: m_worldIndex( Invalid< size_t >() )
{
}

/// Destructor.
Layer::~Layer()
{
    HELIUM_ASSERT( !m_spWorld );
}

/// @copydoc GameObject::Serialize()
void Layer::Serialize( Serializer& s )
{
    L_SERIALIZE_BASE( s );

    s << L_TAGGED( m_spPackage );

    // Serialize entities manually when linking so that we can update their layer references at the same time.
    s << Serializer::Tag( TXT( "m_entities" ) );

    bool bLinking = ( s.GetMode() == Serializer::MODE_LINK );
    if( bLinking )
    {
        s.BeginDynArray();

        uint32_t entityCount = static_cast< uint32_t >( m_entities.GetSize() );
        s << entityCount;
        HELIUM_ASSERT( entityCount == m_entities.GetSize() );

        for( uint32_t entityIndex = 0; entityIndex < entityCount; ++entityIndex )
        {
            EntityPtr& rspEntity = m_entities[ entityIndex ];
            s << rspEntity;

            Entity* pEntity = rspEntity;
            if( pEntity )
            {
                HELIUM_ASSERT( pEntity->GetLayer().Get() == NULL );
                pEntity->SetLayerInfo( this, entityIndex );
            }
        }

        s.EndDynArray();
    }
    else
    {
        s << Serializer::WrapDynArray( m_entities );
    }

#if L_DEBUG
    size_t entityCount = m_entities.GetSize();
    HELIUM_UNREF( entityCount );
    StripNonPackageEntities();
    HELIUM_ASSERT_MSG( entityCount == m_entities.GetSize(), TXT( "Layer contained non-package entities." ) );
#endif
}

/// Bind a package with this layer.
///
/// When a package is initially bound, all entities currently within the package will also be registered with the
/// layer.  Entities belonging to the any previously bound package will be unregistered.  Nothing will happen if the
/// given package is already bound to this layer.
///
/// @param[in] pPackage  Package to bind.
///
/// @see GetPackage()
void Layer::BindPackage( Package* pPackage )
{
    if( m_spPackage.Get() == pPackage )
    {
        return;
    }

    m_spPackage = pPackage;
    AddPackageEntities();
}

/// Create an entity within this layer and store it in the entity list.
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
Entity* Layer::CreateEntity(
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
            TRACE_ERROR,
            TXT( "Layer::CreateEntity(): Layer \"%s\" is not bound to a package.\n" ),
            *GetPath().ToString() );

        return NULL;
    }

    HELIUM_ASSERT( pType );
    if( !pType )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "Layer::CreateEntity(): No entity type specified.\n" ) );

        return NULL;
    }

    bool bIsEntityType = pType->IsType( Entity::GetStaticType() );
    HELIUM_ASSERT( bIsEntityType );
    if( !bIsEntityType )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "Layer::CreateEntity(): GameObjectType \"%s\" specified is not an entity type.\n" ),
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
            TRACE_ERROR,
            ( TXT( "Layer::CreateEntity(): Failed to create entity \"%s\" of type \"%s\" in layer package \"%s\" " )
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

    size_t layerIndex = m_entities.Push( pEntity );
    HELIUM_ASSERT( IsValid( layerIndex ) );
    pEntity->SetLayerInfo( this, layerIndex );

    return pEntity;
}

/// Destroy an entity in this layer.
///
/// @param[in] pEntity  Entity to destroy.
///
/// @return  True if entity destruction was successful, false if not.
///
/// @see CreateEntity()
bool Layer::DestroyEntity( Entity* pEntity )
{
    HELIUM_ASSERT( pEntity );

    // Make sure the entity is part of this layer.
    if( pEntity->GetLayer().Get() != this )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "Layer::DestroyEntity(): Entity \"%s\" is not part of layer \"%s\".\n" ),
            *pEntity->GetPath().ToString(),
            *GetPath().ToString() );

        return false;
    }

    // Clear the entity's references back to this layer and remove it from the entity list.
    size_t index = pEntity->GetLayerIndex();
    HELIUM_ASSERT( index < m_entities.GetSize() );

    pEntity->ClearLayerInfo();
    m_entities.RemoveSwap( index );

    // Update the index of the entity which has been moved to fill the entity list entry we just removed.
    size_t entityCount = m_entities.GetSize();
    if( index < entityCount )
    {
        Entity* pMovedEntity = m_entities[ index ];
        HELIUM_ASSERT( pMovedEntity );
        HELIUM_ASSERT( pMovedEntity->GetLayerIndex() == entityCount );
        pMovedEntity->SetLayerIndex( index );
    }

    return true;
}

/// Set the world to which this layer is currently bound, along with the index of this layer within the world.
///
/// @param[in] pWorld      World to set.
/// @param[in] worldIndex  Index within the world to set.
///
/// @see SetWorldIndex(), GetWorld(), GetWorldIndex(), ClearWorldInfo()
void Layer::SetWorldInfo( World* pWorld, size_t worldIndex )
{
    HELIUM_ASSERT( pWorld );
    HELIUM_ASSERT( IsValid( worldIndex ) );

    m_spWorld = pWorld;
    m_worldIndex = worldIndex;
}

/// Update the index of this layer within its world.
///
/// @param[in] worldIndex  Index within the world to set.
///
/// @see SetWorldInfo(), GetWorld(), GetWorldIndex(), ClearWorldInfo()
void Layer::SetWorldIndex( size_t worldIndex )
{
    HELIUM_ASSERT( m_spWorld );
    HELIUM_ASSERT( IsValid( worldIndex ) );

    m_worldIndex = worldIndex;
}

/// Clear out any currently set world binding information.
///
/// @see SetWorldInfo(), SetWorldIndex(), GetWorld(), GetWorldIndex()
void Layer::ClearWorldInfo()
{
    m_spWorld.Release();
    SetInvalid( m_worldIndex );
}

/// Register entities with this layer that are directly part of the bound package.
void Layer::AddPackageEntities()
{
    // Clear out all existing entities.
    size_t entityCount = m_entities.GetSize();
    for( size_t entityIndex = 0; entityIndex < entityCount; ++entityIndex )
    {
        Entity* pEntity = m_entities[ entityIndex ];
        HELIUM_ASSERT( pEntity );
        pEntity->ClearLayerInfo();
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
            HELIUM_ASSERT( spEntity->GetLayer().Get() == NULL );

            size_t entityIndex = m_entities.Push( spEntity );
            HELIUM_ASSERT( IsValid( entityIndex ) );
            spEntity->SetLayerInfo( this, entityIndex );
        }
    }
}

/// Unregister entities in this layer than are not directly part of the bound package.
void Layer::StripNonPackageEntities()
{
    // If no package is bound, no entities should be bound.
    Package* pPackage = m_spPackage;
    if( !pPackage )
    {
        if( !m_entities.IsEmpty() )
        {
            HELIUM_TRACE(
                TRACE_WARNING,
                ( TXT( "Layer::StripNonPackageEntities(): Layer contains %" ) TPRIuSZ TXT( " entities, but has " )
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
                TRACE_WARNING,
                ( TXT( "Layer::StripNonPackageEntities(): Entity \"%s\" is not directly part of the bound " )
                TXT( "package \"%s\".  Entity will be removed.\n" ) ),
                *pEntity->GetPath().ToString(),
                *pPackage->GetPath().ToString() );

            pEntity->ClearLayerInfo();
            m_entities.RemoveSwap( entityIndex );

            --entityIndex;
            --entityCount;
        }
    }
}
