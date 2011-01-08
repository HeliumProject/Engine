//----------------------------------------------------------------------------------------------------------------------
// GraphicsSceneView.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsTypesPch.h"
#include "GraphicsTypes/GraphicsSceneView.h"

#include "Rendering/Renderer.h"
#include "Rendering/RConstantBuffer.h"
#include "Rendering/RRenderContext.h"
#include "Rendering/RSurface.h"

using namespace Lunar;

const float32_t GraphicsSceneView::DEFAULT_HORIZONTAL_FOV = 70.0f;
const float32_t GraphicsSceneView::DEFAULT_ASPECT_RATIO = 1.0f;
const float32_t GraphicsSceneView::DEFAULT_NEAR_CLIP = 0.5f;
const float32_t GraphicsSceneView::DEFAULT_FAR_CLIP = -1.0f;
const float32_t GraphicsSceneView::DEFAULT_SHADOW_CUTOFF_DISTANCE = 1500.0f;
const float32_t GraphicsSceneView::DEFAULT_SHADOW_FADE_DISTANCE = 1000.0f;

/// Constructor.
GraphicsSceneView::GraphicsSceneView()
    : m_viewMatrix( Simd::Matrix44::IDENTITY )
    , m_projectionMatrix( Simd::Matrix44::IDENTITY )
    , m_inverseViewMatrix( Simd::Matrix44::IDENTITY )
    , m_inverseViewProjectionMatrix( Simd::Matrix44::IDENTITY )
    , m_origin( 0.0f )
    , m_forward( 0.0f, 0.0f, 1.0f )
    , m_up( 0.0f, 1.0f, 0.0f )
    , m_clearColor( 0 )
    , m_viewportX( 0 )
    , m_viewportY( 0 )
    , m_viewportWidth( 0 )
    , m_viewportHeight( 0 )
    , m_horizontalFov( DEFAULT_HORIZONTAL_FOV )
    , m_aspectRatio( DEFAULT_ASPECT_RATIO )
    , m_nearClip( DEFAULT_NEAR_CLIP )
    , m_farClip( DEFAULT_FAR_CLIP )
    , m_shadowCutoffDistance( DEFAULT_SHADOW_CUTOFF_DISTANCE )
    , m_shadowFadeDistance( DEFAULT_SHADOW_FADE_DISTANCE )
    , m_bDirtyView( true )
{
    MemoryZero( &m_frustum, sizeof( m_frustum ) );
}

/// Set the render context that should be used by this view.
///
/// @param[in] pRenderContext  Render context to assign.
///
/// @see SetDepthStencilSurface(), SetViewport(), GetRenderContext(), GetDepthStencilSurface(), GetViewportX(),
///      GetViewportY(), GetViewportWidth(), GetViewportHeight()
void GraphicsSceneView::SetRenderContext( RRenderContext* pRenderContext )
{
    m_spRenderContext = pRenderContext;
}

/// Set the depth-stencil surface for this view to use.
///
/// @param[in] pSurface  Depth-stencil surface to use.
///
/// @see SetRenderContext(), SetViewport(), GetDepthStencilSurface(), GetRenderContext(), GetViewportX(),
///      GetViewportY(), GetViewportWidth(), GetViewportHeight()
void GraphicsSceneView::SetDepthStencilSurface( RSurface* pSurface )
{
    m_spDepthStencilSurface = pSurface;
}

/// Set the viewport dimensions for this view to use.
///
/// Setting the viewport will also initialize a constant buffer to use for scaling and offsetting pixel coordinates for
/// screen-space rendering.  Note that if the viewport settings do not change, the constant buffer will not be altered.
/// Additionally, setting the viewport to either a width or height of zero will cause no constant buffer to be allocated
/// (any existing constant buffer will be released).
///
/// @param[in] x       Horizontal pixel coordinate of the upper-left viewport corner.
/// @param[in] y       Vertical pixel coordinate of the upper-left viewport corner.
/// @param[in] width   Viewport width, in pixels.
/// @param[in] height  Viewport height, in pixels.
///
/// @see SetRenderContext(), SetDepthStencilSurface(), GetViewportX(), GetViewportY(), GetViewportWidth(),
///      GetViewportHeight(), GetRenderContext(), GetDepthStencilSurface()
void GraphicsSceneView::SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height )
{
    m_viewportX = x;
    m_viewportY = y;

    if( m_viewportWidth != width || m_viewportHeight != height )
    {
        m_viewportWidth = width;
        m_viewportHeight = height;

        if( ( width | height ) == 0 )
        {
            m_spScreenSpaceVertexConstantBuffer.Release();
        }
        else
        {
            Renderer* pRenderer = Renderer::GetStaticInstance();
            HELIUM_ASSERT( pRenderer || !m_spScreenSpaceVertexConstantBuffer );
            if( pRenderer )
            {
                float32_t invWidth = 1.0f / static_cast< float32_t >( width );
                float32_t invHeight = 1.0f / static_cast< float32_t >( height );

                float32_t positionScaleOffset[ 4 ] =
                {
                    2.0f * invWidth,
                    -2.0f * invHeight,
                    -1.0f - invWidth,
                    1.0f + invHeight,
                };

                if( !m_spScreenSpaceVertexConstantBuffer )
                {
                    m_spScreenSpaceVertexConstantBuffer = pRenderer->CreateConstantBuffer(
                        sizeof( float32_t ) * 4,
                        RENDERER_BUFFER_USAGE_STATIC,
                        positionScaleOffset );
                    HELIUM_ASSERT( m_spScreenSpaceVertexConstantBuffer );
                }
                else
                {
                    pRenderer->Flush();

                    void* pMappedBuffer = m_spScreenSpaceVertexConstantBuffer->Map();
                    HELIUM_ASSERT( pMappedBuffer );
                    MemoryCopy( pMappedBuffer, positionScaleOffset, sizeof( float32_t ) * 4 );
                    m_spScreenSpaceVertexConstantBuffer->Unmap();
                }
            }
        }
    }
}

/// Set the color to which the viewport color buffer should be filled at the start of each frame.
///
/// @param[in] rColor  Clear color.
///
/// @see GetClearColor()
void GraphicsSceneView::SetClearColor( const Color& rColor )
{
    m_clearColor = rColor;
}

/// Set the view origin and orientation.
///
/// @param[in] rOrigin   View origin.
/// @param[in] rForward  View direction.
/// @param[in] rUp       Up direction.
///
/// @see SetHorizontalFov(), SetAspectRatio()
void GraphicsSceneView::SetView( const Simd::Vector3& rOrigin, const Simd::Vector3& rForward, const Simd::Vector3& rUp )
{
    m_origin = rOrigin;
    m_forward = rForward;
    m_up = rUp;

    m_bDirtyView = true;
}

/// Set the horizontal field-of-view angle.
///
/// @param[in] fov  Field-of-view angle, in degrees.
///
/// @see SetView(), SetAspectRatio()
void GraphicsSceneView::SetHorizontalFov( float32_t fov )
{
    HELIUM_ASSERT( fov >= HELIUM_EPSILON );
    m_horizontalFov = Clamp( fov, HELIUM_EPSILON, 180.0f );

    m_bDirtyView = true;
}

/// Set the aspect ratio.
///
/// @param[in] aspectRatio  Aspect ratio (width to height).
///
/// @see SetView(), SetHorizontalFov()
void GraphicsSceneView::SetAspectRatio( float32_t aspectRatio )
{
    HELIUM_ASSERT( aspectRatio >= HELIUM_EPSILON );
    m_aspectRatio = Max( aspectRatio, HELIUM_EPSILON );

    m_bDirtyView = true;
}

/// Set the near clip plane distance.
///
/// @param[in] distance  Near clip distance.
///
/// @see SetFarClip()
void GraphicsSceneView::SetNearClip( float32_t distance )
{
    HELIUM_ASSERT( distance >= HELIUM_EPSILON );
    m_nearClip = Max( distance, HELIUM_EPSILON );

    m_bDirtyView = true;
}

/// Set the far clip plane distance.
///
/// @param[in] distance  Far clip plane distance, or a value less than zero to use an infinite far clip plane.
///
/// @see SetNearClip()
void GraphicsSceneView::SetFarClip( float32_t distance )
{
    HELIUM_ASSERT( distance >= HELIUM_EPSILON || distance < 0.0f );
    m_farClip = ( distance < 0.0f ? distance : Max( distance, HELIUM_EPSILON ) );

    m_bDirtyView = true;
}

/// Update the various view- and projection-related transform matrices from the view settings if necessary.
void GraphicsSceneView::UpdateViewProjection()
{
    // Don't update the transform matrices if the view parameters are not dirty.
    if( !m_bDirtyView )
    {
        return;
    }

    m_bDirtyView = false;

    // Compute the projection matrix.
    float32_t horizontalFovRadians = m_horizontalFov * static_cast< float32_t >( HELIUM_DEG_TO_RAD );
    if( m_farClip >= 0.0f )
    {
        m_projectionMatrix.SetPerspectiveProjection( horizontalFovRadians, m_aspectRatio, m_nearClip, m_farClip );
    }
    else
    {
        m_projectionMatrix.SetPerspectiveProjection( horizontalFovRadians, m_aspectRatio, m_nearClip );
    }

    // Orthonormalize the view basis and compute the view matrix.
    Simd::Vector3 forward = m_forward.GetNormalized();

    Simd::Vector3 right;
    right.CrossSet( m_up, forward );
    right.Normalize();

    Simd::Vector3 up;
    up.CrossSet( forward, right );

    m_viewMatrix = Simd::Matrix44(
        right.GetElement( 0 ),    right.GetElement( 1 ),    right.GetElement( 2 ),    0.0f,
        up.GetElement( 0 ),       up.GetElement( 1 ),       up.GetElement( 2 ),       0.0f,
        forward.GetElement( 0 ),  forward.GetElement( 1 ),  forward.GetElement( 2 ),  0.0f,
        m_origin.GetElement( 0 ), m_origin.GetElement( 1 ), m_origin.GetElement( 2 ), 1.0f );

    // Compute the inverse view and combined inverse view/projection matrices.
    m_viewMatrix.GetInverse( m_inverseViewMatrix );
    m_inverseViewProjectionMatrix.MultiplySet( m_inverseViewMatrix, m_projectionMatrix );

    // Initialize the view frustum.
    m_frustum.Set( m_inverseViewProjectionMatrix.GetTranspose() );
}
