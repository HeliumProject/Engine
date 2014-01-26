#include "FrameworkPch.h"
#include "Framework/World.h"

#include "Rendering/Renderer.h"
#include "Rendering/RSurface.h"
#include "Framework/EntityDefinition.h"
#include "Framework/Slice.h"
#include "Framework/Entity.h"
#include "Framework/SceneDefinition.h"

namespace Helium
{
	class World;
	typedef Helium::StrongPtr< World > WorldPtr;
	typedef Helium::StrongPtr< const World > ConstWorldPtr;
}

using namespace Helium;

HELIUM_DEFINE_CLASS( Helium::World );

/// Constructor.
World::World()
{
}

/// Destructor.
World::~World()
{
	//HELIUM_ASSERT( m_Slices.IsEmpty() );
}

/// Initialize this world instance.
///
/// @return  True if initialization was successful, false if not.
///
/// @see Shutdown()
bool World::Initialize()
{
	HELIUM_ASSERT( m_Slices.IsEmpty() );

	m_ComponentManager.Reset( Components::CreateManager( this ) );
	
	m_RootSlice = Reflect::AssertCast<Slice>(Slice::CreateObject());
	HELIUM_ASSERT( m_RootSlice );
	if( !m_RootSlice )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "World::Initialize(): Failed to create a root slice.\n" ) );

		return false;
	}

	AddSlice(m_RootSlice);

	return true;
}

/// Shut down this world instance.
///
/// @see Initialize()
void World::Shutdown()
{
	// Remove all slices first.
	while( !m_Slices.IsEmpty() )
	{
		Slice* pSlice = m_Slices.GetLast();
		HELIUM_ASSERT( pSlice );
		HELIUM_VERIFY( RemoveSlice( pSlice ) );
	}

	m_RootSlice.Set( NULL );

	m_Components.ReleaseAll();
}

ComponentCollection& Helium::World::VirtualGetComponents()
{
	return m_Components;
}

ComponentManager* Helium::World::VirtualGetComponentManager()
{
	return GetComponentManager();
}

Slice *World::GetRootSlice()
{
	return m_RootSlice;
}

/// @copydoc Asset::PreDestroy()
void World::RefCountPreDestroy()
{
	Shutdown();

	Base::RefCountPreDestroy();
}

/// Create an entity in this world.
///
/// @param[in] pSlice                SceneDefinition in which to create the entity.
/// @param[in] pType                 EntityDefinition type.
/// @param[in] rPosition             EntityDefinition position.
/// @param[in] rRotation             EntityDefinition rotation.
/// @param[in] rScale                EntityDefinition scale.
/// @param[in] pTemplate             Template from which to create the entity.
/// @param[in] name                  Asset name to assign to the entity, or a null name to automatically generate a
///                                  name based on the entity type.
/// @param[in] bAssignInstanceIndex  True to assign an instance index to the entity, false to not include an
///                                  instance index.
///
/// @return  Pointer to the entity instance if created successfully, null if not.
///
/// @see DestroyEntity()
// EntityDefinition* World::CreateEntity(
//     SceneDefinition* pSlice,
//     Entity* pEntity)
// {
//     return 0;
// 
//     //// Make sure the destination slice is valid.
//     //HELIUM_ASSERT( pSlice );
//     //if( !pSlice )
//     //{
//     //    HELIUM_TRACE( TraceLevels::Error, TXT( "World::CreateEntity(): Missing entity slice.\n" ) );
// 
//     //    return NULL;
//     //}
// 
//     //World* pSliceWorld = pSlice->GetWorld();
//     //HELIUM_ASSERT( pSliceWorld == this );
//     //if( pSliceWorld != this )
//     //{
//     //    HELIUM_TRACE(
//     //        TraceLevels::Error,
//     //        TXT( "World::CreateEntity(): SceneDefinition \"%s\" is not bound to world \"%s\".\n" ),
//     //        *pSlice->GetPath().ToString(),
//     //        *GetPath().ToString() );
// 
//     //    return NULL;
//     //}
// 
//     //// Attempt to create the entity.
//     //EntityDefinition* pEntity = pSlice->CreateEntity(
//     //    pType,
//     //    rPosition,
//     //    rRotation,
//     //    rScale,
//     //    pTemplate,
//     //    name,
//     //    bAssignInstanceIndex );
//     //if( !pEntity )
//     //{
//     //    HELIUM_TRACE(
//     //        TraceLevels::Error,
//     //        TXT( "World::CreateEntity(): Failed to create entity in world \"%s\", slice \"%s\".\n" ),
//     //        *GetPath().ToString(),
//     //        *pSlice->GetPath().ToString() );
// 
//     //    return NULL;
//     //}
// 
//     //return pEntity;
// }

/// Destroy an entity in this world.
///
/// @param[in] pEntity  EntityDefinition to destroy.
///
/// @return  True if the entity was destroyed successfully, false if not.
///
/// @see CreateEntity()
// bool World::DestroyEntity( Entity* pEntity )
// {
//     return false;
//     //HELIUM_ASSERT( pEntity );
// 
//     //// Get the entity slice and make sure the entity is part of this world.
//     //SlicePtr spEntitySlice( pEntity->GetSlice() );
//     //if( !spEntitySlice )
//     //{
//     //    HELIUM_TRACE(
//     //        TraceLevels::Error,
//     //        TXT( "World::DestroyEntity(): EntityDefinition \"%s\" is not bound to a slice.\n" ),
//     //        *pEntity->GetPath().ToString() );
// 
//     //    return false;
//     //}
// 
//     //WorldPtr spEntityWorld( spEntitySlice->GetWorld() );
//     //if( spEntityWorld.Get() != this )
//     //{
//     //    HELIUM_TRACE(
//     //        TraceLevels::Error,
//     //        TXT( "World::DestroyEntity(): EntityDefinition \"%s\" is not part of world \"%s\".\n" ),
//     //        *pEntity->GetPath().ToString() );
// 
//     //    return false;
//     //}
// 
//     //bool bDestroyResult = spEntitySlice->DestroyEntity( pEntity );
// 
//     //return bDestroyResult;
// }

/// Add a slice to this world.
///
/// @param[in] pSlice  SceneDefinition to add.
///
/// @return  True if the slice was added successfully, false if not.
///
/// @see RemoveSlice()
bool World::AddSlice( Slice* pSlice )
{
	// Make sure a valid slice not already attached to a world was specified.
	HELIUM_ASSERT( pSlice );
	if( !pSlice )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "World::AddSlice(): Null slice specified.\n" ) );

		return false;
	}

	World* pExistingWorld = pSlice->GetWorld();
	HELIUM_ASSERT( !pExistingWorld );
	if( pExistingWorld )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "World::AddSlice(): SceneDefinition \"%s\" is already bound to world.\n" ),
			*pSlice->GetSceneDefinition()->GetPath().ToString() );

		return false;
	}

	// Add the slice to our slice list and set it referencing back to this world.
	size_t sliceIndex = m_Slices.Push( SlicePtr( pSlice ) );
	HELIUM_ASSERT( IsValid( sliceIndex ) );
	pSlice->SetWorldInfo( this, sliceIndex );

	// Attach all entities in the slice.
	//size_t entityCount = pSlice->GetEntityCount();
	//for( size_t entityIndex = 0; entityIndex < entityCount; ++entityIndex )
	//{
	//    Entity* pEntity = pSlice->GetEntity( entityIndex );
	//    HELIUM_ASSERT( pEntity );

	//    pEntity->Attach()
	//}

	return true;
}

/// Remove a slice from this world.
///
/// @param[in] pSlice  SceneDefinition to remove.
///
/// @return  True if the slice was removed successfully, false if not.
///
/// @see AddSlice()
bool World::RemoveSlice( Slice* pSlice )
{
	HELIUM_ASSERT( pSlice );

	// Make sure the slice is part of this world.
	if( pSlice->GetWorld() != this )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "World::RemoveSlice(): SceneDefinition \"%s\" is not part of world.\n" ),
			*pSlice->GetSceneDefinition()->GetPath().ToString() );

		return false;
	}

	//// Detach all entities in the slice.
	//size_t entityCount = pSlice->GetEntityCount();
	//for( size_t entityIndex = 0; entityIndex < entityCount; ++entityIndex )
	//{
	//    Entity* pEntity = pSlice->GetEntity( entityIndex );
	//    HELIUM_ASSERT( pEntity );
	//}

	// Remove the slice from the slice list and clear out all references back to this world.
	size_t index = pSlice->GetWorldIndex();
	HELIUM_ASSERT( index < m_Slices.GetSize() );

	pSlice->ClearWorldInfo();
	m_Slices.RemoveSwap( index );

	// Update the index of the slice which has been moved to fill the slice list entry we just removed.
	size_t sliceCount = m_Slices.GetSize();
	if( index < sliceCount )
	{
		Slice* pMovedSlice = m_Slices[ index ];
		HELIUM_ASSERT( pMovedSlice );
		HELIUM_ASSERT( pMovedSlice->GetWorldIndex() == sliceCount );
		pMovedSlice->SetWorldIndex( index );
	}

	return true;
}

/// Get the slice associated with the given index in this world.
///
/// @param[in] index  SceneDefinition index.
///
/// @return  SceneDefinition instance.
///
/// @see GetSliceCount()
Slice* World::GetSlice( size_t index ) const
{
	HELIUM_ASSERT( index < m_Slices.GetSize() );

	return m_Slices[ index ];
}
