//----------------------------------------------------------------------------------------------------------------------
// GraphicsSceneView.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Update this scene view if an update is needed and clear the dirty flag.
    void GraphicsSceneView::ConditionalUpdate()
    {
        if( m_bDirtyView )
        {
            UpdateViewProjection();
        }
    }

    /// Get the render context used by this view.
    ///
    /// @return  Render context used by this view.
    ///
    /// @see GetDepthStencilSurface(), GetViewportX(), GetViewportY(), GetViewportWidth(), GetViewportHeight(),
    ///      SetRenderContext(), SetDepthStencilSurface(), SetViewport()
    RRenderContext* GraphicsSceneView::GetRenderContext() const
    {
        return m_spRenderContext;
    }

    /// Get the depth-stencil surface used by this view.
    ///
    /// @return  Depth-stencil surface used by this view.
    ///
    /// @see GetRenderContext(), GetViewportX(), GetViewportY(), GetViewportWidth(), GetViewportHeight(),
    ///      SetDepthStencilSurface(), SetRenderContext(), SetViewport()
    RSurface* GraphicsSceneView::GetDepthStencilSurface() const
    {
        return m_spDepthStencilSurface;
    }

    /// Get the horizontal pixel coordinate of the upper-left corner of this view.
    ///
    /// @return  Horizontal pixel coordinate of the upper-left viewport corner.
    ///
    /// @see GetViewportY(), GetViewportWidth(), GetViewportHeight(), GetRenderContext(), GetDepthStencilSurface(),
    ///      SetViewport(), SetRenderContext(), SetDepthStencilSurface()
    uint32_t GraphicsSceneView::GetViewportX() const
    {
        return m_viewportX;
    }

    /// Get the vertical pixel coordinate of the upper-left corner of this view.
    ///
    /// @return  Vertical pixel coordinate of the upper-left viewport corner.
    ///
    /// @see GetViewportX(), GetViewportWidth(), GetViewportHeight(), GetRenderContext(), GetDepthStencilSurface(),
    ///      SetViewport(), SetRenderContext(), SetDepthStencilSurface()
    uint32_t GraphicsSceneView::GetViewportY() const
    {
        return m_viewportY;
    }

    /// Get the pixel width of this view.
    ///
    /// @return  Viewport width, in pixels.
    ///
    /// @see GetViewportHeight(), GetViewportX(), GetViewportY(), GetRenderContext(), GetDepthStencilSurface(),
    ///      SetViewport(), SetRenderContext(), SetDepthStencilSurface()
    uint32_t GraphicsSceneView::GetViewportWidth() const
    {
        return m_viewportWidth;
    }

    /// Get the pixel height of this view.
    ///
    /// @return  Viewport height, in pixels.
    ///
    /// @see GetViewportWidth(), GetViewportX(), GetViewportY(), GetRenderContext(), GetDepthStencilSurface(),
    ///      SetViewport(), SetRenderContext(), SetDepthStencilSurface()
    uint32_t GraphicsSceneView::GetViewportHeight() const
    {
        return m_viewportHeight;
    }

    /// Get the color to which the viewport color buffer will be filled at the start of each frame.
    ///
    /// @return  Clear color.
    ///
    /// @see SetClearColor()
    const Color& GraphicsSceneView::GetClearColor() const
    {
        return m_clearColor;
    }

    /// Get the camera origin.
    ///
    /// @return  Camera view origin.
    ///
    /// @see GetForward(), GetUp()
    const Simd::Vector3& GraphicsSceneView::GetOrigin() const
    {
        return m_origin;
    }

    /// Get a vector pointing forward in relation to the camera viewing orientation.
    ///
    /// @return  Camera forward direction.
    ///
    /// @see GetOrigin(), GetUp()
    const Simd::Vector3& GraphicsSceneView::GetForward() const
    {
        return m_forward;
    }

    /// Get a vector pointing up in relation to the camera viewing orientation.
    ///
    /// @return  Camera up direction.
    ///
    /// @see GetOrigin(), GetForward()
    const Simd::Vector3& GraphicsSceneView::GetUp() const
    {
        return m_up;
    }

    /// Get the view matrix for this scene view.
    ///
    /// @return  View matrix.
    const Simd::Matrix44& GraphicsSceneView::GetViewMatrix() const
    {
        return m_viewMatrix;
    }

    /// Get the inverse view matrix for this scene view.
    ///
    /// @return  Inverse view matrix.
    const Simd::Matrix44& GraphicsSceneView::GetInverseViewMatrix() const
    {
        return m_inverseViewMatrix;
    }

    /// Get the combined inverse view/projection matrix for this scene view.
    ///
    /// @return  Inverse view/projection matrix.
    const Simd::Matrix44& GraphicsSceneView::GetInverseViewProjectionMatrix() const
    {
        return m_inverseViewProjectionMatrix;
    }

    /// Get the current world-space view frustum.
    ///
    /// @return  View frustum.
    const Simd::Frustum& GraphicsSceneView::GetFrustum() const
    {
        return m_frustum;
    }

    /// Get the distance from the camera at which shadows should no longer be rendered.
    ///
    /// @return  Shadow cutoff distance.
    ///
    /// @see GetShadowFadeDistance()
    float32_t GraphicsSceneView::GetShadowCutoffDistance() const
    {
        return m_shadowCutoffDistance;
    }

    /// Get the distance from the camera at which shadows should begin to fade out.
    ///
    /// @return  Shadow fade-out start distance.
    ///
    /// @see GetShadowCutoffDistance()
    float32_t GraphicsSceneView::GetShadowFadeDistance() const
    {
        return m_shadowFadeDistance;
    }
}
