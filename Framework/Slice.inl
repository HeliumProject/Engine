//----------------------------------------------------------------------------------------------------------------------
// Slice.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Get the package currently bound to this slice.
    ///
    /// @return  Bound package.
    ///
    /// @see BindPackage()
    Package* Slice::GetPackage() const
    {
        return m_spPackage;
    }

    /// Get the number of entities in this slice.
    ///
    /// @return  Entity count.
    ///
    /// @see GetEntity()
    size_t Slice::GetEntityCount() const
    {
        return m_entities.GetSize();
    }

    /// Get the entity associated with the given index in this slice.
    ///
    /// @param[in] index  Entity index.
    ///
    /// @return  Entity instance.
    ///
    /// @see GetEntityCount()
    Entity* Slice::GetEntity( size_t index ) const
    {
        HELIUM_ASSERT( index < m_entities.GetSize() );

        return m_entities[ index ];
    }

    /// Get the world to which this slice is currently bound.
    ///
    /// @return  Slice world.
    ///
    /// @see GetWorldIndex(), SetWorldInfo(), SetWorldIndex(), ClearWorldInfo()
    const WorldWPtr& Slice::GetWorld() const
    {
        return m_spWorld;
    }

    /// Get the index of this slice in the world to which it is currently bound.
    ///
    /// Note that this index may change at runtime, so it should not be used to track a slice.
    ///
    /// @return  Slice index within its world.
    ///
    /// @see GetWorld(), SetWorldInfo(), SetWorldIndex(), ClearWorldInfo()
    size_t Slice::GetWorldIndex() const
    {
        return m_worldIndex;
    }
}
