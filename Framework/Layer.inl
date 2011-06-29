//----------------------------------------------------------------------------------------------------------------------
// Layer.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Get the package currently bound to this layer.
    ///
    /// @return  Bound package.
    ///
    /// @see BindPackage()
    Package* Layer::GetPackage() const
    {
        return m_spPackage;
    }

    /// Get the number of entities in this layer.
    ///
    /// @return  Entity count.
    ///
    /// @see GetEntity()
    size_t Layer::GetEntityCount() const
    {
        return m_entities.GetSize();
    }

    /// Get the entity associated with the given index in this layer.
    ///
    /// @param[in] index  Entity index.
    ///
    /// @return  Entity instance.
    ///
    /// @see GetEntityCount()
    Entity* Layer::GetEntity( size_t index ) const
    {
        HELIUM_ASSERT( index < m_entities.GetSize() );

        return m_entities[ index ];
    }

    /// Get the world to which this layer is currently bound.
    ///
    /// @return  Layer world.
    ///
    /// @see GetWorldIndex(), SetWorldInfo(), SetWorldIndex(), ClearWorldInfo()
    const WorldWPtr& Layer::GetWorld() const
    {
        return m_spWorld;
    }

    /// Get the index of this layer in the world to which it is currently bound.
    ///
    /// Note that this index may change at runtime, so it should not be used to track a layer.
    ///
    /// @return  Layer index within its world.
    ///
    /// @see GetWorld(), SetWorldInfo(), SetWorldIndex(), ClearWorldInfo()
    size_t Layer::GetWorldIndex() const
    {
        return m_worldIndex;
    }
}
