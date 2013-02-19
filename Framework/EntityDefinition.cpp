#include "FrameworkPch.h"
#include "Framework/EntityDefinition.h"

#include "Framework/Slice.h"
#include "Framework/FrameworkDataDeduction.h"

HELIUM_IMPLEMENT_OBJECT( Helium::EntityDefinition, Framework, 0 );

using namespace Helium;

/// Constructor.
EntityDefinition::EntityDefinition()
: m_position( 0.0f )
, m_rotation( Simd::Quat::IDENTITY )
, m_scale( 1.0f )
, m_sliceIndex( Invalid< size_t >() )
{
}

/// Destructor.
EntityDefinition::~EntityDefinition()
{
    HELIUM_ASSERT( !m_spSlice );
}

/// Directly set the position of this entity.
///
/// Note that this interface does not guarantee proper updating of scenes in which this entity belongs.
///
/// @param[in] rPosition  Position to set.
///
/// @see GetPosition(), SetRotation(), SetScale()
void EntityDefinition::SetPosition( const Simd::Vector3& rPosition )
{
    m_position = rPosition;
}

/// Directly set the rotation of this entity.
///
/// Note that this interface does not guarantee proper updating of scenes in which this entity belongs.
///
/// @param[in] rRotation  Rotation to set.
///
/// @see GetRotation(), SetPosition(), SetScale()
void EntityDefinition::SetRotation( const Simd::Quat& rRotation )
{
    m_rotation = rRotation;
}

/// Directly set the scale of this entity.
///
/// Note that this interface does not guarantee proper updating of scenes in which this entity belongs.
///
/// @param[in] rScale  Scale to set.
///
/// @see GetScale(), SetPosition(), SetRotation()
void EntityDefinition::SetScale( const Simd::Vector3& rScale )
{
    m_scale = rScale;
}

/// Set the slice to which this entity is currently bound, along with the index of this entity within the slice.
///
/// @param[in] pSlice      SliceDefinition to set.
/// @param[in] sliceIndex  Index within the slice to set.
///
/// @see SetSliceIndex(), GetSlice(), GetSliceIndex(), ClearSliceInfo()
void EntityDefinition::SetSliceInfo( SliceDefinition* pSlice, size_t sliceIndex )
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
void EntityDefinition::SetSliceIndex( size_t sliceIndex )
{
    HELIUM_ASSERT( m_spSlice );
    HELIUM_ASSERT( IsValid( sliceIndex ) );

    m_sliceIndex = sliceIndex;
}

/// Clear out any currently set slice binding information.
///
/// @see SetSliceInfo(), SetSliceIndex(), GetSlice(), GetSliceIndex()
void EntityDefinition::ClearSliceInfo()
{
    m_spSlice.Release();
    SetInvalid( m_sliceIndex );
}

Helium::EntityPtr Helium::EntityDefinition::CreateEntity()
{
    throw std::exception("The method or operation is not implemented.");
}
