#include "FrameworkPch.h"
#include "Framework/Entity.h"

#include "Framework/Slice.h"

HELIUM_IMPLEMENT_OBJECT( Helium::Entity, Framework, 0 );

void Helium::Entity::PreUpdate( float dt )
{
    Log::Print("Frame %f", dt);
}

using namespace Helium;

/// Constructor.
Entity::Entity()
: m_position( 0.0f )
, m_rotation( Simd::Quat::IDENTITY )
, m_scale( 1.0f )
, m_sliceIndex( Invalid< size_t >() )
{
}

/// Destructor.
Entity::~Entity()
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
void Entity::SetPosition( const Simd::Vector3& rPosition )
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
void Entity::SetRotation( const Simd::Quat& rRotation )
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
void Entity::SetScale( const Simd::Vector3& rScale )
{
    m_scale = rScale;
}

/// Set the slice to which this entity is currently bound, along with the index of this entity within the slice.
///
/// @param[in] pSlice      Slice to set.
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

/// Get the world to which this entity is currently bound.
///
/// @return  Entity world.
///
/// @see GetSlice()
WorldWPtr Entity::GetWorld() const
{
    return ( m_spSlice ? m_spSlice->GetWorld() : WorldWPtr() );
}
