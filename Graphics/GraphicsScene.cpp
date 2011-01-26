//----------------------------------------------------------------------------------------------------------------------
// GraphicsScene.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsPch.h"
#include "Graphics/GraphicsScene.h"

#include "Platform/Math/Simd/Plane.h"
#include "Platform/Math/Simd/Vector3Soa.h"
#include "Platform/Math/Simd/VectorConversion.h"
#include "Engine/JobContext.h"
#include "EngineJobs/EngineJobsInterface.h"
#include "Rendering/RConstantBuffer.h"
#include "Rendering/RIndexBuffer.h"
#include "Rendering/RPixelShader.h"
#include "Rendering/RRenderCommandProxy.h"
#include "Rendering/RRenderContext.h"
#include "Rendering/Renderer.h"
#include "Rendering/RSurface.h"
#include "Rendering/RVertexBuffer.h"
#include "Rendering/RVertexInputLayout.h"
#include "Rendering/RVertexShader.h"
#include "GraphicsTypes/VertexTypes.h"
#include "GraphicsJobs/GraphicsJobsInterface.h"
#include "Graphics/DynamicDrawer.h"
#include "Graphics/Material.h"
#include "Graphics/RenderResourceManager.h"
#include "Graphics/Texture.h"

using namespace Lunar;

#if !HELIUM_RELEASE && !HELIUM_PROFILE
static const size_t SCENE_VIEW_BUFFERED_DRAWER_POOL_BLOCK_SIZE = 4;
#endif !HELIUM_RELEASE && !HELIUM_PROFILE

namespace Lunar
{
    L_DECLARE_RPTR( RRenderCommandProxy );
}

L_IMPLEMENT_OBJECT( GraphicsScene, Graphics, 0 );

/// Constructor.
GraphicsScene::GraphicsScene()
    :
#if !HELIUM_RELEASE && !HELIUM_PROFILE
      m_viewBufferedDrawerPool( SCENE_VIEW_BUFFERED_DRAWER_POOL_BLOCK_SIZE )
    ,
#endif  // !HELIUM_RELEASE && !HELIUM_PROFILE
      m_ambientLightTopColor( 0xffffffff )
    , m_ambientLightTopBrightness( 0.25f )
    , m_ambientLightBottomColor( 0xff000000 )
    , m_ambientLightBottomBrightness( 0.0f )
    , m_directionalLightDirection( 0.0f, -1.0f, 0.0f )
    , m_directionalLightColor( 0xffffffff )
    , m_directionalLightBrightness( 1.0f )
    , m_activeViewId( Invalid< uint32_t >() )
    , m_constantBufferSetIndex( 0 )
{
#if !HELIUM_RELEASE && !HELIUM_PROFILE
    HELIUM_VERIFY( m_sceneBufferedDrawer.Initialize() );
#endif
}

/// Destructor.
GraphicsScene::~GraphicsScene()
{
}

/// Update this graphics scene for the current frame.
void GraphicsScene::Update()
{
    // Check for lost devices.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( !pRenderer )
    {
        return;
    }

    Renderer::EStatus rendererStatus = pRenderer->GetStatus();
    if( rendererStatus == Renderer::STATUS_LOST )
    {
        return;
    }

    if( rendererStatus == Renderer::STATUS_NOT_RESET )
    {
        rendererStatus = pRenderer->Reset();
        if( rendererStatus != Renderer::STATUS_READY )
        {
            return;
        }
    }

    // No need to update anything if we have no scene render texture or scene views.
    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();

    RTexture2dPtr spSceneTexture = rRenderResourceManager.GetSceneTexture();
    if( !spSceneTexture )
    {
        return;
    }

    size_t sceneViewCount = m_sceneViews.GetSize();
    if( sceneViewCount == 0 )
    {
        return;
    }

    // Prepare the array of inverse view/projection matrices for each view's shadow depth pass.
    if( m_shadowViewInverseViewProjectionMatrices.GetSize() < sceneViewCount )
    {
        m_shadowViewInverseViewProjectionMatrices.Reserve( sceneViewCount );
        m_shadowViewInverseViewProjectionMatrices.Resize( sceneViewCount );
    }

    // Update each scene view as necessary and compute their inverse view/projection matrices.
    for( size_t viewIndex = 0; viewIndex < sceneViewCount; ++viewIndex )
    {
        if( !m_sceneViews.IsElementValid( viewIndex ) )
        {
            continue;
        }

        m_sceneViews[ viewIndex ].ConditionalUpdate();
        UpdateShadowInverseViewProjectionMatrixSimple( viewIndex );
    }

    // Update each scene object as necessary.
    size_t sceneObjectCount = m_sceneObjects.GetSize();
    for( size_t objectIndex = 0; objectIndex < sceneObjectCount; ++objectIndex )
    {
        if( !m_sceneObjects.IsElementValid( objectIndex ) )
        {
            continue;
        }

        m_sceneObjects[ objectIndex ].ConditionalUpdate( this );
    }

    // Swap dynamic constant buffers and update their contents.
    SwapDynamicConstantBuffers();

    // Resize the visible object bit array as necessary.
    m_visibleSceneObjects.Reserve( sceneObjectCount );
    m_visibleSceneObjects.Resize( sceneObjectCount );

#if !HELIUM_RELEASE && !HELIUM_PROFILE
    // Set up the scene's buffered drawer for the current frame.
    m_sceneBufferedDrawer.BeginDrawing();
#endif  // !HELIUM_RELEASE && !HELIUM_PROFILE

    // Update and render each scene view.
    for( size_t viewIndex = 0; viewIndex < sceneViewCount; ++viewIndex )
    {
#if !HELIUM_RELEASE && !HELIUM_PROFILE
        // Set up the current view's buffered drawer for the current frame.
        BufferedDrawer* pDrawer = NULL;
        if( viewIndex < m_viewBufferedDrawers.GetSize() )
        {
            pDrawer = m_viewBufferedDrawers[ viewIndex ];
            if( pDrawer )
            {
                pDrawer->BeginDrawing();
            }
        }
#endif  // !HELIUM_RELEASE && !HELIUM_PROFILE

        DrawSceneView( static_cast< uint_fast32_t >( viewIndex ) );

#if !HELIUM_RELEASE && !HELIUM_PROFILE
        // Finish drawing with the current view's buffered drawer.
        if( pDrawer )
        {
            pDrawer->EndDrawing();
        }
#endif  // !HELIUM_RELEASE && !HELIUM_PROFILE
    }

#if !HELIUM_RELEASE && !HELIUM_PROFILE
    // Finish drawing with the scene's buffered drawer.
    m_sceneBufferedDrawer.EndDrawing();
#endif  // !HELIUM_RELEASE && !HELIUM_PROFILE
}

/// Allocate a new scene view.
///
/// @return  ID of the newly allocated view.
///
/// @see ReleaseSceneView(), GetSceneView(), SetActiveSceneView()
uint32_t GraphicsScene::AllocateSceneView()
{
    GraphicsSceneView* pSceneView = m_sceneViews.New();
    HELIUM_ASSERT( pSceneView );

    return static_cast< uint32_t >( m_sceneViews.GetElementIndex( pSceneView ) );
}

/// Release previously allocated scene view.
///
/// @param[in] id  ID of the view to release.
///
/// @see AllocateSceneView(), GetSceneView(), SetActiveSceneView()
void GraphicsScene::ReleaseSceneView( uint32_t id )
{
    HELIUM_ASSERT( id < m_sceneViews.GetSize() );
    HELIUM_ASSERT( m_sceneViews.IsElementValid( id ) );

    m_sceneViews.Remove( id );

    if( m_activeViewId == id )
    {
        SetInvalid( m_activeViewId );
    }

    // Release any allocated buffered drawing interface for the view being released.
#if !HELIUM_RELEASE && !HELIUM_PROFILE
    if( id < m_viewBufferedDrawers.GetSize() )
    {
        BufferedDrawer* pDrawer = m_viewBufferedDrawers[ id ];
        if( pDrawer )
        {
            pDrawer->Shutdown();
            m_viewBufferedDrawerPool.Release( pDrawer );
            m_viewBufferedDrawers[ id ] = NULL;
        }
    }
#endif  // !HELIUM_RELEASE && !HELIUM_PROFILE
}

/// Set the active scene view to use for rendering.
///
/// @param[in] id  ID of the view to set active.
///
/// @see AllocateSceneView(), ReleaseSceneView(), GetSceneView()
void GraphicsScene::SetActiveSceneView( uint32_t id )
{
    HELIUM_ASSERT( IsInvalid( id ) || ( id < m_sceneViews.GetSize() && m_sceneViews.IsElementValid( id ) ) );

    m_activeViewId = id;
}

/// Allocate a new scene object and add it to the scene.
///
/// @return  ID of the newly allocated object.
///
/// @see ReleaseSceneObject(), GetSceneObject()
size_t GraphicsScene::AllocateSceneObject()
{
    GraphicsSceneObject* pSceneObject = m_sceneObjects.New();
    HELIUM_ASSERT( pSceneObject );

    return m_sceneObjects.GetElementIndex( pSceneObject );
}

/// Detach and release a previously allocated scene object.
///
/// @param[in] id  ID of the object to release.
///
/// @see AllocateSceneObject(), GetSceneObject()
void GraphicsScene::ReleaseSceneObject( size_t id )
{
    HELIUM_ASSERT( id < m_sceneObjects.GetSize() );
    HELIUM_ASSERT( m_sceneObjects.IsElementValid( id ) );

    m_sceneObjects.Remove( id );
}

/// Allocate new scene object sub-mesh data and add it to the scene.
///
/// @param[in] sceneObjectId  ID of the parent graphics scene object used to control the placement of the sub-mesh
///                           as well as provide its vertex data.
///
/// @return  ID of the newly allocated object.
///
/// @see ReleaseSceneObjectSubMeshData(), GetSceneObjectSubMeshData()
size_t GraphicsScene::AllocateSceneObjectSubMeshData( size_t sceneObjectId )
{
    HELIUM_ASSERT( sceneObjectId < m_sceneObjects.GetSize() );
    HELIUM_ASSERT( m_sceneObjects.IsElementValid( sceneObjectId ) );

    GraphicsSceneObject::SubMeshData* pSubMeshData = m_sceneObjectSubMeshes.New( sceneObjectId );
    HELIUM_ASSERT( pSubMeshData );

    return m_sceneObjectSubMeshes.GetElementIndex( pSubMeshData );
}

/// Detach and release previously allocated scene object sub-mesh data.
///
/// @param[in] id  ID of the object to release.
///
/// @see AllocateScenObjectSubMeshData(), GetSceneObjectSubMeshData()
void GraphicsScene::ReleaseSceneObjectSubMeshData( size_t id )
{
    HELIUM_ASSERT( id < m_sceneObjectSubMeshes.GetSize() );
    HELIUM_ASSERT( m_sceneObjectSubMeshes.IsElementValid( id ) );

    m_sceneObjectSubMeshes.Remove( id );
}

/// Set the properties for the scene's ambient lighting.
///
/// @param[in] rTopColor         Ambient light coloring to apply to upward-facing normals.
/// @param[in] topBrightness     Ambient light brightness factor to apply to upward-facing normals.
/// @param[in] rBottomColor      Ambient light coloring to apply to downward-facing normals.
/// @param[in] bottomBrightness  Ambient light brightness factor to apply to downward-facing normals.
///
/// @see GetAmbientLightTopColor(), GetAmbientLightTopBrightness(), GetAmbientLightBottomColor(),
///      GetAmbientLightBottomBrightness()
void GraphicsScene::SetAmbientLight(
                                    const Color& rTopColor,
                                    float32_t topBrightness,
                                    const Color& rBottomColor,
                                    float32_t bottomBrightness )
{
    m_ambientLightTopColor = rTopColor;
    m_ambientLightTopBrightness = topBrightness;
    m_ambientLightBottomColor = rBottomColor;
    m_ambientLightBottomBrightness = bottomBrightness;
}

/// Set the properties for the scene's global directional light.
///
/// @param[in] rDirection  Light direction.
/// @param[in] rColor      Light color.
/// @param[in] brightness  Light brightness factor.
///
/// @see GetDirectionalLightDirection(), GetDirectionalLightColor(), GetDirectionalLightBrightness()
void GraphicsScene::SetDirectionalLight( const Simd::Vector3& rDirection, const Color& rColor, float32_t brightness )
{
    m_directionalLightDirection = rDirection;
    m_directionalLightDirection.Normalize();

    m_directionalLightColor = rColor;
    m_directionalLightBrightness = brightness;
}

#if !HELIUM_RELEASE && !HELIUM_PROFILE
/// Get the buffered drawing interface for the specified scene view.
///
/// Draw calls buffered through the provided interface will only be rendered on the scene view with the specified
/// ID.
///
/// @param[in] id  Scene view ID.
///
/// @return  Pointer to the buffered drawing interface for the specified scene view if valid, null if not.
///
/// @see GetSceneBufferedDrawer()
BufferedDrawer* GraphicsScene::GetSceneViewBufferedDrawer( uint32_t id )
{
    // Make sure the ID references a valid view first.
    if( id >= m_sceneViews.GetSize() || !m_sceneViews.IsElementValid( id ) )
    {
        return NULL;
    }

    // If a buffered drawer does not already exist for the specified view, allocate one from the object pool.
    BufferedDrawer* pDrawer = NULL;

    size_t viewBufferedDrawerCount = m_viewBufferedDrawers.GetSize();
    if( id < viewBufferedDrawerCount )
    {
        pDrawer = m_viewBufferedDrawers[ id ];
    }

    if( !pDrawer )
    {
        pDrawer = m_viewBufferedDrawerPool.Allocate();
        if( pDrawer )
        {
            if( id >= viewBufferedDrawerCount )
            {
                m_viewBufferedDrawers.Add( NULL, id - viewBufferedDrawerCount + 1 );
            }

            m_viewBufferedDrawers[ id ] = pDrawer;

            HELIUM_VERIFY( pDrawer->Initialize() );
        }
    }

    return pDrawer;
}
#endif  // !HELIUM_RELEASE && !HELIUM_PROFILE

/// Get the name of the default sampler state used by material shaders.  Note that this must match the name given in
/// Data/Shaders/Common.inl.
///
/// @return  Default sampler state name.
Name GraphicsScene::GetDefaultSamplerStateName()
{
    static Name defaultSamplerStateName( TXT( "DefaultSamplerState" ) );

    return defaultSamplerStateName;
}

/// Get the name of the shadow map sampler state.
///
/// @return  Shadow map sampler state name.
Name GraphicsScene::GetShadowSamplerStateName()
{
    static Name shadowSamplerStateName( TXT( "ShadowSamplerState" ) );

    return shadowSamplerStateName;
}

/// Get the reserved name for the shadow map texture in material shaders.
///
/// @return  Shadow map shader texture input name.
Name GraphicsScene::GetShadowMapTextureName()
{
    static Name shadowMapTextureName( TXT( "_ShadowMap" ) );

    return shadowMapTextureName;
}

/// Update the shadow depth pass inverse view/projection matrix for a given scene view.
///
/// @param[in] viewIndex  Index of the scene view for which to update the shadow depth pass transform matrix.
void GraphicsScene::UpdateShadowInverseViewProjectionMatrixSimple( size_t viewIndex )
{
    HELIUM_ASSERT( viewIndex < m_sceneViews.GetSize() );
    HELIUM_ASSERT( m_sceneViews.IsElementValid( viewIndex ) );
    HELIUM_ASSERT( viewIndex < m_shadowViewInverseViewProjectionMatrices.GetSize() );

    // Compute the scene directional light's view basis for shadow calculation.
    Simd::Vector3 shadowViewForward = m_directionalLightDirection;
    Simd::Vector3 shadowViewUp( 0.0f, 1.0f, 0.0f );

    Simd::Vector3 shadowViewRight;
    shadowViewRight.CrossSet( shadowViewUp, shadowViewForward );
    shadowViewRight.Normalize();

    shadowViewUp.CrossSet( shadowViewForward, shadowViewRight );

    // Compute the corners of the view frustum region affected by shadowing.
    GraphicsSceneView& rView = m_sceneViews[ viewIndex ];

    float32_t shadowCutoffDistance = rView.GetShadowCutoffDistance();

    const Simd::Matrix44& rViewMatrix = rView.GetViewMatrix();
    Simd::Vector3 shadowClipNormal = Vector4ToVector3( rViewMatrix.GetRow( 2 ) );
    Simd::Vector3 shadowClipPoint = Vector4ToVector3( rViewMatrix.GetRow( 3 ) ) +
        shadowClipNormal * shadowCutoffDistance;
    shadowClipNormal.Negate();

    Simd::Plane shadowClipPlane( shadowClipNormal, shadowClipNormal.Dot( shadowClipPoint ) );

    Simd::Frustum shadowClippedViewFrustum = rView.GetFrustum();
    shadowClippedViewFrustum.SetFarClip( shadowClipPlane );

    HELIUM_SIMD_ALIGN_PRE float32_t shadowFrustumPointsX[
        ( 8 + ( HELIUM_SIMD_SIZE / sizeof( float32_t ) - 1 ) ) & ~( HELIUM_SIMD_SIZE / sizeof( float32_t ) - 1 ) ]
    HELIUM_SIMD_ALIGN_POST;
    HELIUM_SIMD_ALIGN_PRE float32_t shadowFrustumPointsY[
        ( 8 + ( HELIUM_SIMD_SIZE / sizeof( float32_t ) - 1 ) ) & ~( HELIUM_SIMD_SIZE / sizeof( float32_t ) - 1 ) ]
    HELIUM_SIMD_ALIGN_POST;
    HELIUM_SIMD_ALIGN_PRE float32_t shadowFrustumPointsZ[
        ( 8 + ( HELIUM_SIMD_SIZE / sizeof( float32_t ) - 1 ) ) & ~( HELIUM_SIMD_SIZE / sizeof( float32_t ) - 1 ) ]
    HELIUM_SIMD_ALIGN_POST;

    size_t cornerCount = shadowClippedViewFrustum.ComputeCornersSoa(
        shadowFrustumPointsX,
        shadowFrustumPointsY,
        shadowFrustumPointsZ );
    HELIUM_ASSERT( cornerCount == 8 );
    HELIUM_UNREF( cornerCount );

    // Project the view frustum corners onto the shadow viewing plane and compute the bounds needed to
    // contain the frustum points.
    Simd::Vector3Soa shadowViewXSoa( shadowViewRight );
    Simd::Vector3Soa shadowViewYSoa( shadowViewUp );

#if HELIUM_SIMD_SSE
    Simd::Vector3Soa points;

    points.m_x = Helium::Simd::LoadAligned( shadowFrustumPointsX );
    points.m_y = Helium::Simd::LoadAligned( shadowFrustumPointsY );
    points.m_z = Helium::Simd::LoadAligned( shadowFrustumPointsZ );

    Helium::Simd::Register projectedMinX = shadowViewXSoa.Dot( points );
    Helium::Simd::Register projectedMinY = shadowViewYSoa.Dot( points );
    Helium::Simd::Register projectedMaxX = projectedMinX;
    Helium::Simd::Register projectedMaxY = projectedMinY;

    points.m_x = Helium::Simd::LoadAligned( shadowFrustumPointsX + 4 );
    points.m_y = Helium::Simd::LoadAligned( shadowFrustumPointsY + 4 );
    points.m_z = Helium::Simd::LoadAligned( shadowFrustumPointsZ + 4 );

    Helium::Simd::Register projectedX = shadowViewXSoa.Dot( points );
    Helium::Simd::Register projectedY = shadowViewYSoa.Dot( points );

    projectedMinX = Helium::Simd::MinF32( projectedMinX, projectedX );
    projectedMinY = Helium::Simd::MinF32( projectedMinY, projectedY );
    projectedMaxX = Helium::Simd::MaxF32( projectedMaxX, projectedX );
    projectedMaxY = Helium::Simd::MaxF32( projectedMaxY, projectedY );

    Helium::Simd::Register projectedMinXYLo = _mm_unpacklo_ps( projectedMinX, projectedMinY );
    Helium::Simd::Register projectedMinXYHi = _mm_unpackhi_ps( projectedMinX, projectedMinY );
    Helium::Simd::Register projectedMinXY = Helium::Simd::MinF32( projectedMinXYLo, projectedMinXYHi );
    projectedMinXY = Helium::Simd::MinF32( projectedMinXY, _mm_movehl_ps( projectedMinXY, projectedMinXY ) );

    Helium::Simd::Register projectedMaxXYLo = _mm_unpacklo_ps( projectedMaxX, projectedMaxY );
    Helium::Simd::Register projectedMaxXYHi = _mm_unpackhi_ps( projectedMaxX, projectedMaxY );
    Helium::Simd::Register projectedMaxXY = Helium::Simd::MaxF32( projectedMaxXYLo, projectedMaxXYHi );
    projectedMaxXY = Helium::Simd::MaxF32( projectedMaxXY, _mm_movehl_ps( projectedMaxXY, projectedMaxXY ) );

    Helium::Simd::Register halfVec = Helium::Simd::SetSplatF32( 0.5f );

    Helium::Simd::Register projectedCenter = Helium::Simd::MultiplyF32(
        Helium::Simd::AddF32( projectedMinXY, projectedMaxXY ),
        halfVec );
    Helium::Simd::Register projectedWidthHeight = Helium::Simd::SubtractF32( projectedMaxXY, projectedMinXY );

    Helium::Simd::Register projectedCenterX = _mm_shuffle_ps(
        projectedCenter,
        projectedCenter,
        _MM_SHUFFLE( 0, 0, 0, 0 ) );
    Helium::Simd::Register projectedCenterY = _mm_shuffle_ps(
        projectedCenter,
        projectedCenter,
        _MM_SHUFFLE( 1, 1, 1, 1 ) );
    Helium::Simd::Register projectedCenterZ = Helium::Simd::SetSplatF32( -32767.0f );

    Simd::Vector3 shadowViewOrigin = shadowViewRight * Simd::Vector3( projectedCenterX ) +
        shadowViewUp * Simd::Vector3( projectedCenterY ) +
        shadowViewForward * Simd::Vector3( projectedCenterZ );

    HELIUM_SIMD_ALIGN_PRE float32_t shadowViewWidthHeight[ 4 ] HELIUM_SIMD_ALIGN_POST;
    Helium::Simd::StoreAligned( shadowViewWidthHeight, projectedWidthHeight );

    Simd::Matrix44 projection(
        Simd::Matrix44::INIT_ORTHOGONAL_PROJECTION,
        shadowViewWidthHeight[ 0 ],
        shadowViewWidthHeight[ 1 ],
        0.0f,
        65536.0f );
#else
#error Implement for other SIMD architectures.
#endif  // HELIUM_SIMD_SSE

    // Compute the inverse view matrix.
    Simd::Matrix44 inverseView(
        RayToVector4( shadowViewRight ),
        RayToVector4( shadowViewUp ),
        RayToVector4( shadowViewForward ),
        PointToVector4( shadowViewOrigin ) );
    inverseView.Invert();

    // Compute the combined inverse view/projection matrix.
    m_shadowViewInverseViewProjectionMatrices[ viewIndex ].MultiplySet( inverseView, projection );
}

/// Update the shadow depth pass inverse view/projection matrix for a given scene view, using light-space
/// perspective shadow mapping.
///
/// @param[in] viewIndex  Index of the scene view for which to update the shadow depth pass transform matrix.
void GraphicsScene::UpdateShadowInverseViewProjectionMatrixLspsm( size_t viewIndex )
{
    HELIUM_ASSERT( viewIndex < m_sceneViews.GetSize() );
    HELIUM_ASSERT( m_sceneViews.IsElementValid( viewIndex ) );
    HELIUM_ASSERT( viewIndex < m_shadowViewInverseViewProjectionMatrices.GetSize() );

    // XXX TMC TODO: Implement!!
    UpdateShadowInverseViewProjectionMatrixSimple( viewIndex );
}

/// Swap the dynamic constant buffers for view and instance data and push the current frame's data into the new
/// buffers.
void GraphicsScene::SwapDynamicConstantBuffers()
{
    // No need to update any rendering data if we have no active renderer.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( !pRenderer )
    {
        return;
    }

    // Pre-compute the inverse shadow map resolution and the transformation matrix from shadow view projection to UV
    // space for use when applying shadows to the scene.
    float32_t inverseShadowMapResolutionX = 1.0f;
    float32_t inverseShadowMapResolutionY = 1.0f;

    Simd::Matrix44 shadowMapUvTransform(
        Simd::Vector4( 0.5f,  0.0f, 0.0f, 0.0f ),
        Simd::Vector4( 0.0f, -0.5f, 0.0f, 0.0f ),
        Simd::Vector4( 0.0f,  0.0f, 1.0f, 0.0f ),
        Simd::Vector4( 0.5f,  0.5f, 0.0f, 1.0f ) );

    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();
    RTexture2d* pShadowDepthTexture = rRenderResourceManager.GetShadowDepthTexture();
    if( pShadowDepthTexture )
    {
        inverseShadowMapResolutionX = 1.0f / static_cast< float32_t >( pShadowDepthTexture->GetWidth() );
        inverseShadowMapResolutionY = 1.0f / static_cast< float32_t >( pShadowDepthTexture->GetHeight() );

        float32_t shadowMapUsableSize =
            static_cast< float32_t >( rRenderResourceManager.GetShadowDepthTextureUsableSize() );
        float32_t shadowMapUsableX = shadowMapUsableSize * inverseShadowMapResolutionX;
        float32_t shadowMapUsableY = shadowMapUsableSize * inverseShadowMapResolutionY;

        float32_t halfShadowMapUsableX = shadowMapUsableX * 0.5f;
        float32_t negHalfShadowMapUsableY = shadowMapUsableY * -0.5f;

        shadowMapUvTransform.SetElement( 0, halfShadowMapUsableX );
        shadowMapUvTransform.SetElement( 5, negHalfShadowMapUsableY );
        shadowMapUvTransform.SetElement( 12, halfShadowMapUsableX );
        shadowMapUvTransform.SetElement( 13, negHalfShadowMapUsableY + 1.0f );
    }

    // Swap buffer sets.
    size_t bufferSetIndex = ( m_constantBufferSetIndex + 1 ) % HELIUM_ARRAY_COUNT( m_viewVertexGlobalDataBuffers );
    m_constantBufferSetIndex = bufferSetIndex;

    // Update view constant buffers.
    DynArray< RConstantBufferPtr >& rViewVertexGlobalDataBuffers =
        m_viewVertexGlobalDataBuffers[ bufferSetIndex ];
    DynArray< RConstantBufferPtr >& rViewVertexBasePassDataBuffers =
        m_viewVertexBasePassDataBuffers[ bufferSetIndex ];
    DynArray< RConstantBufferPtr >& rViewPixelBasePassDataBuffers =
        m_viewPixelBasePassDataBuffers[ bufferSetIndex ];
    DynArray< RConstantBufferPtr >& rShadowViewVertexDataBuffers = m_shadowViewVertexDataBuffers[ bufferSetIndex ];

    size_t sceneViewCount = m_sceneViews.GetSize();
    size_t viewBufferCount = rViewVertexGlobalDataBuffers.GetSize();
    HELIUM_ASSERT( rViewVertexBasePassDataBuffers.GetSize() == viewBufferCount );
    HELIUM_ASSERT( rViewPixelBasePassDataBuffers.GetSize() == viewBufferCount );
    HELIUM_ASSERT( rShadowViewVertexDataBuffers.GetSize() == viewBufferCount );
    if( viewBufferCount < sceneViewCount )
    {
        size_t additionalBufferCount = sceneViewCount - viewBufferCount;
        rViewVertexGlobalDataBuffers.Add( NULL, additionalBufferCount );
        rViewVertexBasePassDataBuffers.Add( NULL, additionalBufferCount );
        rViewPixelBasePassDataBuffers.Add( NULL, additionalBufferCount );
        rShadowViewVertexDataBuffers.Add( NULL, additionalBufferCount );
    }

    for( size_t viewIndex = 0; viewIndex < sceneViewCount; ++viewIndex )
    {
        if( !m_sceneViews.IsElementValid( viewIndex ) )
        {
            continue;
        }

        // Update the global vertex shader constants.
        RConstantBufferPtr spBuffer = rViewVertexGlobalDataBuffers[ viewIndex ];
        if( !spBuffer )
        {
            spBuffer = pRenderer->CreateConstantBuffer(
                sizeof( float32_t ) * 32,
                RENDERER_BUFFER_USAGE_DYNAMIC );
            if( !spBuffer )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "GraphicsScene::SwapDynamicConstantBuffers(): View vertex global data constant buffer " )
                    TXT( "creation failed!\n" ) ) );
            }

            rViewVertexGlobalDataBuffers[ viewIndex ] = spBuffer;
        }

        if( spBuffer )
        {
            float32_t* pMappedData = static_cast< float32_t* >( spBuffer->Map( RENDERER_BUFFER_MAP_HINT_DISCARD ) );
            HELIUM_ASSERT( pMappedData );

            GraphicsSceneView& rView = m_sceneViews[ viewIndex ];
            const Simd::Matrix44& rInverseViewProjectionMatrix = rView.GetInverseViewProjectionMatrix();
            const Simd::Matrix44& rInverseViewMatrix = rView.GetInverseViewMatrix();

            *( pMappedData++ ) = rInverseViewProjectionMatrix.GetElement( 0 );
            *( pMappedData++ ) = rInverseViewProjectionMatrix.GetElement( 4 );
            *( pMappedData++ ) = rInverseViewProjectionMatrix.GetElement( 8 );
            *( pMappedData++ ) = rInverseViewProjectionMatrix.GetElement( 12 );
            *( pMappedData++ ) = rInverseViewProjectionMatrix.GetElement( 1 );
            *( pMappedData++ ) = rInverseViewProjectionMatrix.GetElement( 5 );
            *( pMappedData++ ) = rInverseViewProjectionMatrix.GetElement( 9 );
            *( pMappedData++ ) = rInverseViewProjectionMatrix.GetElement( 13 );
            *( pMappedData++ ) = rInverseViewProjectionMatrix.GetElement( 2 );
            *( pMappedData++ ) = rInverseViewProjectionMatrix.GetElement( 6 );
            *( pMappedData++ ) = rInverseViewProjectionMatrix.GetElement( 10 );
            *( pMappedData++ ) = rInverseViewProjectionMatrix.GetElement( 14 );
            *( pMappedData++ ) = rInverseViewProjectionMatrix.GetElement( 3 );
            *( pMappedData++ ) = rInverseViewProjectionMatrix.GetElement( 7 );
            *( pMappedData++ ) = rInverseViewProjectionMatrix.GetElement( 11 );
            *( pMappedData++ ) = rInverseViewProjectionMatrix.GetElement( 15 );

            *( pMappedData++ ) = rInverseViewMatrix.GetElement( 0 );
            *( pMappedData++ ) = rInverseViewMatrix.GetElement( 4 );
            *( pMappedData++ ) = rInverseViewMatrix.GetElement( 8 );
            *( pMappedData++ ) = rInverseViewMatrix.GetElement( 12 );
            *( pMappedData++ ) = rInverseViewMatrix.GetElement( 1 );
            *( pMappedData++ ) = rInverseViewMatrix.GetElement( 5 );
            *( pMappedData++ ) = rInverseViewMatrix.GetElement( 9 );
            *( pMappedData++ ) = rInverseViewMatrix.GetElement( 13 );
            *( pMappedData++ ) = rInverseViewMatrix.GetElement( 2 );
            *( pMappedData++ ) = rInverseViewMatrix.GetElement( 6 );
            *( pMappedData++ ) = rInverseViewMatrix.GetElement( 10 );
            *( pMappedData++ ) = rInverseViewMatrix.GetElement( 14 );
            *( pMappedData++ ) = rInverseViewMatrix.GetElement( 3 );
            *( pMappedData++ ) = rInverseViewMatrix.GetElement( 7 );
            *( pMappedData++ ) = rInverseViewMatrix.GetElement( 11 );
            *pMappedData       = rInverseViewMatrix.GetElement( 15 );

            spBuffer->Unmap();
        }

        // Update the base-pass vertex shader constants.
        spBuffer = rViewVertexBasePassDataBuffers[ viewIndex ];
        if( !spBuffer )
        {
            spBuffer = pRenderer->CreateConstantBuffer(
                sizeof( float32_t ) * 24,
                RENDERER_BUFFER_USAGE_DYNAMIC );
            if( !spBuffer )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "GraphicsScene::SwapDynamicConstantBuffers(): View vertex base-pass data constant " )
                    TXT( "buffer creation failed!\n" ) ) );
            }

            rViewVertexBasePassDataBuffers[ viewIndex ] = spBuffer;
        }

        if( spBuffer )
        {
            float32_t* pMappedData = static_cast< float32_t* >( spBuffer->Map( RENDERER_BUFFER_MAP_HINT_DISCARD ) );
            HELIUM_ASSERT( pMappedData );

            HELIUM_ASSERT( viewIndex < m_shadowViewInverseViewProjectionMatrices.GetSize() );
            Simd::Matrix44 shadowViewInvViewProj;
            shadowViewInvViewProj.MultiplySet(
                m_shadowViewInverseViewProjectionMatrices[ viewIndex ],
                shadowMapUvTransform );

            GraphicsSceneView& rView = m_sceneViews[ viewIndex ];
            const Simd::Matrix44& rInverseViewMatrix = rView.GetInverseViewMatrix();

            Simd::Vector3 lightDir = -m_directionalLightDirection;
            lightDir = rInverseViewMatrix.TransformVector( lightDir );

            *( pMappedData++ ) = shadowViewInvViewProj.GetElement( 0 );
            *( pMappedData++ ) = shadowViewInvViewProj.GetElement( 4 );
            *( pMappedData++ ) = shadowViewInvViewProj.GetElement( 8 );
            *( pMappedData++ ) = shadowViewInvViewProj.GetElement( 12 );
            *( pMappedData++ ) = shadowViewInvViewProj.GetElement( 1 );
            *( pMappedData++ ) = shadowViewInvViewProj.GetElement( 5 );
            *( pMappedData++ ) = shadowViewInvViewProj.GetElement( 9 );
            *( pMappedData++ ) = shadowViewInvViewProj.GetElement( 13 );
            *( pMappedData++ ) = shadowViewInvViewProj.GetElement( 2 );
            *( pMappedData++ ) = shadowViewInvViewProj.GetElement( 6 );
            *( pMappedData++ ) = shadowViewInvViewProj.GetElement( 10 );
            *( pMappedData++ ) = shadowViewInvViewProj.GetElement( 14 );
            *( pMappedData++ ) = shadowViewInvViewProj.GetElement( 3 );
            *( pMappedData++ ) = shadowViewInvViewProj.GetElement( 7 );
            *( pMappedData++ ) = shadowViewInvViewProj.GetElement( 11 );
            *( pMappedData++ ) = shadowViewInvViewProj.GetElement( 15 );

            *( pMappedData++ ) = lightDir.GetElement( 0 );
            *( pMappedData++ ) = lightDir.GetElement( 1 );
            *( pMappedData++ ) = lightDir.GetElement( 2 );
            *( pMappedData++ ) = 0.0f;

            *( pMappedData++ ) = static_cast< float32_t >( rView.GetViewportWidth() ) * 0.5f;
            *( pMappedData++ ) = static_cast< float32_t >( rView.GetViewportHeight() ) * 0.5f;
            *( pMappedData++ ) = 0.0f;
            *pMappedData       = 0.0f;

            spBuffer->Unmap();
        }

        // Update the base-pass pixel shader constants.
        spBuffer = rViewPixelBasePassDataBuffers[ viewIndex ];
        if( !spBuffer )
        {
            spBuffer = pRenderer->CreateConstantBuffer(
                sizeof( float32_t ) * 16,
                RENDERER_BUFFER_USAGE_DYNAMIC );
            if( !spBuffer )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "GraphicsScene::SwapDynamicConstantBuffers(): View pixel base-pass data constant " )
                    TXT( "buffer creation failed!\n" ) ) );
            }

            rViewPixelBasePassDataBuffers[ viewIndex ] = spBuffer;
        }

        if( spBuffer )
        {
            float32_t* pMappedData = static_cast< float32_t* >( spBuffer->Map( RENDERER_BUFFER_MAP_HINT_DISCARD ) );
            HELIUM_ASSERT( pMappedData );

            *( pMappedData++ ) = m_ambientLightTopColor.GetFloatR() * m_ambientLightTopBrightness;
            *( pMappedData++ ) = m_ambientLightTopColor.GetFloatG() * m_ambientLightTopBrightness;
            *( pMappedData++ ) = m_ambientLightTopColor.GetFloatB() * m_ambientLightTopBrightness;
            *( pMappedData++ ) = 1.0f;

            *( pMappedData++ ) = m_ambientLightBottomColor.GetFloatR() * m_ambientLightBottomBrightness;
            *( pMappedData++ ) = m_ambientLightBottomColor.GetFloatG() * m_ambientLightBottomBrightness;
            *( pMappedData++ ) = m_ambientLightBottomColor.GetFloatB() * m_ambientLightBottomBrightness;
            *( pMappedData++ ) = 1.0f;

            *( pMappedData++ ) = m_directionalLightColor.GetFloatR() * m_directionalLightBrightness;
            *( pMappedData++ ) = m_directionalLightColor.GetFloatG() * m_directionalLightBrightness;
            *( pMappedData++ ) = m_directionalLightColor.GetFloatB() * m_directionalLightBrightness;
            *( pMappedData++ ) = 1.0f;

            *( pMappedData++ ) = inverseShadowMapResolutionX;
            *( pMappedData++ ) = inverseShadowMapResolutionY;
            *( pMappedData++ ) = 0.0f;
            *pMappedData       = 0.0f;

            spBuffer->Unmap();
        }

        // Update the shadow depth pass vertex shader constants.
        spBuffer = rShadowViewVertexDataBuffers[ viewIndex ];
        if( !spBuffer )
        {
            spBuffer = pRenderer->CreateConstantBuffer(
                sizeof( float32_t ) * 32,
                RENDERER_BUFFER_USAGE_DYNAMIC );
            if( !spBuffer )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "GraphicsScene::SwapDynamicConstantBuffers(): Shadow view vertex data constant buffer " )
                    TXT( "creation failed!\n" ) ) );
            }

            rShadowViewVertexDataBuffers[ viewIndex ] = spBuffer;
        }

        if( spBuffer )
        {
            float32_t* pMappedData = static_cast< float32_t* >( spBuffer->Map( RENDERER_BUFFER_MAP_HINT_DISCARD ) );
            HELIUM_ASSERT( pMappedData );

            HELIUM_ASSERT( viewIndex < m_shadowViewInverseViewProjectionMatrices.GetSize() );
            const Simd::Matrix44& rShadowViewInvViewProj = m_shadowViewInverseViewProjectionMatrices[ viewIndex ];

            *( pMappedData++ ) = rShadowViewInvViewProj.GetElement( 0 );
            *( pMappedData++ ) = rShadowViewInvViewProj.GetElement( 4 );
            *( pMappedData++ ) = rShadowViewInvViewProj.GetElement( 8 );
            *( pMappedData++ ) = rShadowViewInvViewProj.GetElement( 12 );
            *( pMappedData++ ) = rShadowViewInvViewProj.GetElement( 1 );
            *( pMappedData++ ) = rShadowViewInvViewProj.GetElement( 5 );
            *( pMappedData++ ) = rShadowViewInvViewProj.GetElement( 9 );
            *( pMappedData++ ) = rShadowViewInvViewProj.GetElement( 13 );
            *( pMappedData++ ) = rShadowViewInvViewProj.GetElement( 2 );
            *( pMappedData++ ) = rShadowViewInvViewProj.GetElement( 6 );
            *( pMappedData++ ) = rShadowViewInvViewProj.GetElement( 10 );
            *( pMappedData++ ) = rShadowViewInvViewProj.GetElement( 14 );
            *( pMappedData++ ) = rShadowViewInvViewProj.GetElement( 3 );
            *( pMappedData++ ) = rShadowViewInvViewProj.GetElement( 7 );
            *( pMappedData++ ) = rShadowViewInvViewProj.GetElement( 11 );
            *pMappedData       = rShadowViewInvViewProj.GetElement( 15 );

            spBuffer->Unmap();
        }
    }

    // Map each instance constant buffer for updating.
    DynArray< RConstantBufferPtr >& rStaticInstanceVertexGlobalDataBufferPool =
        m_staticInstanceVertexGlobalDataBufferPool[ bufferSetIndex ];
    DynArray< RConstantBufferPtr >& rSkinnedInstanceVertexGlobalDataBufferPool =
        m_skinnedInstanceVertexGlobalDataBufferPool[ bufferSetIndex ];
    HELIUM_UNREF( rSkinnedInstanceVertexGlobalDataBufferPool );

    size_t sceneObjectCount = m_sceneObjects.GetSize();
    size_t instanceBufferCount = m_objectVertexGlobalDataBuffers.GetSize();
    if( instanceBufferCount < sceneObjectCount )
    {
        MemoryZero( m_objectVertexGlobalDataBuffers.GetData(), instanceBufferCount * sizeof( RConstantBuffer* ) );
        m_objectVertexGlobalDataBuffers.Add( NULL, sceneObjectCount - instanceBufferCount );
    }
    else
    {
        MemoryZero( m_objectVertexGlobalDataBuffers.GetData(), sceneObjectCount * sizeof( RConstantBuffer* ) );
    }

    size_t mappedBufferCount = m_mappedObjectVertexGlobalDataBuffers.GetSize();
    if( mappedBufferCount < sceneObjectCount )
    {
        MemoryZero( m_mappedObjectVertexGlobalDataBuffers.GetData(), mappedBufferCount * sizeof( float32_t* ) );
        m_mappedObjectVertexGlobalDataBuffers.Add( NULL, sceneObjectCount - mappedBufferCount );
    }
    else
    {
        MemoryZero( m_mappedObjectVertexGlobalDataBuffers.GetData(), sceneObjectCount * sizeof( float32_t* ) );
    }

    size_t subMeshCount = m_sceneObjectSubMeshes.GetSize();
    instanceBufferCount = m_subMeshVertexGlobalDataBuffers.GetSize();
    if( instanceBufferCount < subMeshCount )
    {
        MemoryZero( m_subMeshVertexGlobalDataBuffers.GetData(), instanceBufferCount * sizeof( RConstantBuffer* ) );
        m_subMeshVertexGlobalDataBuffers.Add( NULL, subMeshCount - instanceBufferCount );
    }
    else
    {
        MemoryZero( m_subMeshVertexGlobalDataBuffers.GetData(), subMeshCount * sizeof( RConstantBuffer* ) );
    }

    mappedBufferCount = m_mappedSubMeshVertexGlobalDataBuffers.GetSize();
    if( mappedBufferCount < subMeshCount )
    {
        MemoryZero( m_mappedSubMeshVertexGlobalDataBuffers.GetData(), mappedBufferCount * sizeof( float32_t* ) );
        m_mappedSubMeshVertexGlobalDataBuffers.Add( NULL, subMeshCount - mappedBufferCount );
    }
    else
    {
        MemoryZero( m_mappedSubMeshVertexGlobalDataBuffers.GetData(), subMeshCount * sizeof( float32_t* ) );
    }

    size_t staticBufferIndex = 0;
    size_t skinnedBufferIndex = 0;
    HELIUM_UNREF( skinnedBufferIndex );

    for( size_t subMeshIndex = 0; subMeshIndex < subMeshCount; ++subMeshIndex )
    {
        if( !m_sceneObjectSubMeshes.IsElementValid( subMeshIndex ) )
        {
            continue;
        }

        GraphicsSceneObject::SubMeshData& rSubMesh = m_sceneObjectSubMeshes[ subMeshIndex ];

        size_t sceneObjectIndex = rSubMesh.GetSceneObjectId();
        HELIUM_ASSERT( sceneObjectIndex < sceneObjectCount );

        // If the main scene object for the sub mesh already has a constant buffer assigned, we know it is a static
        // mesh that has already been processed, so we can skip it.
        if( m_objectVertexGlobalDataBuffers[ sceneObjectIndex ] )
        {
            continue;
        }

        // Determine whether the object should be rendered as a static mesh (vertex constant buffer per scene
        // object) or skinned mesh (vertex constant buffer per sub-mesh).
        HELIUM_ASSERT( m_sceneObjects.IsElementValid( sceneObjectIndex ) );
        GraphicsSceneObject& rSceneObject = m_sceneObjects[ sceneObjectIndex ];

        RConstantBuffer* pBuffer = NULL;

        uint_fast8_t boneCount = rSceneObject.GetBoneCount();
        if( boneCount != 0 )
        {
            const Simd::Matrix44* pBonePalette = rSceneObject.GetBonePalette();
            if( pBonePalette )
            {
                const uint8_t* pSkinningPaletteMap = rSubMesh.GetSkinningPaletteMap();
                if( pSkinningPaletteMap )
                {
                    RConstantBuffer* pBuffer;
                    if( skinnedBufferIndex < rSkinnedInstanceVertexGlobalDataBufferPool.GetSize() )
                    {
                        pBuffer = rSkinnedInstanceVertexGlobalDataBufferPool[ skinnedBufferIndex ];
                        HELIUM_ASSERT( pBuffer );
                    }
                    else
                    {
                        HELIUM_ASSERT( skinnedBufferIndex == rSkinnedInstanceVertexGlobalDataBufferPool.GetSize() );

                        pBuffer = pRenderer->CreateConstantBuffer(
                            sizeof( float32_t ) * 12 * BONE_COUNT_MAX,
                            RENDERER_BUFFER_USAGE_DYNAMIC );
                        if( !pBuffer )
                        {
                            HELIUM_TRACE(
                                TRACE_ERROR,
                                ( TXT( "GraphicsScene::SwapDynamicConstantBuffers(): Skinned mesh instance " )
                                TXT( "vertex constant global data buffer creation failed!\n" ) ) );
                        }
                        else
                        {
                            rSkinnedInstanceVertexGlobalDataBufferPool.Push( pBuffer );
                        }
                    }

                    if( pBuffer )
                    {
                        ++skinnedBufferIndex;

                        m_subMeshVertexGlobalDataBuffers[ subMeshIndex ] = pBuffer;

                        void* pMappedData = pBuffer->Map( RENDERER_BUFFER_MAP_HINT_DISCARD );
                        HELIUM_ASSERT( pMappedData );
                        m_mappedSubMeshVertexGlobalDataBuffers[ subMeshIndex ] =
                            static_cast< float32_t* >( pMappedData );

                        continue;
                    }
                }
            }
        }

        // Instance data not mapped as a skinned mesh, so map as a static mesh.
        if( staticBufferIndex < rStaticInstanceVertexGlobalDataBufferPool.GetSize() )
        {
            pBuffer = rStaticInstanceVertexGlobalDataBufferPool[ staticBufferIndex ];
            HELIUM_ASSERT( pBuffer );
        }
        else
        {
            HELIUM_ASSERT( staticBufferIndex == rStaticInstanceVertexGlobalDataBufferPool.GetSize() );

            pBuffer = pRenderer->CreateConstantBuffer(
                sizeof( float32_t ) * 12,
                RENDERER_BUFFER_USAGE_DYNAMIC );
            if( !pBuffer )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "GraphicsScene::SwapDynamicConstantBuffers(): Static mesh instance vertex " )
                    TXT( "constant global data buffer creation failed!\n" ) ) );
            }
            else
            {
                rStaticInstanceVertexGlobalDataBufferPool.Push( pBuffer );
            }
        }

        if( pBuffer )
        {
            ++staticBufferIndex;

            m_objectVertexGlobalDataBuffers[ sceneObjectIndex ] = pBuffer;

            void* pMappedData = pBuffer->Map( RENDERER_BUFFER_MAP_HINT_DISCARD );
            HELIUM_ASSERT( pMappedData );
            m_mappedObjectVertexGlobalDataBuffers[ sceneObjectIndex ] = static_cast< float32_t* >( pMappedData );
        }
    }

    // Update each constant buffer in parallel.
    {
        JobContext::Spawner< 1 > rootSpawner;
        JobContext* pContext = rootSpawner.Allocate();
        HELIUM_ASSERT( pContext );
        UpdateGraphicsSceneConstantBuffersJobSpawner* pSpawnerJob =
            pContext->Create< UpdateGraphicsSceneConstantBuffersJobSpawner >();
        HELIUM_ASSERT( pSpawnerJob );

        UpdateGraphicsSceneConstantBuffersJobSpawner::Parameters& rParameters = pSpawnerJob->GetParameters();
        rParameters.sceneObjectCount = static_cast< uint32_t >( sceneObjectCount );
        rParameters.subMeshCount = static_cast< uint32_t >( subMeshCount );
        rParameters.pSceneObjects = m_sceneObjects.GetData();
        rParameters.ppSceneObjectConstantBufferData = m_mappedObjectVertexGlobalDataBuffers.GetData();
        rParameters.pSubMeshes = m_sceneObjectSubMeshes.GetData();
        rParameters.ppSubMeshConstantBufferData = m_mappedSubMeshVertexGlobalDataBuffers.GetData();
    }

    // Unmap the constant buffers.
    for( size_t objectIndex = 0; objectIndex < sceneObjectCount; ++objectIndex )
    {
        if( m_mappedObjectVertexGlobalDataBuffers[ objectIndex ] )
        {
            RConstantBuffer* pBuffer = m_objectVertexGlobalDataBuffers[ objectIndex ];
            HELIUM_ASSERT( pBuffer );
            pBuffer->Unmap();
        }
    }

    for( size_t subMeshIndex = 0; subMeshIndex < subMeshCount; ++subMeshIndex )
    {
        if( m_mappedSubMeshVertexGlobalDataBuffers[ subMeshIndex ] )
        {
            RConstantBuffer* pBuffer = m_subMeshVertexGlobalDataBuffers[ subMeshIndex ];
            HELIUM_ASSERT( pBuffer );
            pBuffer->Unmap();
        }
    }
}

/// Render the specified scene view.
///
/// @param[in] viewIndex  Index of the scene view to render (can be an invalid element, but must be less than the size
///                       of the scene view sparse array).
void GraphicsScene::DrawSceneView( uint_fast32_t viewIndex )
{
    HELIUM_ASSERT( viewIndex < m_sceneViews.GetSize() );

    if( !m_sceneViews.IsElementValid( viewIndex ) )
    {
        return;
    }

    RConstantBuffer* pViewVertexGlobalDataBuffer =
        m_viewVertexGlobalDataBuffers[ m_constantBufferSetIndex ][ viewIndex ];
    if( !pViewVertexGlobalDataBuffer )
    {
        return;
    }

    GraphicsSceneView& rView = m_sceneViews[ viewIndex ];
    RRenderContext* pRenderContext = rView.GetRenderContext();
    if( !pRenderContext )
    {
        return;
    }

    // Determine which scene objects are visible in the current view.
    m_visibleSceneObjects.UnsetAll();

    const Simd::Frustum& rViewFrustum = rView.GetFrustum();

    size_t sceneObjectCount = m_sceneObjects.GetSize();
    for( size_t sceneObjectIndex = 0; sceneObjectIndex < sceneObjectCount; ++sceneObjectIndex )
    {
        if( m_sceneObjects.IsElementValid( sceneObjectIndex ) )
        {
            //const AaBox& rObjectBounds = m_sceneObjects[ sceneObjectIndex ].GetWorldBox();
            const Simd::Sphere& rObjectBounds = m_sceneObjects[ sceneObjectIndex ].GetWorldSphere();
            if( rViewFrustum.Intersects( rObjectBounds ) )
            {
                m_visibleSceneObjects.SetElement( sceneObjectIndex );
            }
        }
    }

    // Build a list of indices for each visible sub-mesh for sorting.
    m_sceneObjectSubMeshIndices.Resize( 0 );

    size_t subMeshCount = m_sceneObjectSubMeshes.GetSize();
    for( size_t subMeshIndex = 0; subMeshIndex < subMeshCount; ++subMeshIndex )
    {
        if( m_sceneObjectSubMeshes.IsElementValid( subMeshIndex ) )
        {
            size_t sceneObjectId = m_sceneObjectSubMeshes[ subMeshIndex ].GetSceneObjectId();
            HELIUM_ASSERT( sceneObjectId < m_visibleSceneObjects.GetSize() );
            if( m_visibleSceneObjects[ sceneObjectId ] )
            {
                m_sceneObjectSubMeshIndices.Push( subMeshIndex );
            }
        }
    }

    // Get the renderer interface and the main command proxy for the renderer.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );

    RRenderCommandProxyPtr spCommandProxy = pRenderer->GetImmediateCommandProxy();
    HELIUM_ASSERT( spCommandProxy );

    // Get the state objects that we will use during rendering.
    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();

    RRasterizerState* pRasterizerStateDefault = rRenderResourceManager.GetRasterizerState(
        RenderResourceManager::RASTERIZER_STATE_DEFAULT );
    RBlendState* pBlendStateOpaque = rRenderResourceManager.GetBlendState( RenderResourceManager::BLEND_STATE_OPAQUE );
    RDepthStencilState* pDepthStateDefault = rRenderResourceManager.GetDepthStencilState(
        RenderResourceManager::DEPTH_STENCIL_STATE_DEFAULT );
    RDepthStencilState* pDepthStateNone = rRenderResourceManager.GetDepthStencilState(
        RenderResourceManager::DEPTH_STENCIL_STATE_NONE );
    RSamplerState* pSamplerStatePointClamp = rRenderResourceManager.GetSamplerState(
        RenderResourceManager::TEXTURE_FILTER_POINT,
        RENDERER_TEXTURE_ADDRESS_MODE_CLAMP );

    // Set the default depth state.
    spCommandProxy->SetDepthStencilState( pDepthStateDefault, 0 );

    // Draw shadow depth pass (this will also set up the shadow depth scene as needed).
    DrawShadowDepthPass( viewIndex );

    // Set up normal scene rendering.
    RSurface* pDepthStencilSurface = rView.GetDepthStencilSurface();

    // Depth-stencil surfaces can be null, so we don't assert on the depth-stencil surface returned.
    RTexture2dPtr spSceneTexture = rRenderResourceManager.GetSceneTexture();
    HELIUM_ASSERT( spSceneTexture );
    RSurfacePtr spSceneTextureSurface = spSceneTexture->GetSurface( 0 );
    HELIUM_ASSERT( spSceneTextureSurface );

    spCommandProxy->SetRenderSurfaces( spSceneTextureSurface, pDepthStencilSurface );

    spCommandProxy->SetViewport(
        rView.GetViewportX(),
        rView.GetViewportY(),
        rView.GetViewportWidth(),
        rView.GetViewportHeight() );

    spCommandProxy->BeginScene();
    spCommandProxy->Clear( RENDERER_CLEAR_FLAG_ALL, rView.GetClearColor() );

    spCommandProxy->SetRasterizerState( pRasterizerStateDefault );
    spCommandProxy->SetVertexConstantBuffers( 0, 1, &pViewVertexGlobalDataBuffer );

    // Draw passes...
    DrawDepthPrePass( viewIndex );
    DrawBasePass( viewIndex );

#if !HELIUM_RELEASE && !HELIUM_PROFILE
    // Draw buffered world-space draw calls for the current scene and view.
    m_sceneBufferedDrawer.DrawWorldElements();

    if( viewIndex < m_viewBufferedDrawers.GetSize() )
    {
        BufferedDrawer* pDrawer = m_viewBufferedDrawers[ viewIndex ];
        if( pDrawer )
        {
            pDrawer->DrawWorldElements();
        }
    }
#endif  // !HELIUM_RELEASE && !HELIUM_PROFILE

    spCommandProxy->EndScene();

    // Draw the scene texture to the screen.
    RSurface* pBackBuffer = pRenderContext->GetBackBufferSurface();
    HELIUM_ASSERT( pBackBuffer );
    spCommandProxy->SetRenderSurfaces( pBackBuffer, NULL );

    spCommandProxy->SetViewport(
        rView.GetViewportX(),
        rView.GetViewportY(),
        rView.GetViewportWidth(),
        rView.GetViewportHeight() );

    spCommandProxy->BeginScene();

    spCommandProxy->SetRasterizerState( pRasterizerStateDefault );
    spCommandProxy->SetBlendState( pBlendStateOpaque );
    spCommandProxy->SetDepthStencilState( pDepthStateNone, 0 );
    spCommandProxy->SetSamplerStates( 0, 1, &pSamplerStatePointClamp );

    DynamicDrawer& rDynamicDrawer = DynamicDrawer::GetStaticInstance();
    rDynamicDrawer.Begin();

    float32_t viewportWidthFloat = static_cast< float32_t >( rView.GetViewportWidth() );
    float32_t viewportHeightFloat = static_cast< float32_t >( rView.GetViewportHeight() );

    Float16 zeroFloat16;
    zeroFloat16.packed = 0;

    Float32 floatPacker;

    floatPacker.value = viewportWidthFloat / static_cast< float32_t >( spSceneTexture->GetWidth() );
    Float16 sceneWidthFloat16 = Float32To16( floatPacker );

    floatPacker.value = viewportHeightFloat / static_cast< float32_t >( spSceneTexture->GetHeight() );
    Float16 sceneHeightFloat16 = Float32To16( floatPacker );

    float32_t halfPixelX = 1.0f / viewportWidthFloat;
    float32_t halfPixelY = 1.0f / viewportHeightFloat;

    float32_t quadMinX = -1.0f - halfPixelX;
    float32_t quadMinY = halfPixelY + 1.0f;
    float32_t quadMaxX = 1.0f - halfPixelX;
    float32_t quadMaxY = halfPixelY - 1.0f;

    rDynamicDrawer.DrawScreenSpaceQuad(
        SimpleTexturedVertex( quadMinX, quadMaxY, 0.0f, zeroFloat16, sceneHeightFloat16 ),
        SimpleTexturedVertex( quadMinX, quadMinY, 0.0f, zeroFloat16, zeroFloat16 ),
        SimpleTexturedVertex( quadMaxX, quadMinY, 0.0f, sceneWidthFloat16, zeroFloat16 ),
        SimpleTexturedVertex( quadMaxX, quadMaxY, 0.0f, sceneWidthFloat16, sceneHeightFloat16 ),
        spSceneTexture );
    rDynamicDrawer.Flush();

#if !HELIUM_RELEASE && !HELIUM_PROFILE
    // Draw buffered screen-space draw calls for the current scene and view.
    RConstantBuffer* pScreenSpaceVertexConstantBuffer = rView.GetScreenSpaceVertexConstantBuffer();
    spCommandProxy->SetVertexConstantBuffers( 0, 1, &pScreenSpaceVertexConstantBuffer );
    spCommandProxy->SetRasterizerState( pRasterizerStateDefault );

    RBlendState* pBlendStateTranslucent = rRenderResourceManager.GetBlendState(
        RenderResourceManager::BLEND_STATE_TRANSPARENT );
    spCommandProxy->SetBlendState( pBlendStateTranslucent );

    m_sceneBufferedDrawer.DrawScreenElements();

    if( viewIndex < m_viewBufferedDrawers.GetSize() )
    {
        BufferedDrawer* pDrawer = m_viewBufferedDrawers[ viewIndex ];
        if( pDrawer )
        {
            pDrawer->DrawScreenElements();
        }
    }
#endif  // !HELIUM_RELEASE && !HELIUM_PROFILE

    spCommandProxy->EndScene();

    spCommandProxy->UnbindResources();

    pRenderContext->Swap();
}

/// Draw the shadow depth render pass.
///
/// - The m_sceneObjectSubMeshIndices array should already be prepared with the (unsorted) list of visible sub
///   meshes.  This function will sort by depth if rendering is performed.
/// - Default rasterizer and depth states should be already set.
///
/// @param[in] viewIndex  Index of the view for which the shadow depth pass is being rendered.
///
/// @see DrawDepthPrePass(), DrawBasePass()
void GraphicsScene::DrawShadowDepthPass( uint_fast32_t viewIndex )
{
    HELIUM_ASSERT( viewIndex < m_sceneViews.GetSize() );
    HELIUM_ASSERT( m_sceneViews.IsElementValid( viewIndex ) );

    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();

    // Check whether shadows are enabled.
    GraphicsConfig::EShadowMode shadowMode = rRenderResourceManager.GetShadowMode();
    if( shadowMode == GraphicsConfig::EShadowMode::INVALID || shadowMode == GraphicsConfig::EShadowMode::NONE )
    {
        return;
    }

    // Make sure the pre-pass vertex shader resources exist.
    ShaderVariant* pPrePassVertexShaderVariant = rRenderResourceManager.GetPrePassVertexShader();
    if( !pPrePassVertexShaderVariant )
    {
        return;
    }

    Shader* pPrePassShader = pPrePassVertexShaderVariant->GetShader();
    HELIUM_ASSERT( pPrePassShader );
    const Shader::Options& rPrePassShaderSysOptions = pPrePassShader->GetSystemOptions();

    Shader::SelectPair optionSelectPair = { GetSkinningSysSelectName(), GetNoneOptionName() };
    size_t optionSetIndex = rPrePassShaderSysOptions.GetOptionSetIndex(
        RShader::TYPE_VERTEX,
        NULL,
        0,
        &optionSelectPair,
        1 );
    RShader* pPrePassShaderResource = pPrePassVertexShaderVariant->GetRenderResource( optionSetIndex );
    if( !pPrePassShaderResource )
    {
        return;
    }

    HELIUM_ASSERT( pPrePassShaderResource->GetType() == RShader::TYPE_VERTEX );
    RVertexShader* pPrePassNoSkinningVertexShader = static_cast< RVertexShader* >( pPrePassShaderResource );

    optionSelectPair.choice = GetSkinningSmoothOptionName();
    optionSetIndex = rPrePassShaderSysOptions.GetOptionSetIndex(
        RShader::TYPE_VERTEX,
        NULL,
        0,
        &optionSelectPair,
        1 );
    pPrePassShaderResource = pPrePassVertexShaderVariant->GetRenderResource( optionSetIndex );
    if( !pPrePassShaderResource )
    {
        return;
    }

    HELIUM_ASSERT( pPrePassShaderResource->GetType() == RShader::TYPE_VERTEX );
    RVertexShader* pPrePassSmoothSkinningVertexShader = static_cast< RVertexShader* >( pPrePassShaderResource );

    // Make sure the shadow depth pass constant buffer exists.
    RConstantBuffer* pShadowViewVertexDataBuffer =
        m_shadowViewVertexDataBuffers[ m_constantBufferSetIndex ][ viewIndex ];
    if( !pShadowViewVertexDataBuffer )
    {
        return;
    }

    // Retrieve the shadow depth texture resource (this should exist if shadows are enabled).
    RTexture2d* pShadowDepthTexture = rRenderResourceManager.GetShadowDepthTexture();
    HELIUM_ASSERT( pShadowDepthTexture );

    uint32_t shadowDepthTextureUsableSize = rRenderResourceManager.GetShadowDepthTextureUsableSize();
    HELIUM_ASSERT( shadowDepthTextureUsableSize <= pShadowDepthTexture->GetWidth() );
    HELIUM_ASSERT( shadowDepthTextureUsableSize <= pShadowDepthTexture->GetHeight() );

    RSurfacePtr spShadowDepthTextureSurface = pShadowDepthTexture->GetSurface( 0 );
    HELIUM_ASSERT( spShadowDepthTextureSurface );

    // Sort meshes based on distance from front to back in order to reduce overdraw.
    size_t subMeshIndexCount = m_sceneObjectSubMeshIndices.GetSize();

    {
        JobContext::Spawner< 1 > rootSpawner;

        JobContext* pContext = rootSpawner.Allocate();
        HELIUM_ASSERT( pContext );
        SortJob< size_t, SubMeshFrontToBackCompare >* pJob =
            pContext->Create< SortJob< size_t, SubMeshFrontToBackCompare > >();
        HELIUM_ASSERT( pJob );

        SortJob< size_t, SubMeshFrontToBackCompare >::Parameters& rParameters = pJob->GetParameters();
        rParameters.pBase = m_sceneObjectSubMeshIndices.GetData();
        rParameters.count = subMeshIndexCount;
        rParameters.compare = SubMeshFrontToBackCompare(
            m_directionalLightDirection,
            m_sceneObjects,
            m_sceneObjectSubMeshes );
        rParameters.singleJobCount = 100;
    }

    // Prepare the shadow depth pass scene for rendering.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );

    RRenderCommandProxyPtr spCommandProxy = pRenderer->GetImmediateCommandProxy();
    HELIUM_ASSERT( spCommandProxy );

    RTexture2d* pSceneTexture = rRenderResourceManager.GetSceneTexture();
    HELIUM_ASSERT( pSceneTexture );
    RSurfacePtr spSceneTextureSurface = pSceneTexture->GetSurface( 0 );
    HELIUM_ASSERT( spSceneTextureSurface );

    spCommandProxy->SetRenderSurfaces( spSceneTextureSurface, spShadowDepthTextureSurface );
    spCommandProxy->SetViewport( 0, 0, shadowDepthTextureUsableSize, shadowDepthTextureUsableSize );

    RRasterizerState* pRasterizerStateShadowDepth = rRenderResourceManager.GetRasterizerState(
        RenderResourceManager::RASTERIZER_STATE_SHADOW_DEPTH );
    spCommandProxy->SetRasterizerState( pRasterizerStateShadowDepth );

    RBlendState* pBlendStateNoColor = rRenderResourceManager.GetBlendState(
        RenderResourceManager::BLEND_STATE_NO_COLOR );
    spCommandProxy->SetBlendState( pBlendStateNoColor );

    // Draw the scene.
    spCommandProxy->BeginScene();
    spCommandProxy->Clear( RENDERER_CLEAR_FLAG_DEPTH );

    spCommandProxy->SetVertexConstantBuffers( 0, 1, &pShadowViewVertexDataBuffer );
    spCommandProxy->SetPixelShader( NULL );

    RVertexShader* pPreviousVertexShader = NULL;

    for( size_t meshIndexIndex = 0; meshIndexIndex < subMeshIndexCount; ++meshIndexIndex )
    {
        size_t meshIndex = m_sceneObjectSubMeshIndices[ meshIndexIndex ];
        HELIUM_ASSERT( m_sceneObjectSubMeshes.IsElementValid( meshIndex ) );

        GraphicsSceneObject::SubMeshData& rSubMeshData = m_sceneObjectSubMeshes[ meshIndex ];

        size_t sceneObjectId = rSubMeshData.GetSceneObjectId();
        HELIUM_ASSERT( IsValid( sceneObjectId ) );
        HELIUM_ASSERT( sceneObjectId < m_sceneObjects.GetSize() );
        HELIUM_ASSERT( m_sceneObjects.IsElementValid( sceneObjectId ) );

        HELIUM_ASSERT( meshIndex < m_subMeshVertexGlobalDataBuffers.GetSize() );
        RConstantBuffer* pInstanceVertexGlobalDataBuffer = m_subMeshVertexGlobalDataBuffers[ meshIndex ];
        if( !pInstanceVertexGlobalDataBuffer )
        {
            HELIUM_ASSERT( sceneObjectId < m_objectVertexGlobalDataBuffers.GetSize() );
            pInstanceVertexGlobalDataBuffer = m_objectVertexGlobalDataBuffers[ sceneObjectId ];
            if( !pInstanceVertexGlobalDataBuffer )
            {
                continue;
            }
        }

        GraphicsSceneObject& rSceneObject = m_sceneObjects[ sceneObjectId ];

        RVertexBuffer* pVertexBuffer = rSceneObject.GetVertexBuffer();
        if( !pVertexBuffer )
        {
            continue;
        }

        RVertexDescription* pVertexDescription = rSceneObject.GetVertexDescription();
        if( !pVertexDescription )
        {
            continue;
        }

        RIndexBuffer* pIndexBuffer = rSceneObject.GetIndexBuffer();
        if( !pIndexBuffer )
        {
            continue;
        }

        RVertexShader* pVertexShader;
        if( rSceneObject.GetBoneCount() == 0 || !rSceneObject.GetBonePalette() )
        {
            pVertexShader = pPrePassNoSkinningVertexShader;
        }
        else
        {
            pVertexShader = pPrePassSmoothSkinningVertexShader;
        }

        pVertexShader->CacheDescription( pRenderer, pVertexDescription );
        RVertexInputLayout* pInputLayout = pVertexShader->GetCachedInputLayout();
        if( !pInputLayout )
        {
            continue;
        }

        uint32_t vertexStride = rSceneObject.GetVertexStride();
        uint32_t offset = 0;

        ERendererPrimitiveType primitiveType = rSubMeshData.GetPrimitiveType();
        uint32_t primitiveCount = rSubMeshData.GetPrimitiveCount();
        uint32_t startVertex = rSubMeshData.GetStartVertex();
        uint32_t vertexRange = rSubMeshData.GetVertexRange();
        uint32_t startIndex = rSubMeshData.GetStartIndex();

        if( pPreviousVertexShader != pVertexShader )
        {
            spCommandProxy->SetVertexShader( pVertexShader );
            pPreviousVertexShader = pVertexShader;
        }

        spCommandProxy->SetVertexConstantBuffers( 1, 1, &pInstanceVertexGlobalDataBuffer );
        spCommandProxy->SetVertexBuffers( 0, 1, &pVertexBuffer, &vertexStride, &offset );
        spCommandProxy->SetIndexBuffer( pIndexBuffer );
        spCommandProxy->SetVertexInputLayout( pInputLayout );

        spCommandProxy->DrawIndexed(
            primitiveType,
            startVertex,
            0,
            vertexRange,
            startIndex,
            primitiveCount );
    }

    spCommandProxy->EndScene();
}

/// Draw the depth-only pre-pass for the given scene view.
///
/// - The m_sceneObjectSubMeshIndices array should already be prepared with the (unsorted) list of visible sub
///   meshes.  This function will sort by depth if rendering is performed.
/// - Standard viewport render surfaces are expected to have already been set, with the depth buffer cleared.
/// - Default rasterizer and depth states should be already set.
/// - Global per-view constant buffers should be already set.
///
/// @param[in] viewIndex  Index of the view for which the depth-only pre-pass is being rendered.
///
/// @see DrawShadowDepthPass(), DrawBasePass()
void GraphicsScene::DrawDepthPrePass( uint_fast32_t viewIndex )
{
    HELIUM_ASSERT( viewIndex < m_sceneViews.GetSize() );
    HELIUM_ASSERT( m_sceneViews.IsElementValid( viewIndex ) );

    // Make sure the pre-pass vertex shader resources exist.
    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();

    ShaderVariant* pPrePassVertexShaderVariant = rRenderResourceManager.GetPrePassVertexShader();
    if( !pPrePassVertexShaderVariant )
    {
        return;
    }

    Shader* pPrePassShader = pPrePassVertexShaderVariant->GetShader();
    HELIUM_ASSERT( pPrePassShader );
    const Shader::Options& rPrePassShaderSysOptions = pPrePassShader->GetSystemOptions();

    Shader::SelectPair optionSelectPair = { GetSkinningSysSelectName(), GetNoneOptionName() };
    size_t optionSetIndex = rPrePassShaderSysOptions.GetOptionSetIndex(
        RShader::TYPE_VERTEX,
        NULL,
        0,
        &optionSelectPair,
        1 );
    RShader* pPrePassShaderResource = pPrePassVertexShaderVariant->GetRenderResource( optionSetIndex );
    if( !pPrePassShaderResource )
    {
        return;
    }

    HELIUM_ASSERT( pPrePassShaderResource->GetType() == RShader::TYPE_VERTEX );
    RVertexShader* pPrePassNoSkinningVertexShader = static_cast< RVertexShader* >( pPrePassShaderResource );

    optionSelectPair.choice = GetSkinningSmoothOptionName();
    optionSetIndex = rPrePassShaderSysOptions.GetOptionSetIndex(
        RShader::TYPE_VERTEX,
        NULL,
        0,
        &optionSelectPair,
        1 );
    pPrePassShaderResource = pPrePassVertexShaderVariant->GetRenderResource( optionSetIndex );
    if( !pPrePassShaderResource )
    {
        return;
    }

    HELIUM_ASSERT( pPrePassShaderResource->GetType() == RShader::TYPE_VERTEX );
    RVertexShader* pPrePassSmoothSkinningVertexShader = static_cast< RVertexShader* >( pPrePassShaderResource );

    // Sort meshes based on distance from front to back in order to reduce overdraw.
    GraphicsSceneView& rView = m_sceneViews[ viewIndex ];
    const Simd::Vector3& rViewDirection = rView.GetForward();

    size_t subMeshIndexCount = m_sceneObjectSubMeshIndices.GetSize();

    {
        JobContext::Spawner< 1 > rootSpawner;

        JobContext* pContext = rootSpawner.Allocate();
        HELIUM_ASSERT( pContext );
        SortJob< size_t, SubMeshFrontToBackCompare >* pJob =
            pContext->Create< SortJob< size_t, SubMeshFrontToBackCompare > >();
        HELIUM_ASSERT( pJob );

        SortJob< size_t, SubMeshFrontToBackCompare >::Parameters& rParameters = pJob->GetParameters();
        rParameters.pBase = m_sceneObjectSubMeshIndices.GetData();
        rParameters.count = subMeshIndexCount;
        rParameters.compare = SubMeshFrontToBackCompare( rViewDirection, m_sceneObjects, m_sceneObjectSubMeshes );
        rParameters.singleJobCount = 100;
    }

    // Initialize the blend state and shaders for performing no color writes.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );

    RRenderCommandProxyPtr spCommandProxy = pRenderer->GetImmediateCommandProxy();
    HELIUM_ASSERT( spCommandProxy );

    RBlendState* pBlendStateNoColor = rRenderResourceManager.GetBlendState(
        RenderResourceManager::BLEND_STATE_NO_COLOR );
    spCommandProxy->SetBlendState( pBlendStateNoColor );

    spCommandProxy->SetPixelShader( NULL );

    // Draw each visible mesh instance.
    RVertexShader* pPreviousVertexShader = NULL;

    for( size_t meshIndexIndex = 0; meshIndexIndex < subMeshIndexCount; ++meshIndexIndex )
    {
        size_t meshIndex = m_sceneObjectSubMeshIndices[ meshIndexIndex ];
        HELIUM_ASSERT( m_sceneObjectSubMeshes.IsElementValid( meshIndex ) );

        GraphicsSceneObject::SubMeshData& rSubMeshData = m_sceneObjectSubMeshes[ meshIndex ];

        size_t sceneObjectId = rSubMeshData.GetSceneObjectId();
        HELIUM_ASSERT( IsValid( sceneObjectId ) );
        HELIUM_ASSERT( sceneObjectId < m_sceneObjects.GetSize() );
        HELIUM_ASSERT( m_sceneObjects.IsElementValid( sceneObjectId ) );

        HELIUM_ASSERT( meshIndex < m_subMeshVertexGlobalDataBuffers.GetSize() );
        RConstantBuffer* pInstanceVertexGlobalDataBuffer = m_subMeshVertexGlobalDataBuffers[ meshIndex ];
        if( !pInstanceVertexGlobalDataBuffer )
        {
            HELIUM_ASSERT( sceneObjectId < m_objectVertexGlobalDataBuffers.GetSize() );
            pInstanceVertexGlobalDataBuffer = m_objectVertexGlobalDataBuffers[ sceneObjectId ];
            if( !pInstanceVertexGlobalDataBuffer )
            {
                continue;
            }
        }

        GraphicsSceneObject& rSceneObject = m_sceneObjects[ sceneObjectId ];

        RVertexBuffer* pVertexBuffer = rSceneObject.GetVertexBuffer();
        if( !pVertexBuffer )
        {
            continue;
        }

        RVertexDescription* pVertexDescription = rSceneObject.GetVertexDescription();
        if( !pVertexDescription )
        {
            continue;
        }

        RIndexBuffer* pIndexBuffer = rSceneObject.GetIndexBuffer();
        if( !pIndexBuffer )
        {
            continue;
        }

        RVertexShader* pVertexShader;
        if( rSceneObject.GetBoneCount() == 0 || !rSceneObject.GetBonePalette() )
        {
            pVertexShader = pPrePassNoSkinningVertexShader;
        }
        else
        {
            pVertexShader = pPrePassSmoothSkinningVertexShader;
        }

        pVertexShader->CacheDescription( pRenderer, pVertexDescription );
        RVertexInputLayout* pInputLayout = pVertexShader->GetCachedInputLayout();
        if( !pInputLayout )
        {
            continue;
        }

        uint32_t vertexStride = rSceneObject.GetVertexStride();
        uint32_t offset = 0;

        ERendererPrimitiveType primitiveType = rSubMeshData.GetPrimitiveType();
        uint32_t primitiveCount = rSubMeshData.GetPrimitiveCount();
        uint32_t startVertex = rSubMeshData.GetStartVertex();
        uint32_t vertexRange = rSubMeshData.GetVertexRange();
        uint32_t startIndex = rSubMeshData.GetStartIndex();

        if( pPreviousVertexShader != pVertexShader )
        {
            spCommandProxy->SetVertexShader( pVertexShader );
            pPreviousVertexShader = pVertexShader;
        }

        spCommandProxy->SetVertexConstantBuffers( 1, 1, &pInstanceVertexGlobalDataBuffer );
        spCommandProxy->SetVertexBuffers( 0, 1, &pVertexBuffer, &vertexStride, &offset );
        spCommandProxy->SetIndexBuffer( pIndexBuffer );
        spCommandProxy->SetVertexInputLayout( pInputLayout );

        spCommandProxy->DrawIndexed(
            primitiveType,
            startVertex,
            0,
            vertexRange,
            startIndex,
            primitiveCount );
    }
}

/// Draw the base pass for the given scene view.
///
/// - The m_sceneObjectSubMeshIndices array should already be prepared with the (unsorted) list of visible sub
///   meshes.  This function will sort as appropriate if rendering is performed.
/// - Standard viewport render surfaces are expected to have already been set, with the depth buffer either cleared
///   or prepared by the depth-only pre-pass.
/// - Default rasterizer and depth states should be already set.
/// - Global per-view constant buffers should be already set (buffers specific to the base pass will be set by this
///   function).
///
/// @param[in] viewIndex  Index of the view for which the base pass is being rendered.
///
/// @see DrawShadowDepthPass(), DrawDepthPrePass()
void GraphicsScene::DrawBasePass( uint_fast32_t viewIndex )
{
    HELIUM_ASSERT( viewIndex < m_sceneViews.GetSize() );
    HELIUM_ASSERT( m_sceneViews.IsElementValid( viewIndex ) );

    // Make sure per-view constant buffers for the base pass exist.
    RConstantBuffer* pViewVertexBasePassDataBuffer =
        m_viewVertexBasePassDataBuffers[ m_constantBufferSetIndex ][ viewIndex ];
    if( !pViewVertexBasePassDataBuffer )
    {
        return;
    }

    RConstantBuffer* pViewPixelBasePassDataBuffer =
        m_viewPixelBasePassDataBuffers[ m_constantBufferSetIndex ][ viewIndex ];
    if( !pViewPixelBasePassDataBuffer )
    {
        return;
    }

    // Build the list of system options for retrieving the proper material shader variant to use for rendering.
    static Name shadowsSysSelectName( TXT( "SHADOWS" ) );

    static const Name shadowSelectOptions[] =
    {
        GetNoneOptionName(),
        Name( TXT( "SHADOWS_SIMPLE" ) ),
        Name( TXT( "SHADOWS_PCF_DITHERED" ) )
    };

    Shader::SelectPair systemSelections[] =
    {
        { Name( TXT( "SHADOWS" ) ), Name( NULL_NAME ) },
        { GetSkinningSysSelectName(), Name( NULL_NAME ) }
    };

    BOOST_STATIC_ASSERT( HELIUM_ARRAY_COUNT( shadowSelectOptions ) == GraphicsConfig::EShadowMode::MAX );

    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();

    GraphicsConfig::EShadowMode shadowMode = rRenderResourceManager.GetShadowMode();
    if( static_cast< size_t >( shadowMode ) >= GraphicsConfig::EShadowMode::MAX )
    {
        shadowMode = GraphicsConfig::EShadowMode::NONE;
    }

    systemSelections[ 0 ].choice = shadowSelectOptions[ shadowMode ];

    // Sort meshes based on material in order to reduce shader switches.
    size_t subMeshIndexCount = m_sceneObjectSubMeshIndices.GetSize();

    {
        JobContext::Spawner< 1 > rootSpawner;

        JobContext* pContext = rootSpawner.Allocate();
        HELIUM_ASSERT( pContext );
        SortJob< size_t, SubMeshMaterialCompare >* pJob =
            pContext->Create< SortJob< size_t, SubMeshMaterialCompare > >();
        HELIUM_ASSERT( pJob );

        SortJob< size_t, SubMeshMaterialCompare >::Parameters& rParameters = pJob->GetParameters();
        rParameters.pBase = m_sceneObjectSubMeshIndices.GetData();
        rParameters.count = subMeshIndexCount;
        rParameters.compare = SubMeshMaterialCompare( m_sceneObjectSubMeshes );
        rParameters.singleJobCount = 100;
    }

    // Set the opaque rendering blend state and per-view constant buffers for this pass.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );

    RRenderCommandProxyPtr spCommandProxy = pRenderer->GetImmediateCommandProxy();
    HELIUM_ASSERT( spCommandProxy );

    RBlendState* pBlendStateOpaque = rRenderResourceManager.GetBlendState(
        RenderResourceManager::BLEND_STATE_OPAQUE );
    spCommandProxy->SetBlendState( pBlendStateOpaque );

    spCommandProxy->SetVertexConstantBuffers( 1, 1, &pViewVertexBasePassDataBuffer );
    spCommandProxy->SetPixelConstantBuffers( 0, 1, &pViewPixelBasePassDataBuffer );

    // Draw each visible sub-mesh.
    Name defaultSamplerStateName = GetDefaultSamplerStateName();
    Name shadowSamplerStateName = GetShadowSamplerStateName();
    Name shadowMapTextureName = GetShadowMapTextureName();

    RSamplerState* pSamplerStateDefault = rRenderResourceManager.GetSamplerState(
        RenderResourceManager::TEXTURE_FILTER_LINEAR,
        RENDERER_TEXTURE_ADDRESS_MODE_WRAP );
    RSamplerState* pSamplerStateShadowMap = rRenderResourceManager.GetSamplerState(
        RenderResourceManager::TEXTURE_FILTER_LINEAR,
        RENDERER_TEXTURE_ADDRESS_MODE_CLAMP );

    RTexture2d* pShadowDepthTexture = rRenderResourceManager.GetShadowDepthTexture();

    RVertexShader* pPreviousVertexShader = NULL;
    RPixelShader* pPreviousPixelShader = NULL;
    RConstantBuffer* pPreviousMaterialVertexConstantBuffer = NULL;
    RConstantBuffer* pPreviousMaterialPixelConstantBuffer = NULL;

    for( size_t meshIndexIndex = 0; meshIndexIndex < subMeshIndexCount; ++meshIndexIndex )
    {
        size_t meshIndex = m_sceneObjectSubMeshIndices[ meshIndexIndex ];
        HELIUM_ASSERT( m_sceneObjectSubMeshes.IsElementValid( meshIndex ) );

        GraphicsSceneObject::SubMeshData& rSubMeshData = m_sceneObjectSubMeshes[ meshIndex ];

        size_t sceneObjectId = rSubMeshData.GetSceneObjectId();
        HELIUM_ASSERT( IsValid( sceneObjectId ) );
        HELIUM_ASSERT( sceneObjectId < m_sceneObjects.GetSize() );
        HELIUM_ASSERT( m_sceneObjects.IsElementValid( sceneObjectId ) );

        HELIUM_ASSERT( meshIndex < m_subMeshVertexGlobalDataBuffers.GetSize() );
        RConstantBuffer* pInstanceVertexGlobalDataBuffer = m_subMeshVertexGlobalDataBuffers[ meshIndex ];
        if( !pInstanceVertexGlobalDataBuffer )
        {
            HELIUM_ASSERT( sceneObjectId < m_objectVertexGlobalDataBuffers.GetSize() );
            pInstanceVertexGlobalDataBuffer = m_objectVertexGlobalDataBuffers[ sceneObjectId ];
            if( !pInstanceVertexGlobalDataBuffer )
            {
                continue;
            }
        }

        GraphicsSceneObject& rSceneObject = m_sceneObjects[ sceneObjectId ];

        RVertexBuffer* pVertexBuffer = rSceneObject.GetVertexBuffer();
        if( !pVertexBuffer )
        {
            continue;
        }

        RVertexDescription* pVertexDescription = rSceneObject.GetVertexDescription();
        if( !pVertexDescription )
        {
            continue;
        }

        RIndexBuffer* pIndexBuffer = rSceneObject.GetIndexBuffer();
        if( !pIndexBuffer )
        {
            continue;
        }

        Material* pMaterial = rSubMeshData.GetMaterial();
        if( !pMaterial )
        {
            continue;
        }

        Shader* pShaderResource = pMaterial->GetShader();
        if( !pShaderResource )
        {
            continue;
        }

        ShaderVariant* pVertexShaderVariant = pMaterial->GetShaderVariant( RShader::TYPE_VERTEX );
        if( !pVertexShaderVariant )
        {
            continue;
        }

        ShaderVariant* pPixelShaderVariant = pMaterial->GetShaderVariant( RShader::TYPE_PIXEL );
        if( !pPixelShaderVariant )
        {
            continue;
        }

        if( rSceneObject.GetBoneCount() == 0 || !rSceneObject.GetBonePalette() )
        {
            systemSelections[ 1 ].choice = GetNoneOptionName();
        }
        else
        {
            systemSelections[ 1 ].choice = GetSkinningSmoothOptionName();
        }

        const Shader::Options& rSystemOptions = pShaderResource->GetSystemOptions();
        size_t vertexShaderIndex = rSystemOptions.GetOptionSetIndex(
            RShader::TYPE_VERTEX,
            NULL,
            0,
            systemSelections,
            HELIUM_ARRAY_COUNT( systemSelections ) );
        size_t pixelShaderIndex = rSystemOptions.GetOptionSetIndex(
            RShader::TYPE_PIXEL,
            NULL,
            0,
            systemSelections,
            HELIUM_ARRAY_COUNT( systemSelections ) );

        RVertexShader* pVertexShader =
            static_cast< RVertexShader* >( pVertexShaderVariant->GetRenderResource( vertexShaderIndex ) );
        if( !pVertexShader )
        {
            continue;
        }

        RPixelShader* pPixelShader =
            static_cast< RPixelShader* >( pPixelShaderVariant->GetRenderResource( pixelShaderIndex ) );
        if( !pPixelShader )
        {
            continue;
        }

        pVertexShader->CacheDescription( pRenderer, pVertexDescription );
        RVertexInputLayout* pInputLayout = pVertexShader->GetCachedInputLayout();
        if( !pInputLayout )
        {
            continue;
        }

        RConstantBuffer* pMaterialVertexConstantBuffer = pMaterial->GetConstantBuffer(
            RShader::TYPE_VERTEX );
        RConstantBuffer* pMaterialPixelConstantBuffer = pMaterial->GetConstantBuffer(
            RShader::TYPE_PIXEL );

        uint32_t vertexStride = rSceneObject.GetVertexStride();
        uint32_t offset = 0;

        ERendererPrimitiveType primitiveType = rSubMeshData.GetPrimitiveType();
        uint32_t primitiveCount = rSubMeshData.GetPrimitiveCount();
        uint32_t startVertex = rSubMeshData.GetStartVertex();
        uint32_t vertexRange = rSubMeshData.GetVertexRange();
        uint32_t startIndex = rSubMeshData.GetStartIndex();

        spCommandProxy->SetVertexConstantBuffers( 2, 1, &pInstanceVertexGlobalDataBuffer );

        if( pMaterialVertexConstantBuffer != pPreviousMaterialVertexConstantBuffer )
        {
            spCommandProxy->SetVertexConstantBuffers( 3, 1, &pMaterialVertexConstantBuffer );
            pPreviousMaterialVertexConstantBuffer = pMaterialVertexConstantBuffer;
        }

        if( pMaterialPixelConstantBuffer != pPreviousMaterialPixelConstantBuffer )
        {
            spCommandProxy->SetPixelConstantBuffers( 1, 1, &pMaterialPixelConstantBuffer );
            pPreviousMaterialPixelConstantBuffer = pMaterialPixelConstantBuffer;
        }

        spCommandProxy->SetVertexBuffers( 0, 1, &pVertexBuffer, &vertexStride, &offset );
        spCommandProxy->SetIndexBuffer( pIndexBuffer );

        if( pVertexShader != pPreviousVertexShader )
        {
            spCommandProxy->SetVertexShader( pVertexShader );
            pPreviousVertexShader = pVertexShader;
        }

        if( pPixelShader != pPreviousPixelShader )
        {
            spCommandProxy->SetPixelShader( pPixelShader );
            pPreviousPixelShader = pPixelShader;
        }

        spCommandProxy->SetVertexInputLayout( pInputLayout );

        const ShaderSamplerInfoSet* pSamplerInfoSet = pPixelShaderVariant->GetSamplerInfoSet( 0 );
        if( pSamplerInfoSet )
        {
            const DynArray< ShaderSamplerInfo >& samplerInputs = pSamplerInfoSet->inputs;
            size_t samplerInputCount = samplerInputs.GetSize();
            for( size_t inputIndex = 0; inputIndex < samplerInputCount; ++inputIndex )
            {
                const ShaderSamplerInfo& rInputInfo = samplerInputs[ inputIndex ];
                Name samplerName = rInputInfo.name;

                RSamplerState* pSamplerState = NULL;
                if( samplerName == defaultSamplerStateName )
                {
                    pSamplerState = pSamplerStateDefault;
                }
                else if( samplerName == shadowSamplerStateName ||  // Shader model 4+
                    samplerName == shadowMapTextureName )     // Older shader versions
                {
                    pSamplerState = pSamplerStateShadowMap;
                }

                spCommandProxy->SetSamplerStates( rInputInfo.bindIndex, 1, &pSamplerState );
            }
        }

        const ShaderTextureInfoSet* pTextureInfoSet = pPixelShaderVariant->GetTextureInfoSet( 0 );
        if( pTextureInfoSet )
        {
            size_t materialTextureCount = pMaterial->GetTextureParameterCount();

            const DynArray< ShaderTextureInfo >& textureInputs = pTextureInfoSet->inputs;
            size_t textureInputCount = textureInputs.GetSize();
            for( size_t inputIndex = 0; inputIndex < textureInputCount; ++inputIndex )
            {
                const ShaderTextureInfo& rInputInfo = textureInputs[ inputIndex ];
                Name textureName = rInputInfo.name;

                RTexture* pTextureResource = NULL;

                if( textureName == shadowMapTextureName )
                {
                    pTextureResource = pShadowDepthTexture;
                }
                else
                {
                    for( size_t materialTextureIndex = 0;
                        materialTextureIndex < materialTextureCount;
                        ++materialTextureIndex )
                    {
                        const Material::TextureParameter& rTextureParameter = pMaterial->GetTextureParameter(
                            materialTextureIndex );
                        if( rTextureParameter.name == textureName )
                        {
                            Texture* pTexture = rTextureParameter.value;
                            if( pTexture )
                            {
                                pTextureResource = pTexture->GetRenderResource();
                            }

                            break;
                        }
                    }
                }

                spCommandProxy->SetTexture( rInputInfo.bindIndex, pTextureResource );
            }
        }

        spCommandProxy->DrawIndexed(
            primitiveType,
            startVertex,
            0,
            vertexRange,
            startIndex,
            primitiveCount );
    }
}

/// Get a name identifier for "NONE" select options.
///
/// @return  Name for the string "NONE".
Name GraphicsScene::GetNoneOptionName()
{
    static Name noneOptionName( TXT( "NONE" ) );

    return noneOptionName;
}

/// Get the name of the skinning system select for shaders.
///
/// @return  Skinning system select name.
///
/// @see GetSkinningSmoothOptionName(), GetSkinningRigidOptionName()
Name GraphicsScene::GetSkinningSysSelectName()
{
    static Name skinningSysSelectName( TXT( "SKINNING" ) );

    return skinningSysSelectName;
}

/// Get the name of the smooth skinning system select option for shaders.
///
/// @return  Smooth skinning select option name.
///
/// @see GetSkinningSysSelectName(), GetSkinningRigidOptionName()
Name GraphicsScene::GetSkinningSmoothOptionName()
{
    static Name skinningSmoothOptionName( TXT( "SKINNING_SMOOTH" ) );

    return skinningSmoothOptionName;
}

/// Get the name of the rigid skinning system select option for shaders.
///
/// @return  Rigid skinning select option name.
///
/// @see GetSkinningSysSelectName(), GetSkinningSmoothOptionName()
Name GraphicsScene::GetSkinningRigidOptionName()
{
    static Name skinningRigidOptionName( TXT( "SKINNING_RIGID" ) );

    return skinningRigidOptionName;
}

/// Constructor.
GraphicsScene::SubMeshFrontToBackCompare::SubMeshFrontToBackCompare()
: m_cameraDirection( 0.0f )
, m_pSceneObjects( NULL )
, m_pSubMeshes( NULL )
{
}

/// Constructor.
///
/// @param[in] rCameraDirection  Camera world direction.
/// @param[in] rSceneObjects     List of graphics scene objects in the scene.
/// @param[in] rSubMeshes        List of scene object sub-meshes in the scene.
GraphicsScene::SubMeshFrontToBackCompare::SubMeshFrontToBackCompare(
    const Simd::Vector3& rCameraDirection,
    const SparseArray< GraphicsSceneObject >& rSceneObjects,
    const SparseArray< GraphicsSceneObject::SubMeshData >& rSubMeshes )
    : m_cameraDirection( rCameraDirection )
    , m_pSceneObjects( &rSceneObjects )
    , m_pSubMeshes( &rSubMeshes )
{
}

/// Compare two sub-meshes for sorting.
///
/// @param[in] subMeshIndex0  Index of the first sub-mesh to compare.
/// @param[in] subMeshIndex1  Index of the second sub-mesh to compare.
///
/// @return  True if the first sub-mesh should be sorted before the second, false if it should be sorted after or if
///          they share the same sorting priority.
bool GraphicsScene::SubMeshFrontToBackCompare::operator()( size_t subMeshIndex0, size_t subMeshIndex1 ) const
{
    const GraphicsSceneObject::SubMeshData& rSubMesh0 = m_pSubMeshes->GetElement( subMeshIndex0 );
    const GraphicsSceneObject::SubMeshData& rSubMesh1 = m_pSubMeshes->GetElement( subMeshIndex1 );

    size_t sceneObjectIndex0 = rSubMesh0.GetSceneObjectId();
    HELIUM_ASSERT( m_pSceneObjects->IsElementValid( sceneObjectIndex0 ) );
    size_t sceneObjectIndex1 = rSubMesh1.GetSceneObjectId();
    HELIUM_ASSERT( m_pSceneObjects->IsElementValid( sceneObjectIndex1 ) );

    const GraphicsSceneObject& rSceneObject0 = m_pSceneObjects->GetElement( sceneObjectIndex0 );
    const GraphicsSceneObject& rSceneObject1 = m_pSceneObjects->GetElement( sceneObjectIndex1 );

    Simd::Vector3 object0Pos = Simd::Vector4ToVector3( rSceneObject0.GetTransform().GetRow( 3 ) );
    Simd::Vector3 object1Pos = Simd::Vector4ToVector3( rSceneObject1.GetTransform().GetRow( 3 ) );

    float distance0 = object0Pos.Dot( m_cameraDirection );
    float distance1 = object1Pos.Dot( m_cameraDirection );

    return ( distance0 < distance1 );
}

/// Constructor.
GraphicsScene::SubMeshMaterialCompare::SubMeshMaterialCompare()
: m_pSubMeshes( NULL )
{
}

/// Constructor.
///
/// @param[in] rSubMeshes  List of scene object sub-meshes in the scene.
GraphicsScene::SubMeshMaterialCompare::SubMeshMaterialCompare(
    const SparseArray< GraphicsSceneObject::SubMeshData >& rSubMeshes )
    : m_pSubMeshes( &rSubMeshes )
{
}

/// Compare two sub-meshes for sorting.
///
/// @param[in] subMeshIndex0  Index of the first sub-mesh to compare.
/// @param[in] subMeshIndex1  Index of the second sub-mesh to compare.
///
/// @return  True if the first sub-mesh should be sorted before the second, false if it should be sorted after or if
///          they share the same sorting priority.
bool GraphicsScene::SubMeshMaterialCompare::operator()( size_t subMeshIndex0, size_t subMeshIndex1 ) const
{
    const GraphicsSceneObject::SubMeshData& rSubMesh0 = m_pSubMeshes->GetElement( subMeshIndex0 );
    const GraphicsSceneObject::SubMeshData& rSubMesh1 = m_pSubMeshes->GetElement( subMeshIndex1 );

    Material* pMaterial0 = rSubMesh0.GetMaterial();
    Material* pMaterial1 = rSubMesh1.GetMaterial();
    if( pMaterial0 == pMaterial1 )
    {
        return false;
    }

    if( !pMaterial0 )
    {
        return true;
    }

    if( !pMaterial1 )
    {
        return false;
    }

    ShaderVariant* pVariant0 = pMaterial0->GetShaderVariant( RShader::TYPE_VERTEX );
    ShaderVariant* pVariant1 = pMaterial1->GetShaderVariant( RShader::TYPE_VERTEX );
    if( pVariant0 != pVariant1 )
    {
        return ( pVariant0 < pVariant1 );
    }

    pVariant0 = pMaterial0->GetShaderVariant( RShader::TYPE_PIXEL );
    pVariant1 = pMaterial1->GetShaderVariant( RShader::TYPE_PIXEL );

    return ( pVariant0 < pVariant1 );
}
