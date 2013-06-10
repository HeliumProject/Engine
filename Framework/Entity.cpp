#include "FrameworkPch.h"
#include "Framework/Entity.h"

#include "Framework/Slice.h"
#include "Foundation/Log.h"

using namespace Helium;

REFLECT_DEFINE_OBJECT(Helium::Entity);

Entity::~Entity()
{
	m_Components.ReleaseAll();
}

void Helium::Entity::PopulateStructure( Reflect::Structure& comp )
{

}

World * Helium::Entity::GetWorld()
{
	return m_spSlice ? m_spSlice->GetWorld() : NULL;
}

/// Set the slice to which this entity is currently bound, along with the index of this entity within the slice.
///
/// @param[in] pSlice      SceneDefinition to set.
/// @param[in] sliceIndex  Index within the slice to set.
///
/// @see SetSliceIndex(), GetSlice(), GetSliceIndex(), ClearSliceInfo()
void Entity::SetSliceInfo( Slice* pSlice, size_t sliceIndex )
{
	HELIUM_ASSERT( pSlice );
	HELIUM_ASSERT( IsValid( sliceIndex ) );

	m_spSlice = pSlice;
	m_sliceIndex = sliceIndex;
}

/// Update the index of this entity within its slice.
///
/// @param[in] sliceIndex  Index within the slice to set.
///
/// @see SetSliceInfo(), GetSlice(), GetSliceIndex(), ClearSliceInfo()
void Entity::SetSliceIndex( size_t sliceIndex )
{
	HELIUM_ASSERT( m_spSlice );
	HELIUM_ASSERT( IsValid( sliceIndex ) );

	m_sliceIndex = sliceIndex;
}

/// Clear out any currently set slice binding information.
///
/// @see SetSliceInfo(), SetSliceIndex(), GetSlice(), GetSliceIndex()
void Entity::ClearSliceInfo()
{
	m_spSlice.Release();
	SetInvalid( m_sliceIndex );
}

ComponentCollection & Helium::Entity::VirtualGetComponents()
{
	return GetComponents();
}

World * Helium::Entity::VirtualGetWorld()
{
	return GetWorld();
}
