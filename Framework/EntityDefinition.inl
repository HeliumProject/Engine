//----------------------------------------------------------------------------------------------------------------------
// EntityDefinition.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Get the entity position.
    ///
    /// @return  Current position.
    ///
    /// @see GetRotation(), GetScale()
    const Simd::Vector3& EntityDefinition::GetPosition() const
    {
        return m_position;
    }

    /// Get the entity rotation.
    ///
    /// @return  Current rotation.
    ///
    /// @see GetPosition(), GetScale()
    const Simd::Quat& EntityDefinition::GetRotation() const
    {
        return m_rotation;
    }

    /// Get the entity scale.
    ///
    /// @return  Current scale.
    ///
    /// @see GetPosition(), GetRotation()
    const Simd::Vector3& EntityDefinition::GetScale() const
    {
        return m_scale;
    }

    /// Get the slice to which this entity is currently bound.
    ///
    /// @return  EntityDefinition slice.
    ///
    /// @see GetSliceIndex(), SetSliceInfo(), SetSliceIndex(), ClearSliceInfo()
    const SliceDefinitionWPtr& EntityDefinition::GetSlice() const
    {
        return m_spSlice;
    }

    /// Get the index of this entity in the slice to which it is currently bound.
    ///
    /// Note that this index may change at runtime, so it should not be used to track an entity.
    ///
    /// @return  EntityDefinition index within its slice.
    ///
    /// @see GetSlice(), SetSliceInfo(), SetSliceIndex(), ClearSliceInfo()
    size_t EntityDefinition::GetSliceIndex() const
    {
        return m_sliceIndex;
    }
}
