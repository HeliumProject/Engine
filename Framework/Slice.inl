//----------------------------------------------------------------------------------------------------------------------
// SliceDefinition.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    World *Slice::GetWorld()
    {
        return m_spWorld.Get();
    }

    /// Get the world to which this slice is currently bound.
    ///
    /// @return  SliceDefinition world.
    ///
    /// @see GetWorldIndex(), SetWorldInfo(), SetWorldIndex(), ClearWorldInfo()
    const World *Slice::GetWorld() const
    {
        return m_spWorld.Get();
    }

    /// Get the index of this slice in the world to which it is currently bound.
    ///
    /// Note that this index may change at runtime, so it should not be used to track a slice.
    ///
    /// @return  SliceDefinition index within its world.
    ///
    /// @see GetWorld(), SetWorldInfo(), SetWorldIndex(), ClearWorldInfo()
    size_t Slice::GetWorldIndex() const
    {
        return m_worldIndex;
    }
        /// Get the number of entities in this slice.
    ///
    /// @return  EntityDefinition count.
    ///
    /// @see GetEntity()
    size_t Slice::GetEntityCount() const
    {
        return m_entities.GetSize();
    }

    /// Get the entity associated with the given index in this slice.
    ///
    /// @param[in] index  EntityDefinition index.
    ///
    /// @return  EntityDefinition instance.
    ///
    /// @see GetEntityCount()
    Entity* Slice::GetEntity( size_t index ) const
    {
        HELIUM_ASSERT( index < m_entities.GetSize() );

        return m_entities[ index ];
    }
}
