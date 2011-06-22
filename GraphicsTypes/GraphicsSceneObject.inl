//----------------------------------------------------------------------------------------------------------------------
// GraphicsSceneObject.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Get the instance transform matrix.
    ///
    /// @return  Transform matrix.
    ///
    /// @see SetTransform()
    const Simd::Matrix44& GraphicsSceneObject::GetTransform() const
    {
        return m_transform;
    }

    /// Get the world-space axis-aligned bounding box for this instance.
    ///
    /// @return  World-space axis-aligned bounding box.
    ///
    /// @see SetWorldBounds(), GetWorldSphere()
    const Simd::AaBox& GraphicsSceneObject::GetWorldBox() const
    {
        return m_worldBox;
    }

    /// Get the world-space bounding sphere for this instance.
    ///
    /// @return  World-space bounding sphere.
    ///
    /// @see SetWorldBounds(), GetWorldBox()
    const Simd::Sphere& GraphicsSceneObject::GetWorldSphere() const
    {
        return m_worldSphere;
    }

    /// Get the instance vertex buffer.
    ///
    /// @return  Instance vertex buffer.
    ///
    /// @see GetVertexDescription(), GetVertexStride(), SetVertexData()
    RVertexBuffer* GraphicsSceneObject::GetVertexBuffer() const
    {
        return m_spVertexBuffer;
    }

    /// Get the description for the vertices in the assigned buffer.
    ///
    /// @return  Vertex description.
    ///
    /// @see GetVertexBuffer(), GetVertexStride(), SetVertexData()
    RVertexDescription* GraphicsSceneObject::GetVertexDescription() const
    {
        return m_spVertexDescription;
    }

    /// Get the stride between each vertex in the assigned vertex buffer.
    ///
    /// @return  Vertex stride, in bytes.
    ///
    /// @see GetVertexBuffer(), GetVertexDescription(), SetVertexData()
    uint32_t GraphicsSceneObject::GetVertexStride() const
    {
        return m_vertexStride;
    }

    /// Get the index buffer used for rendering.
    ///
    /// @return  Sub-mesh index buffer.
    ///
    /// @see SetIndexBuffer()
    RIndexBuffer* GraphicsSceneObject::GetIndexBuffer() const
    {
        return m_spIndexBuffer;
    }

#if L_USE_GRANNY_ANIMATION

    /// Get the Granny bone information for this mesh.
    ///
    /// @return  Bone data.
    ///
    /// @see GetBoneCount(), SetBoneData()
    const void* GraphicsSceneObject::GetBoneData() const
    {
        return m_pBoneData;
    }

#else

    /// Get the array of inverse transforms for each bone in the mesh reference pose.
    ///
    /// @return  Inverse reference pose bone transform array.
    ///
    /// @see GetBoneCount(), SetBoneData()
    const Simd::Matrix44* GraphicsSceneObject::GetInverseReferencePose() const
    {
        return m_pInverseReferencePose;
    }

#endif

    /// Get the number of bones in the skinned mesh rendered via this scene object.
    ///
    /// @return  Number of bones in the skinned mesh.
    ///
    /// @see SetBoneData()
    uint8_t GraphicsSceneObject::GetBoneCount() const
    {
        return m_boneCount;
    }

    /// Get the bone transform palette for skinned mesh rendering.
    ///
    /// @return  Array of matrices for each bone's transform.
    ///
    /// @see SetBonePalette()
    const Simd::Matrix44* GraphicsSceneObject::GetBonePalette() const
    {
        return m_pBonePalette;
    }

    /// Get whether this scene object needs to be updated prior to the next scene update.
    ///
    /// @return  True if an update is needed, false if not.
    ///
    /// @see SetNeedsUpdate(), GetUpdateMode(), Update()
    bool GraphicsSceneObject::GetNeedsUpdate() const
    {
        return ( m_updateMode != static_cast< uint8_t >( UPDATE_INVALID ) );
    }

    /// Get the update mode currently set for this scene object.
    ///
    /// @return  Currently set update mode.
    ///
    /// @see SetNeedsUpdate(), GetNeedsUpdate(), Update()
    GraphicsSceneObject::EUpdate GraphicsSceneObject::GetUpdateMode() const
    {
        return ( m_updateMode == static_cast< uint8_t >( UPDATE_INVALID )
                 ? UPDATE_INVALID
                 : static_cast< EUpdate >( m_updateMode ) );
    }

    /// Update this scene object if an update is needed and clear the update flag.
    ///
    /// @param[in] pScene  Graphics scene to which this object is attached.
    ///
    /// @see GetNeedsUpdate(), Update()
    void GraphicsSceneObject::ConditionalUpdate( GraphicsScene* pScene )
    {
        if( m_updateMode != static_cast< uint8_t >( UPDATE_INVALID ) )
        {
            HELIUM_ASSERT( m_pUpdateCallback );
            m_pUpdateCallback( m_pUpdateCallbackData, pScene, this );

            m_updateMode = static_cast< uint8_t >( UPDATE_INVALID );
        }
    }

    /// Get the ID of the parent graphics scene object used to control the placement of this object as well as provide
    /// its vertex data.
    ///
    /// @return  Parent scene object ID.
    size_t GraphicsSceneObject::SubMeshData::GetSceneObjectId() const
    {
        return m_sceneObjectId;
    }

    /// Get the material used for rendering.
    ///
    /// @return  Sub-mesh material.
    ///
    /// @see SetMaterial()
    const MaterialPtr& GraphicsSceneObject::SubMeshData::GetMaterial() const
    {
        return m_spMaterial;
    }

    /// Get the mapping of bone indices to skinning palette indices for use by the GPU during skinned mesh rendering.
    ///
    /// @return  Mapping of bone indices to GPU skinning palette indices.
    ///
    /// @see SetSkinningPaletteMap()
    const uint8_t* GraphicsSceneObject::SubMeshData::GetSkinningPaletteMap() const
    {
        return m_pSkinningPaletteMap;
    }

    /// Get the primitive type to render.
    ///
    /// @return  Primitive type.
    ///
    /// @see SetPrimitiveType()
    ERendererPrimitiveType GraphicsSceneObject::SubMeshData::GetPrimitiveType() const
    {
        return m_primitiveType;
    }

    /// Get the number of primitives to render.
    ///
    /// @return  Primitive count.
    ///
    /// @see SetPrimitiveCount()
    uint32_t GraphicsSceneObject::SubMeshData::GetPrimitiveCount() const
    {
        return m_primitiveCount;
    }

    /// Get the offset of the first vertex to use within the vertex buffer.
    ///
    /// @return  Offset of the first vertex.
    ///
    /// @see SetStartVertex()
    uint32_t GraphicsSceneObject::SubMeshData::GetStartVertex() const
    {
        return m_startVertex;
    }

    /// Get the total range of vertices, starting from the start vertex, that are addressed when rendering this
    /// sub-mesh.
    ///
    /// @return  Number of vertices addressed.
    ///
    /// @see SetVertexRange()
    uint32_t GraphicsSceneObject::SubMeshData::GetVertexRange() const
    {
        return m_vertexRange;
    }

    /// Get the offset of the first index to use within the index buffer.
    ///
    /// @return  Offset of the first index.
    ///
    /// @see SetStartIndex()
    uint32_t GraphicsSceneObject::SubMeshData::GetStartIndex() const
    {
        return m_startIndex;
    }
}
