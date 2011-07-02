//----------------------------------------------------------------------------------------------------------------------
// Mesh.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Get the number of individual sections in this mesh.
    ///
    /// Each section incorporates its own draw call and can have a unique material assigned.
    ///
    /// @return  Number of sections in this mesh.
    ///
    /// @see GetSectionVertexCount(), GetSectionTriangleCount(), GetSectionSkinningPaletteMap()
    size_t Mesh::GetSectionCount() const
    {
        return m_sectionTriangleCounts.GetSize();
    }

    /// Get the number of vertices addressed by a specific mesh section.
    ///
    /// Each mesh section uses its own range of vertices in the mesh vertex buffer.  Vertices for a given section are
    /// stored contiguously in the vertex buffer, with each section stored in sequence.
    ///
    /// @param[in] sectionIndex  Mesh section index.
    ///
    /// @return  Number of vertices addressed by the section associated with the specified index.
    ///
    /// @see GetSectionTriangleCount(), GetSectionSkinningPaletteMap(), GetSectionCount()
    uint32_t Mesh::GetSectionVertexCount( size_t sectionIndex ) const
    {
        HELIUM_ASSERT( sectionIndex < m_sectionVertexCounts.GetSize() );

        return m_sectionVertexCounts[ sectionIndex ];
    }

    /// Get the number of triangles in a specific mesh section.
    ///
    /// Each section uses its own range of vertex indices in the mesh index buffer.  Indices for a given section are
    /// stored contiguously in the index buffer, with each section stored in sequence.
    ///
    /// @param[in] sectionIndex  Mesh section index.
    ///
    /// @return  Number of triangles in the section associated with the specified index.
    ///
    /// @see GetSectionVertexCount(), GetSectionSkinningPaletteMap(), GetSectionCount()
    uint32_t Mesh::GetSectionTriangleCount( size_t sectionIndex ) const
    {
        HELIUM_ASSERT( sectionIndex < m_sectionTriangleCounts.GetSize() );

        return m_sectionTriangleCounts[ sectionIndex ];
    }

    /// Get whether this mesh is a skinned mesh.
    ///
    /// @return  True if this is a skinned mesh, false if not.
    bool Mesh::IsSkinned() const
    {
#if HELIUM_USE_GRANNY_ANIMATION
        return m_grannyData.IsSkinned();
#else
        return ( m_boneCount != 0 );
#endif
    }

#if HELIUM_USE_GRANNY_ANIMATION

    /// Get the Granny-specific data associated with this mesh.
    ///
    /// @return  Granny-specific mesh data.
    const Granny::MeshData& Mesh::GetGrannyData() const
    {
        return m_grannyData;
    }

#else  // HELIUM_USE_GRANNY_ANIMATION

    /// Get the number of bones to which this mesh is weighted.
    ///
    /// @return  Bone count.  Note that this will always return zero for non-skinned meshes.
    ///
    /// @see IsSkinned()
    uint8_t Mesh::GetBoneCount() const
    {
        return m_boneCount;
    }

    /// Get the array of bone names for this mesh.
    ///
    /// @return  Pointer to the bone name array, or null if this mesh is not a skinned mesh.
    const Name* Mesh::GetBoneNames() const
    {
        return m_pBoneNames;
    }

    /// Get the array containing the indices of the parent bone for each bone.
    ///
    /// @return  Pointer to the parent bone index array, or null if this mesh is not a skinned mesh.
    const uint8_t* Mesh::GetParentBoneIndices() const
    {
        return m_pParentBoneIndices;
    }

    /// Get the array of reference pose bone transforms for this mesh.
    ///
    /// @return  Pointer to the array of reference pose bone transforms, or null if this mesh is not a skinned mesh.
    const Simd::Matrix44* Mesh::GetReferencePose() const
    {
        return m_pReferencePose;
    }

#endif  // HELIUM_USE_GRANNY_ANIMATION

    /// Get the number of materials assigned to this mesh's default material set.
    ///
    /// @return  Number of materials in the default material set.
    ///
    /// @see GetMaterial()
    size_t Mesh::GetMaterialCount() const
    {
        return m_materials.GetSize();
    }

    /// Get the default material assigned to a specific submesh.
    ///
    /// @param[in] index  Submesh index.
    ///
    /// @return  Default material assigned to the specified submesh.
    ///
    /// @see GetMaterialCount()
    Material* Mesh::GetMaterial( size_t index ) const
    {
        return ( index < m_materials.GetSize() ? m_materials[ index ].Get() : NULL );
    }

    /// Get the number of vertices in this mesh.
    ///
    /// @return  Mesh vertex count.
    ///
    /// @see GetTriangleCount()
    uint32_t Mesh::GetVertexCount() const
    {
        return m_vertexCount;
    }

    /// Get the number of triangles in this mesh.
    ///
    /// @return  Mesh triangle count.
    ///
    /// @see GetVertexCount()
    uint32_t Mesh::GetTriangleCount() const
    {
        return m_triangleCount;
    }

    /// Get the bounds of this mesh.
    ///
    /// @return  Axis-aligned bounding box encompassing this mesh.
    const Simd::AaBox& Mesh::GetBounds() const
    {
        return m_bounds;
    }

    /// Get the vertex buffer for this mesh.
    ///
    /// @return  Vertex buffer.
    ///
    /// @see GetIndexBuffer()
    RVertexBuffer* Mesh::GetVertexBuffer() const
    {
        return m_spVertexBuffer;
    }

    /// Get the index buffer for this mesh.
    ///
    /// @return  Index buffer.
    ///
    /// @see GetVertexBuffer()
    RIndexBuffer* Mesh::GetIndexBuffer() const
    {
        return m_spIndexBuffer;
    }
}
