//----------------------------------------------------------------------------------------------------------------------
// GraphicsScene.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Access the scene view with the specified ID.
    ///
    /// @param[in] id  ID of the view to retrieve.
    ///
    /// @return  Pointer to the specified scene view.
    ///
    /// @see AllocateSceneView(), ReleaseSceneView(), SetActiveSceneView()
    GraphicsSceneView* GraphicsScene::GetSceneView( uint32_t id )
    {
        HELIUM_ASSERT( id < m_sceneViews.GetSize() );
        HELIUM_ASSERT( m_sceneViews.IsElementValid( id ) );

        return &m_sceneViews[ id ];
    }

    /// Access the scene object with the specified ID.
    ///
    /// @param[in] id  ID of the object to retrieve.
    ///
    /// @return  Pointer to the specified scene object.
    ///
    /// @see AllocateSceneObject(), ReleaseSceneObject()
    GraphicsSceneObject* GraphicsScene::GetSceneObject( size_t id )
    {
        HELIUM_ASSERT( id < m_sceneObjects.GetSize() );
        HELIUM_ASSERT( m_sceneObjects.IsElementValid( id ) );

        return &m_sceneObjects[ id ];
    }

    /// Access the scene object sub-mesh data with the specified ID.
    ///
    /// @param[in] id  ID of the sub-mesh data to retrieve.
    ///
    /// @return  Pointer to the specified scene object sub-mesh data.
    ///
    /// @see AllocateSceneObjectSubMeshData(), ReleaseSceneObjectSubMeshData()
    GraphicsSceneObject::SubMeshData* GraphicsScene::GetSceneObjectSubMeshData( size_t id )
    {
        HELIUM_ASSERT( id < m_sceneObjectSubMeshes.GetSize() );
        HELIUM_ASSERT( m_sceneObjectSubMeshes.IsElementValid( id ) );

        return &m_sceneObjectSubMeshes[ id ];
    }

    /// Get the ambient light color for upward-facing normals.
    ///
    /// @return  Ambient light color for upward-facing normals.
    ///
    /// @see GetAmbientLightTopBrightness(), GetAmbientLightBottomColor(), GetAmbientLightBottomBrightness(),
    ///      SetAmbientLight()
    const Color& GraphicsScene::GetAmbientLightTopColor() const
    {
        return m_ambientLightTopColor;
    }

    /// Get the brightness factor of ambient lighting for upward-facing normals.
    ///
    /// @return  Ambient light brightness for upward-facing normals.
    ///
    /// @see GetAmbientLightTopColor(), GetAmbientLightBottomColor(), GetAmbientLightBottomBrightness(),
    ///      SetAmbientLight()
    float32_t GraphicsScene::GetAmbientLightTopBrightness() const
    {
        return m_ambientLightTopBrightness;
    }

    /// Get the ambient light color for downward-facing normals.
    ///
    /// @return  Ambient light color for downward-facing normals.
    ///
    /// @see GetAmbientLightBottomBrightness(), GetAmbientLightTopColor(), GetAmbientLightTopBrightness(),
    ///      SetAmbientLight()
    const Color& GraphicsScene::GetAmbientLightBottomColor() const
    {
        return m_ambientLightBottomColor;
    }

    /// Get the brightness factor of ambient lighting for downward-facing normals.
    ///
    /// @return  Ambient light brightness for downward-facing normals.
    ///
    /// @see GetAmbientLightBottomColor(), GetAmbientLightTopColor(), GetAmbientLightTopBrightness(), SetAmbientLight()
    float32_t GraphicsScene::GetAmbientLightBottomBrightness() const
    {
        return m_ambientLightBottomBrightness;
    }

    /// Get the direction of the scene's directional light.
    ///
    /// @return  Directional light direction.
    ///
    /// @see GetDirectionalLightColor(), GetDirectionalLightBrightness(), SetDirectionalLight()
    const Simd::Vector3& GraphicsScene::GetDirectionalLightDirection() const
    {
        return m_directionalLightDirection;
    }

    /// Get the color of the scene's directional light.
    ///
    /// @return  Directional light color.
    ///
    /// @see GetDirectionalLightDirection(), GetDirectionalLightBrightness(), SetDirectionalLight()
    const Color& GraphicsScene::GetDirectionalLightColor() const
    {
        return m_directionalLightColor;
    }

    /// Get the brightness factor of the scene's directional light.
    ///
    /// @return  Directional light brightness.
    ///
    /// @see GetDirectionalLightDirection(), GetDirectionalLightColor(), SetDirectionalLight()
    float32_t GraphicsScene::GetDirectionalLightBrightness() const
    {
        return m_directionalLightBrightness;
    }

#if !HELIUM_RELEASE && !HELIUM_PROFILE
    /// Get the buffered drawing interface for the entire scene.
    ///
    /// Draw calls buffered through this interface will be presented on all views for this scene.
    ///
    /// @return  Reference to the buffered drawing interface for this scene.
    ///
    /// @see GetSceneViewBufferedDrawer()
    BufferedDrawer& GraphicsScene::GetSceneBufferedDrawer()
    {
        return m_sceneBufferedDrawer;
    }
#endif  // !HELIUM_RELEASE && !HELIUM_PROFILE
}
