#include "Precompile.h"
#include "Framework/SceneDefinition.h"

#include "Framework/World.h"
#include "Framework/Entity.h"
#include "Framework/EntityDefinition.h"

using namespace Helium;

#if HELIUM_TOOLS
HELIUM_DEFINE_CLASS( Helium::SceneDefinition::ToolsData );
#endif

HELIUM_IMPLEMENT_ASSET( Helium::SceneDefinition, Framework, 0 );

void Helium::SceneDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &SceneDefinition::m_WorldDefinition, "m_WorldDefinition" );
	comp.AddField( &SceneDefinition::m_Entities, "m_Entities" );

#if HELIUM_TOOLS
	comp.AddField( &SceneDefinition::m_ToolsData, "m_ToolsData" );
#endif
}

/// Constructor.
SceneDefinition::SceneDefinition()
{
}

/// Destructor.
SceneDefinition::~SceneDefinition()
{
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
    const AssetType* pType,
    EntityDefinition* pTemplate,
    Name name,
    bool bAssignInstanceIndex )
{
    HELIUM_ASSERT( pType );
    if( !pType )
    {
        HELIUM_TRACE( TraceLevels::Error, "SceneDefinition::CreateEntity(): No entity type specified.\n" );

        return NULL;
    }

    bool bIsEntityType = pType->GetMetaClass()->IsType( EntityDefinition::GetStaticType()->GetMetaClass() );
    HELIUM_ASSERT( bIsEntityType );
    if( !bIsEntityType )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "SceneDefinition::CreateEntity(): AssetType \"%s\" specified is not an entity type.\n",
            *pType->GetName() );

        return NULL;
    }

    if( name.IsEmpty() )
    {
        name = pType->GetName();
    }

    AssetPtr spObject;
    if( !Asset::CreateObject( spObject, pType, name, GetOwningPackage(), pTemplate, bAssignInstanceIndex ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "SceneDefinition::CreateEntity(): Failed to create entity definition \"%s\" of type \"%s\" in slice package \"%s\" (template: %s; assign instance index: %s).\n",
            *name,
            *pType->GetName(),
            *GetOwningPackage()->GetPath().ToString(),
            ( pTemplate ? *pTemplate->GetPath().ToString() : "none" ),
            ( bAssignInstanceIndex ? "yes" : "no" ) );

        return NULL;
    }

    EntityDefinition* pEntity = Reflect::AssertCast< EntityDefinition >( spObject.Get() );
    HELIUM_ASSERT( pEntity );

    //size_t sliceIndex = m_Entities.Push( pEntity );
    //HELIUM_ASSERT( IsValid( sliceIndex ) );
    //pEntity->SetSliceInfo( this, sliceIndex );

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
//     if( pEntity->GetSlice().Get() != this )
//     {
//         HELIUM_TRACE(
//             TraceLevels::Error,
//             "SceneDefinition::DestroyEntity(): EntityDefinition \"%s\" is not part of slice \"%s\".\n",
//             *pEntity->GetPath().ToString(),
//             *GetPath().ToString() );
// 
//         return false;
//     }

    // Clear the entity's references back to this slice and remove it from the entity list.
//     size_t index = pEntity->GetSliceIndex();
//     HELIUM_ASSERT( index < m_Entities.GetSize() );

//     pEntity->ClearSliceInfo();
//     m_Entities.RemoveSwap( index );

    // Update the index of the entity which has been moved to fill the entity list entry we just removed.
//     size_t entityCount = m_Entities.GetSize();
//     if( index < entityCount )
//     {
//         EntityDefinition* pMovedEntity = m_Entities[ index ];
//         HELIUM_ASSERT( pMovedEntity );
//         HELIUM_ASSERT( pMovedEntity->GetSliceIndex() == entityCount );
//         pMovedEntity->SetSliceIndex( index );
//     }

    return true;
}
