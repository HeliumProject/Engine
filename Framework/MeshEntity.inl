//----------------------------------------------------------------------------------------------------------------------
// MeshEntity.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the mesh assigned to this entity.
    ///
    /// @return  Assigned mesh.
    Mesh* MeshEntity::GetMesh() const
    {
        VerifySafety();
        return m_spMesh;
    }

    /// Get the number of override materials assigned to this entity.
    ///
    /// @return  Override material count.
    ///
    /// @see GetOverrideMaterial(), GetMaterial()
    size_t MeshEntity::GetOverrideMaterialCount() const
    {
        VerifySafety();
        return m_overrideMaterials.GetSize();
    }

    /// Get the override material assigned to a specific submesh of the assigned mesh.
    ///
    /// @param[in] index  Submesh index.
    ///
    /// @return  Override material assigned to the specified submesh, or null if no override material is assigned.
    ///
    /// @see GetOverrideMaterialCount(), GetMaterial()
    Material* MeshEntity::GetOverrideMaterial( size_t index ) const
    {
        VerifySafety();
        return ( index < m_overrideMaterials.GetSize() ? m_overrideMaterials[ index ].Get() : NULL );
    }

    /// Get the material that will be used for a specific submesh of the assigned mesh.
    ///
    /// If an override material is assigned to the specified submesh, it will be returned, otherwise the default
    /// material assigned in the mesh resource will be provided.
    ///
    /// @param[in] index  Submesh index.
    ///
    /// @return  Material that will be used for the specified submesh.
    ///
    /// @see GetOverrideMaterial(), GetOverrideMaterialCount()
    Material* MeshEntity::GetMaterial( size_t index ) const
    {
        VerifySafety();

        Material* pOverrideMaterial = GetOverrideMaterial( index );
        if( pOverrideMaterial )
        {
            return pOverrideMaterial;
        }

        return ( m_spMesh ? m_spMesh->GetMaterial( index ) : NULL );
    }
}
