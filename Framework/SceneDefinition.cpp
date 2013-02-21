#include "FrameworkPch.h"
#include "Framework/SceneDefinition.h"

#include "Framework/World.h"
#include "Framework/Entity.h"

HELIUM_IMPLEMENT_OBJECT( Helium::SceneDefinition, Framework, 0 );

using namespace Helium;

/// Constructor.
SceneDefinition::SceneDefinition()
{
}

/// Destructor.
SceneDefinition::~SceneDefinition()
{
}

/// Bind a package with this slice.
///
/// When a package is initially bound, all entities currently within the package will also be registered with the
/// slice.  Entities belonging to the any previously bound package will be unregistered.  Nothing will happen if the
/// given package is already bound to this slice.
///
/// @param[in] pPackage  Package to bind.
///
/// @see GetPackage()
void SceneDefinition::BindPackage( Package* pPackage )
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
/// @param[in] rPosition             EntityDefinition position.
/// @param[in] rRotation             EntityDefinition rotation.
/// @param[in] rScale                EntityDefinition scale.
/// @param[in] pTemplate             Template from which to create the entity.
/// @param[in] name                  Object name to assign to the entity, or a null name to automatically generate a
///                                  name based on the entity type.
/// @param[in] bAssignInstanceIndex  True to assign an instance index to the entity, false to not include an
///                                  instance index.
///
/// @return  Pointer to the entity instance if created successfully, null if not.
///
/// @see DestroyEntity()
EntityDefinition* SceneDefinition::AddEntityDefinition(
    const GameObjectType* pType,
    const Simd::Vector3& rPosition,
    const Simd::Quat& rRotation,
    const Simd::Vector3& rScale,
    EntityDefinition* pTemplate,
    Name name,
    bool bAssignInstanceIndex )
{
    HELIUM_ASSERT( m_spPackage );
    if( !m_spPackage )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "SceneDefinition::CreateEntity(): SceneDefinition \"%s\" is not bound to a package.\n" ),
            *GetPath().ToString() );

        return NULL;
    }

    HELIUM_ASSERT( pType );
    if( !pType )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "SceneDefinition::CreateEntity(): No entity type specified.\n" ) );

        return NULL;
    }

    bool bIsEntityType = pType->GetClass()->IsType( EntityDefinition::GetStaticType()->GetClass() );
    HELIUM_ASSERT( bIsEntityType );
    if( !bIsEntityType )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "SceneDefinition::CreateEntity(): GameObjectType \"%s\" specified is not an entity type.\n" ),
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
            ( TXT( "SceneDefinition::CreateEntity(): Failed to create entity definition \"%s\" of type \"%s\" in slice package \"%s\" " )
              TXT( "(template: %s; assign instance index: %s).\n" ) ),
            *name,
            *pType->GetName(),
            *m_spPackage->GetPath().ToString(),
            ( pTemplate ? *pTemplate->GetPath().ToString() : TXT( "none" ) ),
            ( bAssignInstanceIndex ? TXT( "yes" ) : TXT( "no" ) ) );

        return NULL;
    }

    EntityDefinition* pEntity = Reflect::AssertCast< EntityDefinition >( spObject.Get() );
    HELIUM_ASSERT( pEntity );

    pEntity->SetPosition( rPosition );
    pEntity->SetRotation( rRotation );
    pEntity->SetScale( rScale );

    size_t sliceIndex = m_entityDefinitions.Push( pEntity );
    HELIUM_ASSERT( IsValid( sliceIndex ) );
    pEntity->SetSliceInfo( this, sliceIndex );

    return pEntity;
}

/// Destroy an entity in this slice.
///
/// @param[in] pEntity  EntityDefinition to destroy.
///
/// @return  True if entity destruction was successful, false if not.
///
/// @see CreateEntity()
bool SceneDefinition::DestroyEntityDefinition( EntityDefinition* pEntity )
{
    HELIUM_ASSERT( pEntity );

    // Make sure the entity is part of this slice.
    if( pEntity->GetSlice().Get() != this )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "SceneDefinition::DestroyEntity(): EntityDefinition \"%s\" is not part of slice \"%s\".\n" ),
            *pEntity->GetPath().ToString(),
            *GetPath().ToString() );

        return false;
    }

    // Clear the entity's references back to this slice and remove it from the entity list.
    size_t index = pEntity->GetSliceIndex();
    HELIUM_ASSERT( index < m_entityDefinitions.GetSize() );

    pEntity->ClearSliceInfo();
    m_entityDefinitions.RemoveSwap( index );

    // Update the index of the entity which has been moved to fill the entity list entry we just removed.
    size_t entityCount = m_entityDefinitions.GetSize();
    if( index < entityCount )
    {
        EntityDefinition* pMovedEntity = m_entityDefinitions[ index ];
        HELIUM_ASSERT( pMovedEntity );
        HELIUM_ASSERT( pMovedEntity->GetSliceIndex() == entityCount );
        pMovedEntity->SetSliceIndex( index );
    }

    return true;
}

/// Register entities with this slice that are directly part of the bound package.
void SceneDefinition::AddPackageEntities()
{
    // Clear out all existing entities.
    size_t entityCount = m_entityDefinitions.GetSize();
    for( size_t entityIndex = 0; entityIndex < entityCount; ++entityIndex )
    {
        EntityDefinition* pEntity = m_entityDefinitions[ entityIndex ];
        HELIUM_ASSERT( pEntity );
        pEntity->ClearSliceInfo();
    }

    m_entityDefinitions.Clear();

    // If no package is bound, no entities should be added.
    Package* pPackage = m_spPackage;
    if( !pPackage )
    {
        return;
    }

    // Add package entities.
    for( GameObject* pChild = pPackage->GetFirstChild(); pChild != NULL; pChild = pChild->GetNextSibling() )
    {
        EntityDefinitionPtr spEntity( Reflect::SafeCast< EntityDefinition >( pChild ) );
        if( spEntity )
        {
            HELIUM_ASSERT( spEntity->GetSlice().Get() == NULL );

            size_t entityIndex = m_entityDefinitions.Push( spEntity );
            HELIUM_ASSERT( IsValid( entityIndex ) );
            spEntity->SetSliceInfo( this, entityIndex );
        }
    }
}

/// Unregister entities in this slice than are not directly part of the bound package.
void SceneDefinition::StripNonPackageEntities()
{
    // If no package is bound, no entities should be bound.
    Package* pPackage = m_spPackage;
    if( !pPackage )
    {
        if( !m_entityDefinitions.IsEmpty() )
        {
            HELIUM_TRACE(
                TraceLevels::Warning,
                ( TXT( "SceneDefinition::StripNonPackageEntities(): SceneDefinition contains %" ) TPRIuSZ TXT( " entities, but has " )
                TXT( "no package bound.  Entities will be removed.\n" ) ),
                m_entityDefinitions.GetSize() );
        }

        m_entityDefinitions.Clear();

        return;
    }

    // Remove entities that are not part of the package.
    size_t entityCount = m_entityDefinitions.GetSize();
    for( size_t entityIndex = 0; entityIndex < entityCount; ++entityIndex )
    {
        EntityDefinition* pEntity = m_entityDefinitions[ entityIndex ];
        HELIUM_ASSERT( pEntity );
        GameObject* pOwner = pEntity->GetOwner();
        if( pOwner != pPackage )
        {
            HELIUM_TRACE(
                TraceLevels::Warning,
                ( TXT( "SceneDefinition::StripNonPackageEntities(): EntityDefinition \"%s\" is not directly part of the bound " )
                TXT( "package \"%s\".  EntityDefinition will be removed.\n" ) ),
                *pEntity->GetPath().ToString(),
                *pPackage->GetPath().ToString() );

            pEntity->ClearSliceInfo();
            m_entityDefinitions.RemoveSwap( entityIndex );

            --entityIndex;
            --entityCount;
        }
    }
}
