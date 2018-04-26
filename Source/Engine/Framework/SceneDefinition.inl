namespace Helium
{
    /// Get the number of entities in this slice.
    ///
    /// @return  EntityDefinition count.
    ///
    /// @see GetEntity()
    size_t SceneDefinition::GetEntityDefinitionCount() const
    {
        return m_Entities.GetSize();
    }

    /// Get the entity associated with the given index in this slice.
    ///
    /// @param[in] index  EntityDefinition index.
    ///
    /// @return  EntityDefinition instance.
    ///
    /// @see GetEntityCount()
    EntityDefinition* SceneDefinition::GetEntityDefinition( size_t index ) const
    {
        HELIUM_ASSERT( index < m_Entities.GetSize() );

        return m_Entities[ index ];
    }
        
    const WorldDefinition *SceneDefinition::GetWorldDefinition() const
    {
        return m_WorldDefinition.Get();
    }

    inline void SceneDefinition::SetWorldDefinition(Helium::WorldDefinition *pWorldDefinition)
    {
        m_WorldDefinition = pWorldDefinition;
    }

#if HELIUM_TOOLS
	inline SceneDefinition::ToolsData *SceneDefinition::GetToolsData() const
	{
		return m_ToolsData;
	}

	inline void SceneDefinition::SetToolsData( ToolsData *data )
	{
		m_ToolsData = data;
	}
#endif
}
