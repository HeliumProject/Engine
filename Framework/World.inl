namespace Helium
{
	ComponentCollection & Helium::World::GetComponents()
	{
		return m_Components;
	}

	ComponentManager * Helium::World::GetComponentManager()
	{
		return m_ComponentManager.Ptr();
	}

    /// Get the number of slices currently active in this world.
    ///
    /// @return  Slice count.
    ///
    /// @see GetSlice()
    size_t World::GetSliceCount() const
    {
        return m_Slices.GetSize();
    }
}
