/// Get the mesh assigned to this entity.
///
/// @return  Assigned mesh.
Helium::Mesh* Helium::MeshComponent::GetMesh() const
{
    return m_Mesh;
}

/// Get the number of override materials assigned to this entity.
///
/// @return  Override material count.
///
/// @see GetOverrideMaterial(), GetMaterial()
size_t Helium::MeshComponent::GetOverrideMaterialCount() const
{
    return m_OverrideMaterials.GetSize();
}

/// Get the override material assigned to a specific submesh of the assigned mesh.
///
/// @param[in] index  Submesh index.
///
/// @return  Override material assigned to the specified submesh, or null if no override material is assigned.
///
/// @see GetOverrideMaterialCount(), GetMaterial()
Helium::Material* Helium::MeshComponent::GetOverrideMaterial( size_t index ) const
{
    return ( index < m_OverrideMaterials.GetSize() ? m_OverrideMaterials[ index ].Get() : NULL );
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
Helium::Material* Helium::MeshComponent::GetMaterial( size_t index ) const
{
    Material* pOverrideMaterial = GetOverrideMaterial( index );
    if( pOverrideMaterial )
    {
        return pOverrideMaterial;
    }

    return ( m_Mesh ? m_Mesh->GetMaterial( index ) : NULL );
}
