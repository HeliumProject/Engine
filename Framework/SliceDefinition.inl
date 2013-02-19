//----------------------------------------------------------------------------------------------------------------------
// SliceDefinition.inl
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
    Package* SliceDefinition::GetPackage() const
    {
        return m_spPackage;
    }

    /// Get the number of entities in this slice.
    ///
    /// @return  EntityDefinition count.
    ///
    /// @see GetEntity()
    size_t SliceDefinition::GetEntityDefinitionCount() const
    {
        return m_entityDefinitions.GetSize();
    }

    /// Get the entity associated with the given index in this slice.
    ///
    /// @param[in] index  EntityDefinition index.
    ///
    /// @return  EntityDefinition instance.
    ///
    /// @see GetEntityCount()
    EntityDefinition* SliceDefinition::GetEntityDefinition( size_t index ) const
    {
        HELIUM_ASSERT( index < m_entityDefinitions.GetSize() );

        return m_entityDefinitions[ index ];
    }
}
