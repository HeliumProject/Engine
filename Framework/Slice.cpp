#include "FrameworkPch.h"
#include "Framework/Slice.h"

#include "Framework/World.h"
#include "Framework/Entity.h"

#include "Framework/SceneDefinition.h"
#include "Framework/EntityDefinition.h"
#include "Framework/WorldDefinition.h"
#include "Framework/ComponentDefinition.h"

using namespace Helium;

HELIUM_DEFINE_CLASS( Helium::Slice );

Slice::Slice()
  : m_worldIndex( Invalid< size_t >() )
{

}

void Slice::Initialize( Helium::SceneDefinition *pSceneDefinition )
{
    m_spSceneDefinition = pSceneDefinition;
}

/// Destructor.
Slice::~Slice()
{
    HELIUM_ASSERT( !m_spWorld );
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
Entity* Slice::CreateEntity(EntityDefinition *pEntityDefinition, ParameterSet *pParameterSet)
{
    HELIUM_ASSERT( pEntityDefinition );
    if( !pEntityDefinition )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "Slice::CreateEntity(): EntityDefinition is NULL.\n" ) );
        return NULL;
    }

    EntityPtr entity = pEntityDefinition->CreateEntity();
    HELIUM_ASSERT( entity.Get() );
    if (!entity)
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "Slice::CreateEntity(): Call to EntityDefinition::CreateEntity failed.\n" ) );
        return NULL;
    }
    
    size_t sliceIndex = m_entities.Push( entity );
    HELIUM_ASSERT( IsValid( sliceIndex ) );
    entity->SetSliceInfo( this, sliceIndex );

    pEntityDefinition->FinalizeEntity(entity, pParameterSet);

    return entity.Get();
}

/// Destroy an entity in this slice.
///
/// @param[in] pEntity  EntityDefinition to destroy.
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
            TXT( "Slice::DestroyEntity(): Entity is not part of slice \"%s\".\n" ),
            *pEntity->GetDefinitionPath().ToString(),
            *GetSceneDefinition()->GetPath().ToString() );

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