//----------------------------------------------------------------------------------------------------------------------
// GraphicsSceneView.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_GRAPHICS_TYPES_GRAPHICS_SCENE_VIEW_H
#define HELIUM_GRAPHICS_TYPES_GRAPHICS_SCENE_VIEW_H

#include "GraphicsTypes/GraphicsTypes.h"

#include "Platform/Math/Simd/Matrix44.h"
#include "Platform/Math/Simd/Frustum.h"
#include "Rendering/Color.h"
#include "Rendering/RRenderResource.h"

namespace Helium
{
    L_DECLARE_RPTR( RConstantBuffer );
    L_DECLARE_RPTR( RRenderContext );
    L_DECLARE_RPTR( RSurface );

    /// Information related to a given view within a GraphicsScene.
    ///
    /// A view entails a given view transform, projection, and other information that may be useful (i.e. cached
    /// occlusion information)
    HELIUM_SIMD_ALIGN_PRE class HELIUM_GRAPHICS_TYPES_API GraphicsSceneView
    {
    public:
        /// Default horizontal field-of-view angle.
        static const float32_t DEFAULT_HORIZONTAL_FOV;
        /// Default aspect ratio.
        static const float32_t DEFAULT_ASPECT_RATIO;
        /// Default near-clip distance.
        static const float32_t DEFAULT_NEAR_CLIP;
        /// Default far-clip distance.
        static const float32_t DEFAULT_FAR_CLIP;

        /// Default distance from the camera at which shadows cut off.
        static const float32_t DEFAULT_SHADOW_CUTOFF_DISTANCE;
        /// Default distance from the camera at which shadows begin fading out.
        static const float32_t DEFAULT_SHADOW_FADE_DISTANCE;

        /// @name Construction/Destruction
        //@{
        GraphicsSceneView();
        //@}

        /// @name Updating
        //@{
        inline void ConditionalUpdate();
        //@}

        /// @name Viewport Parameters
        //@{
        void SetRenderContext( RRenderContext* pRenderContext );
        inline RRenderContext* GetRenderContext() const;

        void SetDepthStencilSurface( RSurface* pSurface );
        inline RSurface* GetDepthStencilSurface() const;

        void SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height );
        inline uint32_t GetViewportX() const;
        inline uint32_t GetViewportY() const;
        inline uint32_t GetViewportWidth() const;
        inline uint32_t GetViewportHeight() const;

        void SetClearColor( const Color& rColor );
        inline const Color& GetClearColor() const;
        //@}

        /// @name View Updating
        //@{
        void SetView( const Simd::Vector3& rOrigin, const Simd::Vector3& rForward, const Simd::Vector3& rUp );
        void SetHorizontalFov( float32_t fov );
        void SetAspectRatio( float32_t aspectRatio );
        void SetNearClip( float32_t distance );
        void SetFarClip( float32_t distance );
        //@}

        /// @name Data Access
        //@{
        inline const Simd::Vector3& GetOrigin() const;
        inline const Simd::Vector3& GetForward() const;
        inline const Simd::Vector3& GetUp() const;

        inline const Simd::Matrix44& GetViewMatrix() const;
        inline const Simd::Matrix44& GetInverseViewMatrix() const;
        inline const Simd::Matrix44& GetInverseViewProjectionMatrix() const;

        inline const Simd::Frustum& GetFrustum() const;

        inline RConstantBuffer* GetScreenSpaceVertexConstantBuffer() const;

        inline float32_t GetShadowCutoffDistance() const;
        inline float32_t GetShadowFadeDistance() const;
        //@}

    private:
        /// View transform matrix.
        Simd::Matrix44 m_viewMatrix;
        /// Projection matrix.
        Simd::Matrix44 m_projectionMatrix;
        /// Inverse view matrix.
        Simd::Matrix44 m_inverseViewMatrix;
        /// Combined inverse view/projection matrix.
        Simd::Matrix44 m_inverseViewProjectionMatrix;

        /// View frustum.
        Simd::Frustum m_frustum;

        /// View origin.
        Simd::Vector3 m_origin;
        /// View direction.
        Simd::Vector3 m_forward;
        /// Up vector.
        Simd::Vector3 m_up;

        /// Clear color.
        Color m_clearColor;

        /// Render context for this view.
        RRenderContextPtr m_spRenderContext;
        /// Depth-stencil surface for this view.
        RSurfacePtr m_spDepthStencilSurface;

        /// Horizontal pixel coordinate of the upper-left corner of this view.
        uint32_t m_viewportX;
        /// Vertical pixel coordinate of the upper-left corner of this view.
        uint32_t m_viewportY;
        /// Width of this view in pixels.
        uint32_t m_viewportWidth;
        /// Height of this view in pixels.
        uint32_t m_viewportHeight;

        /// Horizontal field-of-view angle, in degrees.
        float32_t m_horizontalFov;
        /// Aspect ratio (width:height).
        float32_t m_aspectRatio;
        /// Near clip distance.
        float32_t m_nearClip;
        /// Far clip distance.
        float32_t m_farClip;

        /// Maximum shadow distance from the camera.
        float32_t m_shadowCutoffDistance;
        /// Distance at which shadows begin to fade out.
        float32_t m_shadowFadeDistance;

        /// True if the view and projection matrices need to be updated.
        bool m_bDirtyView;

        /// @name View and Projection Matrix Updating
        //@{
        void UpdateViewProjection();
        //@}
    } HELIUM_SIMD_ALIGN_POST;
}

#include "GraphicsTypes/GraphicsSceneView.inl"

#endif  // HELIUM_GRAPHICS_TYPES_GRAPHICS_SCENE_VIEW_H
