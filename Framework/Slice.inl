namespace Helium
{
    /// Get the index of this slice in the world to which it is currently bound.
    ///
    /// Note that this index may change at runtime, so it should not be used to track a slice.
    ///
    /// @return  SceneDefinition index within its world.
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

}
