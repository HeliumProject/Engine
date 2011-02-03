#include "GraphicsPch.h"
#include "Graphics/BufferedDrawer.h"

#include "Platform/Math/Simd/Matrix44.h"
#include "Foundation/StringConverter.h"
#include "Rendering/Renderer.h"
#include "Rendering/RendererUtil.h"
#include "Rendering/RConstantBuffer.h"
#include "Rendering/RFence.h"
#include "Rendering/RIndexBuffer.h"
#include "Rendering/RPixelShader.h"
#include "Rendering/RRenderCommandProxy.h"
#include "Rendering/RVertexBuffer.h"
#include "Rendering/RVertexShader.h"
#include "Graphics/Font.h"
#include "Graphics/Shader.h"

using namespace Lunar;

/// Constructor.
BufferedDrawer::BufferedDrawer()
    : m_currentResourceSetIndex( 0 )
    , m_bDrawing( false )
{
    for( size_t resourceSetIndex = 0; resourceSetIndex < HELIUM_ARRAY_COUNT( m_resourceSets ); ++resourceSetIndex )
    {
        ResourceSet& rResourceSet = m_resourceSets[ resourceSetIndex ];
        rResourceSet.untexturedVertexBufferSize = 0;
        rResourceSet.untexturedIndexBufferSize = 0;
        rResourceSet.texturedVertexBufferSize = 0;
        rResourceSet.texturedIndexBufferSize = 0;
        rResourceSet.screenSpaceTextVertexBufferSize = 0;
        rResourceSet.instanceVertexConstantTransform = Simd::Matrix44::IDENTITY;
        SetInvalid( rResourceSet.instanceVertexConstantBufferIndex );
        rResourceSet.instancePixelConstantBlendColor = Color( 0xffffffff );
        SetInvalid( rResourceSet.instancePixelConstantBufferIndex );
    }
}

/// Destructor.
BufferedDrawer::~BufferedDrawer()
{
}

/// Initialize this buffered drawing interface.
///
/// @return  True if initialization was successful, false if not.
///
/// @see Shutdown()
bool BufferedDrawer::Initialize()
{
    Shutdown();

    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( pRenderer )
    {
        // Allocate the index buffer to use for screen-space text rendering.
        uint16_t quadIndices[ 6 ] = { 0, 1, 2, 0, 2, 3 };

        m_spScreenSpaceTextIndexBuffer = pRenderer->CreateIndexBuffer(
            sizeof( uint16_t ) * 6,
            RENDERER_BUFFER_USAGE_STATIC,
            RENDERER_INDEX_FORMAT_UINT16,
            quadIndices );
        if( !m_spScreenSpaceTextIndexBuffer )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "BufferedDrawer::Initialize(): Failed to create index buffer for screen-space text " )
                  TXT( "rendering.\n" ) ) );

            return false;
        }

        // Allocate constant buffers for per-instance vertex and pixel shader constants.
        for( size_t resourceSetIndex = 0; resourceSetIndex < HELIUM_ARRAY_COUNT( m_resourceSets ); ++resourceSetIndex )
        {
            ResourceSet& rResourceSet = m_resourceSets[ resourceSetIndex ];

            for( size_t bufferIndex = 0;
                 bufferIndex < HELIUM_ARRAY_COUNT( rResourceSet.instanceVertexConstantBuffers );
                 ++bufferIndex )
            {
                RConstantBuffer* pConstantBuffer = pRenderer->CreateConstantBuffer(
                    sizeof( float32_t ) * 16,
                    RENDERER_BUFFER_USAGE_DYNAMIC );
                rResourceSet.instanceVertexConstantBuffers[ bufferIndex ] = pConstantBuffer;
                if( !pConstantBuffer )
                {
                    HELIUM_TRACE(
                        TRACE_ERROR,
                        ( TXT( "BufferedDrawer::Initialize(): Failed to allocate per-instance vertex shader constant " )
                          TXT( "buffers.\n" ) ) );

                    Shutdown();

                    return false;
                }
            }

            for( size_t bufferIndex = 0;
                 bufferIndex < HELIUM_ARRAY_COUNT( rResourceSet.instancePixelConstantBuffers );
                 ++bufferIndex )
            {
                RConstantBuffer* pConstantBuffer = pRenderer->CreateConstantBuffer(
                    sizeof( float32_t ) * 4,
                    RENDERER_BUFFER_USAGE_DYNAMIC );
                rResourceSet.instancePixelConstantBuffers[ bufferIndex ] = pConstantBuffer;
                if( !pConstantBuffer )
                {
                    HELIUM_TRACE(
                        TRACE_ERROR,
                        ( TXT( "BufferedDrawer::Initialize(): Failed to allocate per-instance pixel shader constant " )
                          TXT( "buffers.\n" ) ) );

                    Shutdown();

                    return false;
                }
            }
        }
    }

    return true;
}

/// Shut down this buffered drawing interface and free any allocated resources.
///
/// @see Initialize()
void BufferedDrawer::Shutdown()
{
    m_untexturedVertices.Clear();
    m_texturedVertices.Clear();

    m_untexturedIndices.Clear();
    m_texturedIndices.Clear();

    for( size_t depthModeIndex = 0; depthModeIndex < static_cast< size_t >( DEPTH_MODE_MAX ); ++depthModeIndex )
    {
        m_wireDrawCalls[ depthModeIndex ].Clear();
        m_solidDrawCalls[ depthModeIndex ].Clear();
        m_texturedDrawCalls[ depthModeIndex ].Clear();
        m_pointDrawCalls[ depthModeIndex ].Clear();

        m_wireBufferDrawCalls[ depthModeIndex ].Clear();
        m_solidBufferDrawCalls[ depthModeIndex ].Clear();
        m_texturedBufferDrawCalls[ depthModeIndex ].Clear();
        m_pointBufferDrawCalls[ depthModeIndex ].Clear();

        m_worldTextDrawCalls[ depthModeIndex ].Clear();
    }

    m_screenTextDrawCalls.Clear();
    m_screenTextGlyphIndices.Clear();

    m_spScreenSpaceTextIndexBuffer.Release();

    for( size_t fenceIndex = 0; fenceIndex < HELIUM_ARRAY_COUNT( m_instanceVertexConstantFences ); ++fenceIndex )
    {
        m_instanceVertexConstantFences[ fenceIndex ].Release();
    }

    for( size_t fenceIndex = 0; fenceIndex < HELIUM_ARRAY_COUNT( m_instancePixelConstantFences ); ++fenceIndex )
    {
        m_instancePixelConstantFences[ fenceIndex ].Release();
    }

    for( size_t resourceSetIndex = 0; resourceSetIndex < HELIUM_ARRAY_COUNT( m_resourceSets ); ++resourceSetIndex )
    {
        ResourceSet& rResourceSet = m_resourceSets[ resourceSetIndex ];
        rResourceSet.spUntexturedVertexBuffer.Release();
        rResourceSet.spUntexturedIndexBuffer.Release();
        rResourceSet.spTexturedVertexBuffer.Release();
        rResourceSet.spTexturedIndexBuffer.Release();
        rResourceSet.spScreenSpaceTextVertexBuffer.Release();
        rResourceSet.instanceVertexConstantTransform = Simd::Matrix44::IDENTITY;
        SetInvalid( rResourceSet.instanceVertexConstantBufferIndex );
        rResourceSet.instancePixelConstantBlendColor = Color( 0xffffffff );
        SetInvalid( rResourceSet.instancePixelConstantBufferIndex );
        rResourceSet.untexturedVertexBufferSize = 0;
        rResourceSet.untexturedIndexBufferSize = 0;
        rResourceSet.texturedVertexBufferSize = 0;
        rResourceSet.texturedIndexBufferSize = 0;
        rResourceSet.screenSpaceTextVertexBufferSize = 0;

        for( size_t bufferIndex = 0;
             bufferIndex < HELIUM_ARRAY_COUNT( rResourceSet.instancePixelConstantBuffers );
             ++bufferIndex )
        {
            rResourceSet.instancePixelConstantBuffers[ bufferIndex ].Release();
        }
    }

    m_currentResourceSetIndex = 0;

    m_bDrawing = false;
}

/// Buffer a wireframe primitive draw call.
///
/// @param[in] primitiveType   Type of primitive to draw.
/// @param[in] pVertices       Vertices to use for drawing.
/// @param[in] vertexCount     Number of vertices used for drawing.
/// @param[in] pIndices        Indices to use for drawing.  If this is null, unindexed rendering will be performed.
/// @param[in] primitiveCount  Number of primitives to draw.
/// @param[in] blendColor      Color with which to blend each vertex color.
/// @param[in] depthMode       Mode in which to handle depth testing and writing.
///
/// @see DrawSolid(), DrawTextured(), DrawPoints()
void BufferedDrawer::DrawWire(
    ERendererPrimitiveType primitiveType,
    const SimpleVertex* pVertices,
    uint32_t vertexCount,
    const uint16_t* pIndices,
    uint32_t primitiveCount,
    Color blendColor,
    EDepthMode depthMode )
{
    HELIUM_ASSERT( static_cast< size_t >( primitiveType ) < static_cast< size_t >( RENDERER_PRIMITIVE_TYPE_MAX ) );
    HELIUM_ASSERT( pVertices );
    HELIUM_ASSERT( vertexCount );
    HELIUM_ASSERT( pIndices || vertexCount == RendererUtil::PrimitiveCountToIndexCount( primitiveType, primitiveCount ) );
    HELIUM_ASSERT( primitiveCount );
    HELIUM_ASSERT( static_cast< size_t >( depthMode ) < static_cast< size_t >( DEPTH_MODE_MAX ) );

    // Cannot add draw calls while rendering.
    HELIUM_ASSERT( !m_bDrawing );

    // Don't buffer any drawing information if we have no renderer.
    if( !Renderer::GetStaticInstance() )
    {
        return;
    }

    uint32_t baseVertexIndex = static_cast< uint32_t >( m_untexturedVertices.GetSize() );
    m_untexturedVertices.AddArray( pVertices, vertexCount );

    uint32_t startIndex;
    SetInvalid( startIndex );
    if( pIndices )
    {
        startIndex = static_cast< uint32_t >( m_untexturedIndices.GetSize() );
        m_untexturedIndices.AddArray(
            pIndices,
            RendererUtil::PrimitiveCountToIndexCount( primitiveType, primitiveCount ) );
    }

    UntexturedDrawCall* pDrawCall = m_wireDrawCalls[ depthMode ].New();
    HELIUM_ASSERT( pDrawCall );
    pDrawCall->primitiveType = primitiveType;
    pDrawCall->baseVertexIndex = baseVertexIndex;
    pDrawCall->vertexCount = vertexCount;
    pDrawCall->startIndex = startIndex;
    pDrawCall->primitiveCount = primitiveCount;
    pDrawCall->blendColor = blendColor;
}

/// Buffer a wireframe primitive draw call.
///
/// @param[in] primitiveType    Type of primitive to draw.
/// @param[in] rTransform       World transform to apply when rendering.
/// @param[in] pVertices        Vertex buffer to use for drawing.  This must contain a packed array of SimpleVertex
///                             vertices.
/// @param[in] pIndices         Indices to use for drawing.  If this is null, unindexed rendering will be performed.
/// @param[in] baseVertexIndex  Index of the first vertex to use for rendering.  Index buffer values will be relative to
///                             this vertex.
/// @param[in] vertexCount      Number of vertices used for rendering.
/// @param[in] startIndex       Index of the first index to use for drawing.
/// @param[in] primitiveCount   Number of primitives to draw.
/// @param[in] blendColor       Color with which to blend each vertex color.
/// @param[in] depthMode        Mode in which to handle depth testing and writing.
///
/// @see DrawSolid(), DrawTextured(), DrawPoints()
void BufferedDrawer::DrawWire(
    ERendererPrimitiveType primitiveType,
    const Simd::Matrix44& rTransform,
    RVertexBuffer* pVertices,
    RIndexBuffer* pIndices,
    uint32_t baseVertexIndex,
    uint32_t vertexCount,
    uint32_t startIndex,
    uint32_t primitiveCount,
    Color blendColor,
    EDepthMode depthMode )
{
    HELIUM_ASSERT( static_cast< size_t >( primitiveType ) < static_cast< size_t >( RENDERER_PRIMITIVE_TYPE_MAX ) );
    HELIUM_ASSERT( pVertices );
    HELIUM_ASSERT( pIndices || vertexCount == RendererUtil::PrimitiveCountToIndexCount( primitiveType, primitiveCount ) );
    HELIUM_ASSERT( vertexCount );
    HELIUM_ASSERT( primitiveCount );
    HELIUM_ASSERT( static_cast< size_t >( depthMode ) < static_cast< size_t >( DEPTH_MODE_MAX ) );

    // Cannot add draw calls while rendering.
    HELIUM_ASSERT( !m_bDrawing );

    // Don't buffer any drawing information if we have no renderer.
    if( !Renderer::GetStaticInstance() )
    {
        return;
    }

    UntexturedBufferDrawCall* pDrawCall = m_wireBufferDrawCalls[ depthMode ].New();
    HELIUM_ASSERT( pDrawCall );
    pDrawCall->primitiveType = primitiveType;
    pDrawCall->baseVertexIndex = baseVertexIndex;
    pDrawCall->vertexCount = vertexCount;
    pDrawCall->startIndex = startIndex;
    pDrawCall->primitiveCount = primitiveCount;
    pDrawCall->blendColor = blendColor;
    pDrawCall->spVertexBuffer = pVertices;
    pDrawCall->spIndexBuffer = pIndices;
    pDrawCall->transform = rTransform;
}

/// Buffer a solid primitive draw call.
///
/// @param[in] primitiveType   Type of primitive to draw.
/// @param[in] pVertices       Vertices to use for drawing.
/// @param[in] vertexCount     Number of vertices used for drawing.
/// @param[in] pIndices        Indices to use for drawing.  If this is null, unindexed rendering will be performed.
/// @param[in] primitiveCount  Number of primitives to draw.
/// @param[in] blendColor      Color with which to blend each vertex color.
/// @param[in] depthMode       Mode in which to handle depth testing and writing.
///
/// @see DrawWire(), DrawTextured(), DrawPoints()
void BufferedDrawer::DrawSolid(
    ERendererPrimitiveType primitiveType,
    const SimpleVertex* pVertices,
    uint32_t vertexCount,
    const uint16_t* pIndices,
    uint32_t primitiveCount,
    Color blendColor,
    EDepthMode depthMode )
{
    HELIUM_ASSERT( static_cast< size_t >( primitiveType ) < static_cast< size_t >( RENDERER_PRIMITIVE_TYPE_MAX ) );
    HELIUM_ASSERT( pVertices );
    HELIUM_ASSERT( vertexCount );
    HELIUM_ASSERT( pIndices || vertexCount == RendererUtil::PrimitiveCountToIndexCount( primitiveType, primitiveCount ) );
    HELIUM_ASSERT( primitiveCount );
    HELIUM_ASSERT( static_cast< size_t >( depthMode ) < static_cast< size_t >( DEPTH_MODE_MAX ) );

    // Cannot add draw calls while rendering.
    HELIUM_ASSERT( !m_bDrawing );

    // Don't buffer any drawing information if we have no renderer.
    if( !Renderer::GetStaticInstance() )
    {
        return;
    }

    uint32_t baseVertexIndex = static_cast< uint32_t >( m_untexturedVertices.GetSize() );
    m_untexturedVertices.AddArray( pVertices, vertexCount );

    uint32_t startIndex;
    SetInvalid( startIndex );
    if( pIndices )
    {
        startIndex = static_cast< uint32_t >( m_untexturedIndices.GetSize() );
        m_untexturedIndices.AddArray(
            pIndices,
            RendererUtil::PrimitiveCountToIndexCount( primitiveType, primitiveCount ) );
    }

    UntexturedDrawCall* pDrawCall = m_solidDrawCalls[ depthMode ].New();
    HELIUM_ASSERT( pDrawCall );
    pDrawCall->primitiveType = primitiveType;
    pDrawCall->baseVertexIndex = baseVertexIndex;
    pDrawCall->vertexCount = vertexCount;
    pDrawCall->startIndex = startIndex;
    pDrawCall->primitiveCount = primitiveCount;
    pDrawCall->blendColor = blendColor;
}

/// Buffer a solid primitive draw call.
///
/// @param[in] primitiveType    Type of primitive to draw.
/// @param[in] rTransform       World transform to apply when rendering.
/// @param[in] pVertices        Vertex buffer to use for drawing.  This must contain a packed array of SimpleVertex
///                             vertices.
/// @param[in] pIndices         Indices to use for drawing.  If this is null, unindexed rendering will be performed.
/// @param[in] baseVertexIndex  Index of the first vertex to use for rendering.  Index buffer values will be relative to
///                             this vertex.
/// @param[in] vertexCount      Number of vertices used for rendering.
/// @param[in] startIndex       Index of the first index to use for drawing.
/// @param[in] primitiveCount   Number of primitives to draw.
/// @param[in] blendColor       Color with which to blend each vertex color.
/// @param[in] depthMode        Mode in which to handle depth testing and writing.
///
/// @see DrawWire(), DrawTextured(), DrawPoints()
void BufferedDrawer::DrawSolid(
    ERendererPrimitiveType primitiveType,
    const Simd::Matrix44& rTransform,
    RVertexBuffer* pVertices,
    RIndexBuffer* pIndices,
    uint32_t baseVertexIndex,
    uint32_t vertexCount,
    uint32_t startIndex,
    uint32_t primitiveCount,
    Color blendColor,
    EDepthMode depthMode )
{
    HELIUM_ASSERT( static_cast< size_t >( primitiveType ) < static_cast< size_t >( RENDERER_PRIMITIVE_TYPE_MAX ) );
    HELIUM_ASSERT( pVertices );
    HELIUM_ASSERT( pIndices || vertexCount == RendererUtil::PrimitiveCountToIndexCount( primitiveType, primitiveCount ) );
    HELIUM_ASSERT( vertexCount );
    HELIUM_ASSERT( primitiveCount );
    HELIUM_ASSERT( static_cast< size_t >( depthMode ) < static_cast< size_t >( DEPTH_MODE_MAX ) );

    // Cannot add draw calls while rendering.
    HELIUM_ASSERT( !m_bDrawing );

    // Don't buffer any drawing information if we have no renderer.
    if( !Renderer::GetStaticInstance() )
    {
        return;
    }

    UntexturedBufferDrawCall* pDrawCall = m_solidBufferDrawCalls[ depthMode ].New();
    HELIUM_ASSERT( pDrawCall );
    pDrawCall->primitiveType = primitiveType;
    pDrawCall->baseVertexIndex = baseVertexIndex;
    pDrawCall->vertexCount = vertexCount;
    pDrawCall->startIndex = startIndex;
    pDrawCall->primitiveCount = primitiveCount;
    pDrawCall->blendColor = blendColor;
    pDrawCall->spVertexBuffer = pVertices;
    pDrawCall->spIndexBuffer = pIndices;
    pDrawCall->transform = rTransform;
}

/// Buffer a textured primitive draw call.
///
/// @param[in] primitiveType   Type of primitive to draw.
/// @param[in] pVertices       Vertices to use for drawing.
/// @param[in] vertexCount     Number of vertices used for drawing.
/// @param[in] pIndices        Indices to use for drawing.  If this is null, unindexed rendering will be performed.
/// @param[in] primitiveCount  Number of primitives to draw.
/// @param[in] pTexture        Texture to apply to the mesh.
/// @param[in] blendColor      Color with which to blend each vertex color.
/// @param[in] depthMode       Mode in which to handle depth testing and writing.
///
/// @see DrawWire(), DrawSolid(), DrawPoints()
void BufferedDrawer::DrawTextured(
    ERendererPrimitiveType primitiveType,
    const SimpleTexturedVertex* pVertices,
    uint32_t vertexCount,
    const uint16_t* pIndices,
    uint32_t primitiveCount,
    RTexture2d* pTexture,
    Color blendColor,
    EDepthMode depthMode )
{
    HELIUM_ASSERT( static_cast< size_t >( primitiveType ) < static_cast< size_t >( RENDERER_PRIMITIVE_TYPE_MAX ) );
    HELIUM_ASSERT( pVertices );
    HELIUM_ASSERT( vertexCount );
    HELIUM_ASSERT( pIndices || vertexCount == RendererUtil::PrimitiveCountToIndexCount( primitiveType, primitiveCount ) );
    HELIUM_ASSERT( primitiveCount );
    HELIUM_ASSERT( pTexture );
    HELIUM_ASSERT( static_cast< size_t >( depthMode ) < static_cast< size_t >( DEPTH_MODE_MAX ) );

    // Cannot add draw calls while rendering.
    HELIUM_ASSERT( !m_bDrawing );

    // Don't buffer any drawing information if we have no renderer.
    if( !Renderer::GetStaticInstance() )
    {
        return;
    }

    uint32_t baseVertexIndex = static_cast< uint32_t >( m_texturedVertices.GetSize() );
    m_texturedVertices.AddArray( pVertices, vertexCount );

    uint32_t startIndex;
    SetInvalid( startIndex );
    if( pIndices )
    {
        startIndex = static_cast< uint32_t >( m_texturedIndices.GetSize() );
        m_texturedIndices.AddArray(
            pIndices,
            RendererUtil::PrimitiveCountToIndexCount( primitiveType, primitiveCount ) );
    }

    TexturedDrawCall* pDrawCall = m_texturedDrawCalls[ depthMode ].New();
    HELIUM_ASSERT( pDrawCall );
    pDrawCall->primitiveType = primitiveType;
    pDrawCall->baseVertexIndex = baseVertexIndex;
    pDrawCall->vertexCount = vertexCount;
    pDrawCall->startIndex = startIndex;
    pDrawCall->primitiveCount = primitiveCount;
    pDrawCall->blendColor = blendColor;
    pDrawCall->spTexture = pTexture;
}

/// Buffer a textured primitive draw call.
///
/// @param[in] primitiveType    Type of primitive to draw.
/// @param[in] rTransform       World transform to apply when rendering.
/// @param[in] pVertices        Vertex buffer to use for drawing.  This must contain a packed array of
///                             SimpleTexturedVertex vertices.
/// @param[in] pIndices         Indices to use for drawing.  If this is null, unindexed rendering will be performed.
/// @param[in] baseVertexIndex  Index of the first vertex to use for rendering.  Index buffer values will be relative to
///                             this vertex.
/// @param[in] vertexCount      Number of vertices used for rendering.
/// @param[in] startIndex       Index of the first index to use for drawing.
/// @param[in] primitiveCount   Number of primitives to draw.
/// @param[in] pTexture         Texture to apply to the mesh.
/// @param[in] blendColor       Color with which to blend each vertex color.
/// @param[in] depthMode        Mode in which to handle depth testing and writing.
///
/// @see DrawWire(), DrawSolid(), DrawPoints()
void BufferedDrawer::DrawTextured(
    ERendererPrimitiveType primitiveType,
    const Simd::Matrix44& rTransform,
    RVertexBuffer* pVertices,
    RIndexBuffer* pIndices,
    uint32_t baseVertexIndex,
    uint32_t vertexCount,
    uint32_t startIndex,
    uint32_t primitiveCount,
    RTexture2d* pTexture,
    Color blendColor,
    EDepthMode depthMode )
{
    HELIUM_ASSERT( static_cast< size_t >( primitiveType ) < static_cast< size_t >( RENDERER_PRIMITIVE_TYPE_MAX ) );
    HELIUM_ASSERT( pVertices );
    HELIUM_ASSERT( pIndices || vertexCount == RendererUtil::PrimitiveCountToIndexCount( primitiveType, primitiveCount ) );
    HELIUM_ASSERT( vertexCount );
    HELIUM_ASSERT( primitiveCount );
    HELIUM_ASSERT( pTexture );
    HELIUM_ASSERT( static_cast< size_t >( depthMode ) < static_cast< size_t >( DEPTH_MODE_MAX ) );

    // Cannot add draw calls while rendering.
    HELIUM_ASSERT( !m_bDrawing );

    // Don't buffer any drawing information if we have no renderer.
    if( !Renderer::GetStaticInstance() )
    {
        return;
    }

    TexturedBufferDrawCall* pDrawCall = m_texturedBufferDrawCalls[ depthMode ].New();
    HELIUM_ASSERT( pDrawCall );
    pDrawCall->primitiveType = primitiveType;
    pDrawCall->baseVertexIndex = baseVertexIndex;
    pDrawCall->vertexCount = vertexCount;
    pDrawCall->startIndex = startIndex;
    pDrawCall->primitiveCount = primitiveCount;
    pDrawCall->blendColor = blendColor;
    pDrawCall->spTexture = pTexture;
    pDrawCall->spVertexBuffer = pVertices;
    pDrawCall->spIndexBuffer = pIndices;
    pDrawCall->transform = rTransform;
}

/// Buffer a point list draw call using points larger than a pixel.
///
/// @param[in] pVertices   Vertices to use for drawing.
/// @param[in] pointCount  Number of points to draw.
/// @param[in] blendColor  Color with which to blend each vertex color.
/// @param[in] depthMode   Mode in which to handle depth testing and writing.
///
/// @see DrawWire(), DrawSolid(), DrawTextured()
void BufferedDrawer::DrawPoints(
    const SimpleVertex* pVertices,
    uint32_t pointCount,
    Color blendColor,
    EDepthMode depthMode )
{
    HELIUM_ASSERT( pVertices );
    HELIUM_ASSERT( pointCount );
    HELIUM_ASSERT( static_cast< size_t >( depthMode ) < static_cast< size_t >( DEPTH_MODE_MAX ) );

    // Cannot add draw calls while rendering.
    HELIUM_ASSERT( !m_bDrawing );

    // Don't buffer any drawing information if we have no renderer.
    if( !Renderer::GetStaticInstance() )
    {
        return;
    }

    uint32_t baseVertexIndex = static_cast< uint32_t >( m_untexturedVertices.GetSize() );
    m_untexturedVertices.AddArray( pVertices, pointCount );

    UntexturedDrawCall* pDrawCall = m_pointDrawCalls[ depthMode ].New();
    HELIUM_ASSERT( pDrawCall );
    pDrawCall->primitiveType = RENDERER_PRIMITIVE_TYPE_POINT_LIST;
    pDrawCall->baseVertexIndex = baseVertexIndex;
    pDrawCall->vertexCount = pointCount;
    SetInvalid( pDrawCall->startIndex );
    pDrawCall->primitiveCount = pointCount;
    pDrawCall->blendColor = blendColor;
}

/// Buffer a point list draw call using points larger than a pixel.
///
/// @param[in] rTransform       World transform to apply when rendering.
/// @param[in] pVertices        Vertex buffer to use for drawing.  This must contain a packed array of SimpleVertex
///                             vertices.
/// @param[in] baseVertexIndex  Index of the first vertex to use for rendering.
/// @param[in] pointCount       Number of points to draw.
/// @param[in] blendColor       Color with which to blend each vertex color.
/// @param[in] depthMode        Mode in which to handle depth testing and writing.
///
/// @see DrawWire(), DrawSolid(), DrawTextured()
void BufferedDrawer::DrawPoints(
    const Simd::Matrix44& rTransform,
    RVertexBuffer* pVertices,
    uint32_t baseVertexIndex,
    uint32_t pointCount,
    Color blendColor,
    EDepthMode depthMode )
{
    HELIUM_ASSERT( pVertices );
    HELIUM_ASSERT( pointCount );
    HELIUM_ASSERT( static_cast< size_t >( depthMode ) < static_cast< size_t >( DEPTH_MODE_MAX ) );

    // Cannot add draw calls while rendering.
    HELIUM_ASSERT( !m_bDrawing );

    // Don't buffer any drawing information if we have no renderer.
    if( !Renderer::GetStaticInstance() )
    {
        return;
    }

    UntexturedBufferDrawCall* pDrawCall = m_pointBufferDrawCalls[ depthMode ].New();
    HELIUM_ASSERT( pDrawCall );
    pDrawCall->primitiveType = RENDERER_PRIMITIVE_TYPE_POINT_LIST;
    pDrawCall->baseVertexIndex = baseVertexIndex;
    pDrawCall->vertexCount = pointCount;
    SetInvalid( pDrawCall->startIndex );
    pDrawCall->primitiveCount = pointCount;
    pDrawCall->blendColor = blendColor;
    pDrawCall->spVertexBuffer = pVertices;
    pDrawCall->spIndexBuffer = NULL;
    pDrawCall->transform = rTransform;
}

/// Draw text in world space at a specific transform.
///
/// @param[in] rTransform  World transform at which to start the text.
/// @param[in] rText       Text to draw.
/// @param[in] color       Color to blend with the text.
/// @param[in] size        Identifier of the font size to use.
/// @param[in] depthMode   Mode in which to handle depth testing and writing.
///
/// @see DrawScreenText()
void BufferedDrawer::DrawWorldText(
    const Simd::Matrix44& rTransform,
    const String& rText,
    Color color,
    RenderResourceManager::EDebugFontSize size,
    EDepthMode depthMode )
{
    HELIUM_ASSERT(
        static_cast< size_t >( size ) < static_cast< size_t >( RenderResourceManager::DEBUG_FONT_SIZE_MAX ) );
    HELIUM_ASSERT( static_cast< size_t >( depthMode ) < static_cast< size_t >( DEPTH_MODE_MAX ) );

    // Cannot add draw calls while rendering.
    HELIUM_ASSERT( !m_bDrawing );

    // Don't buffer any drawing information if we have no renderer.
    if( !Renderer::GetStaticInstance() )
    {
        return;
    }

    // Get the font to use for rendering.
    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();
    Font* pFont = rRenderResourceManager.GetDebugFont( size );
    if( !pFont )
    {
        return;
    }

    // Render the text.
    WorldSpaceTextGlyphHandler glyphHandler( this, pFont, color, depthMode, rTransform );
    pFont->ProcessText( rText, glyphHandler );
}

/// Draw text in screen space at a specific transform.
///
/// @param[in] x      X-coordinate of the screen pixel at which to begin drawing the text.
/// @param[in] y      Y-coordinate of the screen pixel at which to begin drawing the text.
/// @param[in] rText  Text to draw.
/// @param[in] color  Color to blend with the text.
/// @param[in] size   Identifier of the font size to use.
///
/// @see DrawWorldText()
void BufferedDrawer::DrawScreenText(
    int32_t x,
    int32_t y,
    const String& rText,
    Color color,
    RenderResourceManager::EDebugFontSize size )
{
    HELIUM_ASSERT(
        static_cast< size_t >( size ) < static_cast< size_t >( RenderResourceManager::DEBUG_FONT_SIZE_MAX ) );

    // Cannot add draw calls while rendering.
    HELIUM_ASSERT( !m_bDrawing );

    // Don't buffer any drawing information if we have no renderer.
    if( !Renderer::GetStaticInstance() )
    {
        return;
    }

    // Get the font to use for rendering.
    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();
    Font* pFont = rRenderResourceManager.GetDebugFont( size );
    if( !pFont )
    {
        return;
    }

    // Store the information needed for drawing the text later.
    ScreenSpaceTextGlyphHandler glyphHandler( this, pFont, x, y, color, size );
    pFont->ProcessText( rText, glyphHandler );
}

/// Push buffered draw command data into vertex and index buffers for rendering.
///
/// This must be called prior to calling DrawWorldElements() or DrawScreenElements().  EndDrawing() should be called
/// when rendering is complete.  No new draw calls can be added between a BeginDrawing() and EndDrawing() call pair.
///
/// @see EndDrawing(), DrawWorldElements(), DrawScreenElements()
void BufferedDrawer::BeginDrawing()
{
    // Flag that we have begun drawing.
    HELIUM_ASSERT( !m_bDrawing );
    m_bDrawing = true;

    // If a renderer is not initialized, we don't need to do anything.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( !pRenderer )
    {
        HELIUM_ASSERT( m_untexturedVertices.IsEmpty() );
        HELIUM_ASSERT( m_untexturedIndices.IsEmpty() );
        HELIUM_ASSERT( m_texturedVertices.IsEmpty() );
        HELIUM_ASSERT( m_texturedIndices.IsEmpty() );
        HELIUM_ASSERT( m_screenTextGlyphIndices.IsEmpty() );

        return;
    }

    // Prepare the vertex and index buffers with the buffered data.
    ResourceSet& rResourceSet = m_resourceSets[ m_currentResourceSetIndex ];

    uint_fast32_t untexturedVertexCount = static_cast< uint_fast32_t >( m_untexturedVertices.GetSize() );
    uint_fast32_t untexturedIndexCount = static_cast< uint_fast32_t >( m_untexturedIndices.GetSize() );
    uint_fast32_t texturedVertexCount = static_cast< uint_fast32_t >( m_texturedVertices.GetSize() );
    uint_fast32_t texturedIndexCount = static_cast< uint_fast32_t >( m_texturedIndices.GetSize() );

    uint_fast32_t screenTextGlyphIndexCount = static_cast< uint_fast32_t >( m_screenTextGlyphIndices.GetSize() );
    uint_fast32_t screenTextVertexCount = screenTextGlyphIndexCount * 4;

    if( untexturedVertexCount > rResourceSet.untexturedVertexBufferSize )
    {
        rResourceSet.spUntexturedVertexBuffer.Release();
        rResourceSet.spUntexturedVertexBuffer = pRenderer->CreateVertexBuffer(
            untexturedVertexCount * sizeof( SimpleVertex ),
            RENDERER_BUFFER_USAGE_DYNAMIC );
        if( !rResourceSet.spUntexturedVertexBuffer )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "Failed to create vertex buffer for untextured debug drawing of %" ) TPRIuFAST32
                  TXT( " vertices.\n" ) ),
                untexturedVertexCount );

            rResourceSet.untexturedVertexBufferSize = 0;
        }
        else
        {
            rResourceSet.untexturedVertexBufferSize = static_cast< uint32_t >( untexturedVertexCount );
        }
    }

    if( untexturedIndexCount > rResourceSet.untexturedIndexBufferSize )
    {
        rResourceSet.spUntexturedIndexBuffer.Release();
        rResourceSet.spUntexturedIndexBuffer = pRenderer->CreateIndexBuffer(
            untexturedIndexCount * sizeof( uint16_t ),
            RENDERER_BUFFER_USAGE_DYNAMIC,
            RENDERER_INDEX_FORMAT_UINT16 );
        if( !rResourceSet.spUntexturedIndexBuffer )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "Failed to create index buffer for untextured debug drawing of %" ) TPRIuFAST32
                  TXT( " indices.\n" ) ),
                untexturedIndexCount );

            rResourceSet.untexturedIndexBufferSize = 0;
        }
        else
        {
            rResourceSet.untexturedIndexBufferSize = static_cast< uint32_t >( untexturedIndexCount );
        }
    }

    if( texturedVertexCount > rResourceSet.texturedVertexBufferSize )
    {
        rResourceSet.spTexturedVertexBuffer.Release();
        rResourceSet.spTexturedVertexBuffer = pRenderer->CreateVertexBuffer(
            texturedVertexCount * sizeof( SimpleTexturedVertex ),
            RENDERER_BUFFER_USAGE_DYNAMIC );
        if( !rResourceSet.spTexturedVertexBuffer )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "Failed to create vertex buffer for textured debug drawing of %" ) TPRIuFAST32
                  TXT( " vertices.\n" ) ),
                texturedVertexCount );

            rResourceSet.texturedVertexBufferSize = 0;
        }
        else
        {
            rResourceSet.texturedVertexBufferSize = static_cast< uint32_t >( texturedVertexCount );
        }
    }

    if( texturedIndexCount > rResourceSet.texturedIndexBufferSize )
    {
        rResourceSet.spTexturedIndexBuffer.Release();
        rResourceSet.spTexturedIndexBuffer = pRenderer->CreateIndexBuffer(
            texturedIndexCount * sizeof( uint16_t ),
            RENDERER_BUFFER_USAGE_DYNAMIC,
            RENDERER_INDEX_FORMAT_UINT16 );
        if( !rResourceSet.spTexturedIndexBuffer )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "Failed to create index buffer for textured debug drawing of %" ) TPRIuFAST32
                  TXT( " indices.\n" ) ),
                texturedIndexCount );

            rResourceSet.texturedIndexBufferSize = 0;
        }
        else
        {
            rResourceSet.texturedIndexBufferSize = static_cast< uint32_t >( texturedIndexCount );
        }
    }

    if( screenTextVertexCount > rResourceSet.screenSpaceTextVertexBufferSize )
    {
        rResourceSet.spScreenSpaceTextVertexBuffer.Release();
        rResourceSet.spScreenSpaceTextVertexBuffer = pRenderer->CreateVertexBuffer(
            screenTextVertexCount * sizeof( ScreenVertex ),
            RENDERER_BUFFER_USAGE_DYNAMIC );
        if( !rResourceSet.spScreenSpaceTextVertexBuffer )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "Failed to create vertex buffer for screen-space text drawing of %" ) TPRIuFAST32
                  TXT( " vertices.\n" ) ),
                screenTextVertexCount );

            rResourceSet.screenSpaceTextVertexBufferSize = 0;
        }
        else
        {
            rResourceSet.screenSpaceTextVertexBufferSize = static_cast< uint32_t >( screenTextVertexCount );
        }
    }

    // Fill the vertex and index buffers for rendering.
    if( untexturedVertexCount && untexturedIndexCount &&
        rResourceSet.spUntexturedVertexBuffer && rResourceSet.spUntexturedIndexBuffer )
    {
        void* pMappedVertexBuffer = rResourceSet.spUntexturedVertexBuffer->Map( RENDERER_BUFFER_MAP_HINT_DISCARD );
        HELIUM_ASSERT( pMappedVertexBuffer );
        MemoryCopy(
            pMappedVertexBuffer,
            m_untexturedVertices.GetData(),
            untexturedVertexCount * sizeof( SimpleVertex ) );
        rResourceSet.spUntexturedVertexBuffer->Unmap();

        void* pMappedIndexBuffer = rResourceSet.spUntexturedIndexBuffer->Map( RENDERER_BUFFER_MAP_HINT_DISCARD );
        HELIUM_ASSERT( pMappedIndexBuffer );
        MemoryCopy(
            pMappedIndexBuffer,
            m_untexturedIndices.GetData(),
            untexturedIndexCount * sizeof( uint16_t ) );
        rResourceSet.spUntexturedIndexBuffer->Unmap();
    }

    if( texturedVertexCount && texturedIndexCount &&
        rResourceSet.spTexturedVertexBuffer && rResourceSet.spTexturedIndexBuffer )
    {
        void* pMappedVertexBuffer = rResourceSet.spTexturedVertexBuffer->Map( RENDERER_BUFFER_MAP_HINT_DISCARD );
        HELIUM_ASSERT( pMappedVertexBuffer );
        MemoryCopy(
            pMappedVertexBuffer,
            m_texturedVertices.GetData(),
            texturedVertexCount * sizeof( SimpleTexturedVertex ) );
        rResourceSet.spTexturedVertexBuffer->Unmap();

        void* pMappedIndexBuffer = rResourceSet.spTexturedIndexBuffer->Map( RENDERER_BUFFER_MAP_HINT_DISCARD );
        HELIUM_ASSERT( pMappedIndexBuffer );
        MemoryCopy(
            pMappedIndexBuffer,
            m_texturedIndices.GetData(),
            texturedIndexCount * sizeof( uint16_t ) );
        rResourceSet.spTexturedIndexBuffer->Unmap();
    }

    if( screenTextVertexCount && rResourceSet.spScreenSpaceTextVertexBuffer )
    {
        ScreenVertex* pScreenVertices = static_cast< ScreenVertex* >( rResourceSet.spScreenSpaceTextVertexBuffer->Map(
            RENDERER_BUFFER_MAP_HINT_DISCARD ) );
        HELIUM_ASSERT( pScreenVertices );

        uint32_t* pGlyphIndex = m_screenTextGlyphIndices.GetData();

        size_t textDrawCount = m_screenTextDrawCalls.GetSize();
        for( size_t drawIndex = 0; drawIndex < textDrawCount; ++drawIndex )
        {
            const TextDrawCall& rDrawCall = m_screenTextDrawCalls[ drawIndex ];
            uint_fast32_t glyphCount = rDrawCall.glyphCount;

            RenderResourceManager& rResourceManager = RenderResourceManager::GetStaticInstance();
            Font* pFont = rResourceManager.GetDebugFont( rDrawCall.size );
            if( pFont )
            {
                float32_t x = static_cast< float32_t >( rDrawCall.x );
                float32_t y = static_cast< float32_t >( rDrawCall.y );
                Color color = rDrawCall.color;

                float32_t inverseTextureWidth = 1.0f / static_cast< float32_t >( pFont->GetTextureSheetWidth() );
                float32_t inverseTextureHeight = 1.0f / static_cast< float32_t >( pFont->GetTextureSheetHeight() );

                uint32_t fontCharacterCount = pFont->GetCharacterCount();

                for( uint_fast32_t glyphIndexOffset = 0; glyphIndexOffset < glyphCount; ++glyphIndexOffset )
                {
                    uint32_t glyphIndex = *pGlyphIndex;
                    ++pGlyphIndex;

                    if( glyphIndex >= fontCharacterCount )
                    {
                        MemoryZero( pScreenVertices, sizeof( *pScreenVertices ) * 4 );
                        pScreenVertices += 4;

                        continue;
                    }

                    const Font::Character& rCharacter = pFont->GetCharacter( glyphIndex );

                    float32_t imageWidthFloat = static_cast< float32_t >( rCharacter.imageWidth );
                    float32_t imageHeightFloat = static_cast< float32_t >( rCharacter.imageHeight );

                    float32_t cornerMinX = Floor( x + 0.5f ) + static_cast< float32_t >( rCharacter.bearingX >> 6 );
                    float32_t cornerMinY = y - static_cast< float32_t >( rCharacter.bearingY >> 6 );
                    float32_t cornerMaxX = cornerMinX + imageWidthFloat;
                    float32_t cornerMaxY = cornerMinY + imageHeightFloat;

                    Float32 texCoordMinX32, texCoordMinY32, texCoordMaxX32, texCoordMaxY32;
                    texCoordMinX32.value = static_cast< float32_t >( rCharacter.imageX );
                    texCoordMinY32.value = static_cast< float32_t >( rCharacter.imageY );
                    texCoordMaxX32.value = texCoordMinX32.value + imageWidthFloat;
                    texCoordMaxY32.value = texCoordMinY32.value + imageHeightFloat;

                    texCoordMinX32.value *= inverseTextureWidth;
                    texCoordMinY32.value *= inverseTextureHeight;
                    texCoordMaxX32.value *= inverseTextureWidth;
                    texCoordMaxY32.value *= inverseTextureHeight;

                    Float16 texCoordMinX = Float32To16( texCoordMinX32 );
                    Float16 texCoordMinY = Float32To16( texCoordMinY32 );
                    Float16 texCoordMaxX = Float32To16( texCoordMaxX32 );
                    Float16 texCoordMaxY = Float32To16( texCoordMaxY32 );

                    pScreenVertices->position[ 0 ] = cornerMinX;
                    pScreenVertices->position[ 1 ] = cornerMinY;
                    pScreenVertices->color[ 0 ] = color.GetR();
                    pScreenVertices->color[ 1 ] = color.GetG();
                    pScreenVertices->color[ 2 ] = color.GetB();
                    pScreenVertices->color[ 3 ] = color.GetA();
                    pScreenVertices->texCoords[ 0 ] = texCoordMinX;
                    pScreenVertices->texCoords[ 1 ] = texCoordMinY;
                    ++pScreenVertices;

                    pScreenVertices->position[ 0 ] = cornerMaxX;
                    pScreenVertices->position[ 1 ] = cornerMinY;
                    pScreenVertices->color[ 0 ] = color.GetR();
                    pScreenVertices->color[ 1 ] = color.GetG();
                    pScreenVertices->color[ 2 ] = color.GetB();
                    pScreenVertices->color[ 3 ] = color.GetA();
                    pScreenVertices->texCoords[ 0 ] = texCoordMaxX;
                    pScreenVertices->texCoords[ 1 ] = texCoordMinY;
                    ++pScreenVertices;

                    pScreenVertices->position[ 0 ] = cornerMaxX;
                    pScreenVertices->position[ 1 ] = cornerMaxY;
                    pScreenVertices->color[ 0 ] = color.GetR();
                    pScreenVertices->color[ 1 ] = color.GetG();
                    pScreenVertices->color[ 2 ] = color.GetB();
                    pScreenVertices->color[ 3 ] = color.GetA();
                    pScreenVertices->texCoords[ 0 ] = texCoordMaxX;
                    pScreenVertices->texCoords[ 1 ] = texCoordMaxY;
                    ++pScreenVertices;

                    pScreenVertices->position[ 0 ] = cornerMinX;
                    pScreenVertices->position[ 1 ] = cornerMaxY;
                    pScreenVertices->color[ 0 ] = color.GetR();
                    pScreenVertices->color[ 1 ] = color.GetG();
                    pScreenVertices->color[ 2 ] = color.GetB();
                    pScreenVertices->color[ 3 ] = color.GetA();
                    pScreenVertices->texCoords[ 0 ] = texCoordMinX;
                    pScreenVertices->texCoords[ 1 ] = texCoordMaxY;
                    ++pScreenVertices;

                    x += Font::Fixed26x6ToFloat32( rCharacter.advance );
                }
            }
            else
            {
                pGlyphIndex += glyphCount;

                size_t vertexSkipCount = glyphCount * 4;
                MemoryZero( pScreenVertices, vertexSkipCount * sizeof( *pScreenVertices ) );
                pScreenVertices += vertexSkipCount;
            }
        }

        rResourceSet.spScreenSpaceTextVertexBuffer->Unmap();
    }

    // Clear the buffered vertex and index data, as it is no longer needed.
    m_untexturedVertices.RemoveAll();
    m_texturedVertices.RemoveAll();
    m_untexturedIndices.RemoveAll();
    m_texturedIndices.RemoveAll();

    // Reset per-instance shader constant management data.
    SetInvalid( rResourceSet.instanceVertexConstantBufferIndex );
    SetInvalid( rResourceSet.instancePixelConstantBufferIndex );
}

/// Finish issuing draw commands and reset buffered data for the next set of draw calls.
///
/// This must be called when all drawing has completed after an earlier BeginDrawing() call.
///
/// @see BeginDrawing(), DrawWorldElements(), DrawScreenElements()
void BufferedDrawer::EndDrawing()
{
    // Flag that we are no longer drawing.
    HELIUM_ASSERT( m_bDrawing );
    m_bDrawing = false;

    // If a renderer is not initialized, we don't need to do anything.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( !pRenderer )
    {
        HELIUM_ASSERT( m_untexturedVertices.IsEmpty() );
        HELIUM_ASSERT( m_untexturedIndices.IsEmpty() );
        HELIUM_ASSERT( m_texturedVertices.IsEmpty() );
        HELIUM_ASSERT( m_texturedIndices.IsEmpty() );
        HELIUM_ASSERT( m_screenTextGlyphIndices.IsEmpty() );

        return;
    }

    // Clear all buffered draw call data.
    m_screenTextGlyphIndices.RemoveAll();
    m_screenTextDrawCalls.RemoveAll();

    for( size_t depthModeIndex = 0; depthModeIndex < static_cast< size_t >( DEPTH_MODE_MAX ); ++depthModeIndex )
    {
        m_worldTextDrawCalls[ depthModeIndex ].RemoveAll();

        m_pointBufferDrawCalls[ depthModeIndex ].RemoveAll();
        m_texturedBufferDrawCalls[ depthModeIndex ].RemoveAll();
        m_solidBufferDrawCalls[ depthModeIndex ].RemoveAll();
        m_wireBufferDrawCalls[ depthModeIndex ].RemoveAll();

        m_pointDrawCalls[ depthModeIndex ].RemoveAll();
        m_texturedDrawCalls[ depthModeIndex ].RemoveAll();
        m_solidDrawCalls[ depthModeIndex ].RemoveAll();
        m_wireDrawCalls[ depthModeIndex ].RemoveAll();
    }

    // Release all fences used to block the usage lifetime of various instance-specific shader constant buffers.
    for( size_t fenceIndex = 0; fenceIndex < HELIUM_ARRAY_COUNT( m_instanceVertexConstantFences ); ++fenceIndex )
    {
        m_instanceVertexConstantFences[ fenceIndex ].Release();
    }

    for( size_t fenceIndex = 0; fenceIndex < HELIUM_ARRAY_COUNT( m_instancePixelConstantFences ); ++fenceIndex )
    {
        m_instancePixelConstantFences[ fenceIndex ].Release();
    }

    // Swap rendering resources for the next set of buffered draw calls.
    m_currentResourceSetIndex = ( m_currentResourceSetIndex + 1 ) % HELIUM_ARRAY_COUNT( m_resourceSets );
}

/// Issue draw commands for buffered development-mode draw calls in world space.
///
/// BeginDrawing() must be called before issuing calls to this function.  This function can be called multiple times
/// between a BeginDrawing() and EndDrawing() pair.
///
/// Special care should be taken with regards to the following:
/// - Vertex and pixel shader constants will be set during rendering.
/// - The rasterizer, blend, and depth-stencil states may be altered when this function returns.
///
/// @param[in] rInverseViewProjection  Combined inverse view and projection matrix.
///
/// @see BeginDrawing(), EndDrawing(), DrawScreenElements()
void BufferedDrawer::DrawWorldElements( const Simd::Matrix44& rInverseViewProjection )
{
    HELIUM_ASSERT( m_bDrawing );

    // If a renderer is not initialized, we don't need to do anything.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( !pRenderer )
    {
        HELIUM_ASSERT( m_untexturedVertices.IsEmpty() );
        HELIUM_ASSERT( m_untexturedIndices.IsEmpty() );
        HELIUM_ASSERT( m_texturedVertices.IsEmpty() );
        HELIUM_ASSERT( m_texturedIndices.IsEmpty() );

        return;
    }

    // Get the shaders to use for debug drawing.
    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();

    ShaderVariant* pVertexShaderVariant = rRenderResourceManager.GetSimpleWorldSpaceVertexShader();
    if( !pVertexShaderVariant )
    {
        return;
    }

    ShaderVariant* pPixelShaderVariant = rRenderResourceManager.GetSimpleWorldSpacePixelShader();
    if( !pPixelShaderVariant )
    {
        return;
    }

    WorldElementResources worldResources;
    worldResources.inverseViewProjection = rInverseViewProjection;

    Shader* pShader = Reflect::AssertCast< Shader >( pVertexShaderVariant->GetOwner() );
    HELIUM_ASSERT( pShader );

    const Shader::Options& rSystemOptions = pShader->GetSystemOptions();
    size_t optionSetIndex;
    RShader* pShaderResource;

    static const Shader::SelectPair untexturedSelectOptions[] =
    {
        { Name( TXT( "TEXTURING" ) ), Name( TXT( "NONE" ) ) },
    };

    optionSetIndex = rSystemOptions.GetOptionSetIndex(
        RShader::TYPE_VERTEX,
        NULL,
        0,
        untexturedSelectOptions,
        HELIUM_ARRAY_COUNT( untexturedSelectOptions ) );
    pShaderResource = pVertexShaderVariant->GetRenderResource( optionSetIndex );
    HELIUM_ASSERT( !pShaderResource || pShaderResource->GetType() == RShader::TYPE_VERTEX );
    worldResources.spUntexturedVertexShader = static_cast< RVertexShader* >( pShaderResource );

    optionSetIndex = rSystemOptions.GetOptionSetIndex(
        RShader::TYPE_PIXEL,
        NULL,
        0,
        untexturedSelectOptions,
        HELIUM_ARRAY_COUNT( untexturedSelectOptions ) );
    pShaderResource = pPixelShaderVariant->GetRenderResource( optionSetIndex );
    HELIUM_ASSERT( !pShaderResource || pShaderResource->GetType() == RShader::TYPE_PIXEL );
    worldResources.spUntexturedPixelShader = static_cast< RPixelShader* >( pShaderResource );

    static const Shader::SelectPair untexturedPointSelectOptions[] =
    {
        { Name( TXT( "TEXTURING" ) ), Name( TXT( "NONE" ) ) },
        { Name( TXT( "POINT_SPRITE" ) ), Name( TXT( "1" ) ) },
    };

    optionSetIndex = rSystemOptions.GetOptionSetIndex(
        RShader::TYPE_VERTEX,
        NULL,
        0,
        untexturedPointSelectOptions,
        HELIUM_ARRAY_COUNT( untexturedPointSelectOptions ) );
    pShaderResource = pVertexShaderVariant->GetRenderResource( optionSetIndex );
    HELIUM_ASSERT( !pShaderResource || pShaderResource->GetType() == RShader::TYPE_VERTEX );
    worldResources.spUntexturedPointsVertexShader = static_cast< RVertexShader* >( pShaderResource );

    static const Shader::SelectPair textureBlendSelectOptions[] =
    {
        { Name( TXT( "TEXTURING" ) ), Name( TXT( "TEXTURING_BLEND" ) ) },
    };

    optionSetIndex = rSystemOptions.GetOptionSetIndex(
        RShader::TYPE_VERTEX,
        NULL,
        0,
        textureBlendSelectOptions,
        HELIUM_ARRAY_COUNT( textureBlendSelectOptions ) );
    pShaderResource = pVertexShaderVariant->GetRenderResource( optionSetIndex );
    HELIUM_ASSERT( !pShaderResource || pShaderResource->GetType() == RShader::TYPE_VERTEX );
    worldResources.spTextureBlendVertexShader = static_cast< RVertexShader* >( pShaderResource );

    optionSetIndex = rSystemOptions.GetOptionSetIndex(
        RShader::TYPE_PIXEL,
        NULL,
        0,
        textureBlendSelectOptions,
        HELIUM_ARRAY_COUNT( textureBlendSelectOptions ) );
    pShaderResource = pPixelShaderVariant->GetRenderResource( optionSetIndex );
    HELIUM_ASSERT( !pShaderResource || pShaderResource->GetType() == RShader::TYPE_PIXEL );
    worldResources.spTextureBlendPixelShader = static_cast< RPixelShader* >( pShaderResource );

    static const Shader::SelectPair textureAlphaSelectOptions[] =
    {
        { Name( TXT( "TEXTURING" ) ), Name( TXT( "TEXTURING_ALPHA" ) ) },
    };

    optionSetIndex = rSystemOptions.GetOptionSetIndex(
        RShader::TYPE_VERTEX,
        NULL,
        0,
        textureAlphaSelectOptions,
        HELIUM_ARRAY_COUNT( textureAlphaSelectOptions ) );
    pShaderResource = pVertexShaderVariant->GetRenderResource( optionSetIndex );
    HELIUM_ASSERT( !pShaderResource || pShaderResource->GetType() == RShader::TYPE_VERTEX );
    worldResources.spTextureAlphaVertexShader = static_cast< RVertexShader* >( pShaderResource );

    optionSetIndex = rSystemOptions.GetOptionSetIndex(
        RShader::TYPE_PIXEL,
        NULL,
        0,
        textureAlphaSelectOptions,
        HELIUM_ARRAY_COUNT( textureAlphaSelectOptions ) );
    pShaderResource = pPixelShaderVariant->GetRenderResource( optionSetIndex );
    HELIUM_ASSERT( !pShaderResource || pShaderResource->GetType() == RShader::TYPE_PIXEL );
    worldResources.spTextureAlphaPixelShader = static_cast< RPixelShader* >( pShaderResource );

    // Get the vertex description resources for the untextured and textured vertex types.
    worldResources.spSimpleVertexDescription = rRenderResourceManager.GetSimpleVertexDescription();
    HELIUM_ASSERT( worldResources.spSimpleVertexDescription );

    worldResources.spSimpleTexturedVertexDescription = rRenderResourceManager.GetSimpleTexturedVertexDescription();
    HELIUM_ASSERT( worldResources.spSimpleTexturedVertexDescription );

    worldResources.spCommandProxy = pRenderer->GetImmediateCommandProxy();
    HELIUM_ASSERT( worldResources.spCommandProxy );

    StateCache stateCache( worldResources.spCommandProxy );
    worldResources.pStateCache = &stateCache;

    // Draw depth enabled data first.
    RDepthStencilState* pDepthStencilState = rRenderResourceManager.GetDepthStencilState(
        RenderResourceManager::DEPTH_STENCIL_STATE_DEFAULT );
    HELIUM_ASSERT( pDepthStencilState );
    worldResources.spCommandProxy->SetDepthStencilState( pDepthStencilState, 0 );

    DrawDepthModeWorldElements( worldResources, DEPTH_MODE_ENABLED );

    // Draw depth disabled data.
    pDepthStencilState = rRenderResourceManager.GetDepthStencilState( RenderResourceManager::DEPTH_STENCIL_STATE_NONE );
    HELIUM_ASSERT( pDepthStencilState );
    worldResources.spCommandProxy->SetDepthStencilState( pDepthStencilState, 0 );

    DrawDepthModeWorldElements( worldResources, DEPTH_MODE_DISABLED );

    // Unset resources.
    stateCache.SetVertexBuffer( NULL, 0 );
    stateCache.SetIndexBuffer( NULL );
    stateCache.SetVertexShader( NULL );
    stateCache.SetPixelShader( NULL );
    stateCache.SetVertexInputLayout( NULL );
    stateCache.SetPixelConstantBuffer( NULL );
    stateCache.SetTexture( NULL );
}

/// Issue draw commands for buffered development-mode draw calls in screen space.
///
/// BeginDrawing() must be called before issuing calls to this function.  This function can be called multiple times
/// between a BeginDrawing() and EndDrawing() pair.
///
/// Special care should be taken with regards to the following:
/// - This function expects the proper global shader constant data (screen-space pixel coordinate conversion values) to
///   be already set in vertex constant buffer 0.
/// - The default rasterizer state should already be set.
/// - The translucent blend state should already be set.
///
/// @see BeginDrawing(), EndDrawing(), DrawWorldElements()
void BufferedDrawer::DrawScreenElements()
{
    HELIUM_ASSERT( m_bDrawing );

    // If a renderer is not initialized, we don't need to do anything.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( !pRenderer )
    {
        HELIUM_ASSERT( m_untexturedVertices.IsEmpty() );
        HELIUM_ASSERT( m_untexturedIndices.IsEmpty() );
        HELIUM_ASSERT( m_texturedVertices.IsEmpty() );
        HELIUM_ASSERT( m_texturedIndices.IsEmpty() );

        return;
    }

    // Make sure we have text to render.
    size_t textDrawCount = m_screenTextDrawCalls.GetSize();
    if( textDrawCount == 0 )
    {
        return;
    }

    RVertexBuffer* pScreenSpaceTextVertexBuffer =
        m_resourceSets[ m_currentResourceSetIndex ].spScreenSpaceTextVertexBuffer;
    if( !pScreenSpaceTextVertexBuffer )
    {
        return;
    }

    // Get the shaders to use for debug drawing.
    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();

    ShaderVariant* pVertexShaderVariant = rRenderResourceManager.GetScreenTextVertexShader();
    if( !pVertexShaderVariant )
    {
        return;
    }

    ShaderVariant* pPixelShaderVariant = rRenderResourceManager.GetScreenTextPixelShader();
    if( !pPixelShaderVariant )
    {
        return;
    }

    RVertexDescriptionPtr spScreenVertexDescription = rRenderResourceManager.GetScreenVertexDescription();
    if( !spScreenVertexDescription )
    {
        return;
    }

    RShader* pShaderResource;

    pShaderResource = pVertexShaderVariant->GetRenderResource( 0 );
    HELIUM_ASSERT( !pShaderResource || pShaderResource->GetType() == RShader::TYPE_VERTEX );
    RVertexShaderPtr spScreenTextVertexShader = static_cast< RVertexShader* >( pShaderResource );

    pShaderResource = pPixelShaderVariant->GetRenderResource( 0 );
    HELIUM_ASSERT( !pShaderResource || pShaderResource->GetType() == RShader::TYPE_PIXEL );
    RPixelShaderPtr spScreenTextPixelShader = static_cast< RPixelShader* >( pShaderResource );

    // Draw each block of text.
    RRenderCommandProxyPtr spCommandProxy = pRenderer->GetImmediateCommandProxy();

    spCommandProxy->SetVertexShader( spScreenTextVertexShader );
    spCommandProxy->SetPixelShader( spScreenTextPixelShader );

    uint32_t vertexStride = static_cast< uint32_t >( sizeof( ScreenVertex ) );
    uint32_t vertexOffset = 0;
    spCommandProxy->SetVertexBuffers( 0, 1, &pScreenSpaceTextVertexBuffer, &vertexStride, &vertexOffset );
    spCommandProxy->SetIndexBuffer( m_spScreenSpaceTextIndexBuffer );

    spScreenTextVertexShader->CacheDescription( pRenderer, spScreenVertexDescription );
    RVertexInputLayout* pVertexInputLayout = spScreenTextVertexShader->GetCachedInputLayout();
    HELIUM_ASSERT( pVertexInputLayout );
    spCommandProxy->SetVertexInputLayout( pVertexInputLayout );

    uint_fast32_t glyphIndexOffset = 0;

    RTexture2d* pPreviousTexture = NULL;

    for( size_t drawIndex = 0; drawIndex < textDrawCount; ++drawIndex )
    {
        const TextDrawCall& rDrawCall = m_screenTextDrawCalls[ drawIndex ];

        uint_fast32_t drawCallGlyphCount = rDrawCall.glyphCount;

        Font* pFont = rRenderResourceManager.GetDebugFont( rDrawCall.size );
        if( !pFont )
        {
            glyphIndexOffset += drawCallGlyphCount;

            continue;
        }

        uint32_t fontCharacterCount = pFont->GetCharacterCount();

        for( uint_fast32_t drawCallGlyphIndex = 0; drawCallGlyphIndex < drawCallGlyphCount; ++drawCallGlyphIndex )
        {
            uint32_t glyphIndex = m_screenTextGlyphIndices[ glyphIndexOffset ];
            if( glyphIndex < fontCharacterCount )
            {
                const Font::Character& rCharacter = pFont->GetCharacter( glyphIndex );
                RTexture2d* pTexture = pFont->GetTextureSheet( rCharacter.texture );
                if( pTexture )
                {
                    if( pTexture != pPreviousTexture )
                    {
                        spCommandProxy->SetTexture( 0, pTexture );
                        pPreviousTexture = pTexture;
                    }

                    spCommandProxy->DrawIndexed(
                        RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST,
                        static_cast< uint32_t >( glyphIndexOffset * 4 ),
                        0,
                        4,
                        0,
                        2 );
                }
            }

            ++glyphIndexOffset;
        }
    }

    if( pPreviousTexture )
    {
        spCommandProxy->SetTexture( 0, NULL );
    }
}

/// Draw world elements for the specified depth test/write mode.
///
/// @param[in] rWorldResources  Cached references to various resources used during rendering.
/// @param[in] depthMode        Mode in which to handle depth testing and writing.
///
/// @see DrawWorldElements()
void BufferedDrawer::DrawDepthModeWorldElements( WorldElementResources& rWorldResources, EDepthMode depthMode )
{
    Renderer* pRenderer = Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );

    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();

    ResourceSet& rResourceSet = m_resourceSets[ m_currentResourceSetIndex ];

    RRenderCommandProxy* pCommandProxy = rWorldResources.spCommandProxy;
    HELIUM_ASSERT( pCommandProxy );

    StateCache* pStateCache = rWorldResources.pStateCache;
    HELIUM_ASSERT( pStateCache );

    RRasterizerState* pRasterizerStateDefault = rRenderResourceManager.GetRasterizerState(
        RenderResourceManager::RASTERIZER_STATE_DEFAULT );
    HELIUM_ASSERT( pRasterizerStateDefault );
    RRasterizerState* pRasterizerStateWireframe = rRenderResourceManager.GetRasterizerState(
        RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
    HELIUM_ASSERT( pRasterizerStateWireframe );

    RBlendState* pBlendStateTransparent = rRenderResourceManager.GetBlendState(
        RenderResourceManager::BLEND_STATE_TRANSPARENT );
    HELIUM_ASSERT( pBlendStateTransparent );

    const Simd::Matrix44& rInverseViewProjection = rWorldResources.inverseViewProjection;

    // Draw textured primitives first.
    const DynArray< TexturedBufferDrawCall >& rTexturedBufferDrawCalls = m_texturedBufferDrawCalls[ depthMode ];
    size_t texturedBufferDrawCallCount = rTexturedBufferDrawCalls.GetSize();
    if( texturedBufferDrawCallCount != 0 )
    {
        pStateCache->SetVertexShader( rWorldResources.spTextureBlendVertexShader );
        pStateCache->SetPixelShader( rWorldResources.spTextureBlendPixelShader );

        rWorldResources.spTextureBlendVertexShader->CacheDescription(
            pRenderer,
            rWorldResources.spSimpleTexturedVertexDescription );
        RVertexInputLayout* pVertexInputLayout =
            rWorldResources.spTextureBlendVertexShader->GetCachedInputLayout();
        HELIUM_ASSERT( pVertexInputLayout );
        pStateCache->SetVertexInputLayout( pVertexInputLayout );

        pStateCache->SetRasterizerState( pRasterizerStateDefault );
        pStateCache->SetBlendState( pBlendStateTransparent );

        for( size_t drawCallIndex = 0; drawCallIndex < texturedBufferDrawCallCount; ++drawCallIndex )
        {
            const TexturedBufferDrawCall& rDrawCall = rTexturedBufferDrawCalls[ drawCallIndex ];

            pStateCache->SetVertexBuffer(
                rDrawCall.spVertexBuffer,
                static_cast< uint32_t >( sizeof( SimpleTexturedVertex ) ) );
            pStateCache->SetTexture( rDrawCall.spTexture );

            RConstantBuffer* pConstantBuffer = SetInstanceVertexConstantData(
                pCommandProxy,
                rResourceSet,
                rInverseViewProjection,
                rDrawCall.transform );
            HELIUM_ASSERT( pConstantBuffer );
            pStateCache->SetVertexConstantBuffer( pConstantBuffer );

            pConstantBuffer = SetInstancePixelConstantData( pCommandProxy, rResourceSet, rDrawCall.blendColor );
            HELIUM_ASSERT( pConstantBuffer );
            pStateCache->SetPixelConstantBuffer( pConstantBuffer );

            RIndexBuffer* pIndexBuffer = rDrawCall.spIndexBuffer;
            if( pIndexBuffer )
            {
                pStateCache->SetIndexBuffer( pIndexBuffer );
                pCommandProxy->DrawIndexed(
                    rDrawCall.primitiveType,
                    rDrawCall.baseVertexIndex,
                    0,
                    rDrawCall.vertexCount,
                    rDrawCall.startIndex,
                    rDrawCall.primitiveCount );
            }
            else
            {
                pCommandProxy->DrawUnindexed(
                    rDrawCall.primitiveType,
                    rDrawCall.baseVertexIndex,
                    rDrawCall.primitiveCount );
            }
        }
    }

    if( rResourceSet.spTexturedVertexBuffer && rResourceSet.spTexturedIndexBuffer )
    {
        const DynArray< TexturedDrawCall >& rTexturedDrawCalls = m_texturedDrawCalls[ depthMode ];
        const DynArray< TexturedDrawCall >& rWorldTextDrawCalls = m_worldTextDrawCalls[ depthMode ];
        size_t texturedDrawCallCount = rTexturedDrawCalls.GetSize();
        size_t worldTextDrawCallCount = rWorldTextDrawCalls.GetSize();

        if( ( texturedDrawCallCount | worldTextDrawCallCount ) != 0 )
        {
            pStateCache->SetVertexBuffer(
                rResourceSet.spTexturedVertexBuffer,
                static_cast< uint32_t >( sizeof( SimpleTexturedVertex ) ) );
            pStateCache->SetIndexBuffer( rResourceSet.spTexturedIndexBuffer );

            RConstantBuffer* pConstantBuffer = SetInstanceVertexConstantData(
                pCommandProxy,
                rResourceSet,
                rInverseViewProjection,
                Simd::Matrix44::IDENTITY );
            HELIUM_ASSERT( pConstantBuffer );
            pStateCache->SetVertexConstantBuffer( pConstantBuffer );

            if( texturedDrawCallCount != 0 )
            {
                pStateCache->SetVertexShader( rWorldResources.spTextureBlendVertexShader );
                pStateCache->SetPixelShader( rWorldResources.spTextureBlendPixelShader );

                rWorldResources.spTextureBlendVertexShader->CacheDescription(
                    pRenderer,
                    rWorldResources.spSimpleTexturedVertexDescription );
                RVertexInputLayout* pVertexInputLayout =
                    rWorldResources.spTextureBlendVertexShader->GetCachedInputLayout();
                HELIUM_ASSERT( pVertexInputLayout );
                pStateCache->SetVertexInputLayout( pVertexInputLayout );

                pStateCache->SetRasterizerState( pRasterizerStateDefault );
                pStateCache->SetBlendState( pBlendStateTransparent );

                for( size_t drawCallIndex = 0; drawCallIndex < texturedDrawCallCount; ++drawCallIndex )
                {
                    const TexturedDrawCall& rDrawCall = rTexturedDrawCalls[ drawCallIndex ];

                    pStateCache->SetTexture( rDrawCall.spTexture );

                    RConstantBuffer* pPixelConstantBuffer = SetInstancePixelConstantData(
                        pCommandProxy,
                        rResourceSet,
                        rDrawCall.blendColor );
                    HELIUM_ASSERT( pPixelConstantBuffer );
                    pStateCache->SetPixelConstantBuffer( pPixelConstantBuffer );

                    uint32_t startIndex = rDrawCall.startIndex;
                    if( IsValid( startIndex ) )
                    {
                        pCommandProxy->DrawIndexed(
                            rDrawCall.primitiveType,
                            rDrawCall.baseVertexIndex,
                            0,
                            rDrawCall.vertexCount,
                            startIndex,
                            rDrawCall.primitiveCount );
                    }
                    else
                    {
                        pCommandProxy->DrawUnindexed(
                            rDrawCall.primitiveType,
                            rDrawCall.baseVertexIndex,
                            rDrawCall.primitiveCount );
                    }
                }
            }

            if( worldTextDrawCallCount != 0 )
            {
                pStateCache->SetVertexShader( rWorldResources.spTextureAlphaVertexShader );
                pStateCache->SetPixelShader( rWorldResources.spTextureAlphaPixelShader );

                rWorldResources.spTextureAlphaVertexShader->CacheDescription(
                    pRenderer,
                    rWorldResources.spSimpleTexturedVertexDescription );
                RVertexInputLayout* pVertexInputLayout =
                    rWorldResources.spTextureAlphaVertexShader->GetCachedInputLayout();
                HELIUM_ASSERT( pVertexInputLayout );
                pStateCache->SetVertexInputLayout( pVertexInputLayout );

                pStateCache->SetRasterizerState( pRasterizerStateDefault );
                pStateCache->SetBlendState( pBlendStateTransparent );

                for( size_t drawCallIndex = 0; drawCallIndex < worldTextDrawCallCount; ++drawCallIndex )
                {
                    const TexturedDrawCall& rDrawCall = rWorldTextDrawCalls[ drawCallIndex ];

                    pStateCache->SetTexture( rDrawCall.spTexture );

                    RConstantBuffer* pPixelConstantBuffer = SetInstancePixelConstantData(
                        pCommandProxy,
                        rResourceSet,
                        rDrawCall.blendColor );
                    HELIUM_ASSERT( pPixelConstantBuffer );
                    pStateCache->SetPixelConstantBuffer( pPixelConstantBuffer );

                    HELIUM_ASSERT( IsValid( rDrawCall.startIndex ) );  // Text should always used indexed rendering.
                    pCommandProxy->DrawIndexed(
                        rDrawCall.primitiveType,
                        rDrawCall.baseVertexIndex,
                        0,
                        rDrawCall.vertexCount,
                        rDrawCall.startIndex,
                        rDrawCall.primitiveCount );
                }
            }
        }
    }

    // Draw untextured data.
    const DynArray< UntexturedBufferDrawCall >& rSolidBufferDrawCalls = m_solidBufferDrawCalls[ depthMode ];
    const DynArray< UntexturedBufferDrawCall >& rWireBufferDrawCalls = m_wireBufferDrawCalls[ depthMode ];
    size_t solidBufferDrawCallCount = rSolidBufferDrawCalls.GetSize();
    size_t wireBufferDrawCallCount = rWireBufferDrawCalls.GetSize();
    if( ( solidBufferDrawCallCount | wireBufferDrawCallCount ) != 0 )
    {
        pStateCache->SetVertexShader( rWorldResources.spUntexturedVertexShader );
        pStateCache->SetPixelShader( rWorldResources.spUntexturedPixelShader );

        rWorldResources.spUntexturedVertexShader->CacheDescription(
            pRenderer,
            rWorldResources.spSimpleVertexDescription );
        RVertexInputLayout* pVertexInputLayout = rWorldResources.spUntexturedVertexShader->GetCachedInputLayout();
        HELIUM_ASSERT( pVertexInputLayout );
        pStateCache->SetVertexInputLayout( pVertexInputLayout );

        pStateCache->SetRasterizerState( pRasterizerStateDefault );
        pStateCache->SetBlendState( pBlendStateTransparent );

        pStateCache->SetTexture( NULL );

        for( size_t drawCallIndex = 0; drawCallIndex < solidBufferDrawCallCount; ++drawCallIndex )
        {
            const UntexturedBufferDrawCall& rDrawCall = rSolidBufferDrawCalls[ drawCallIndex ];

            pStateCache->SetVertexBuffer( rDrawCall.spVertexBuffer, static_cast< uint32_t >( sizeof( SimpleVertex ) ) );

            RConstantBuffer* pConstantBuffer = SetInstanceVertexConstantData(
                pCommandProxy,
                rResourceSet,
                rInverseViewProjection,
                rDrawCall.transform );
            HELIUM_ASSERT( pConstantBuffer );
            pStateCache->SetVertexConstantBuffer( pConstantBuffer );

            RConstantBuffer* pPixelConstantBuffer = SetInstancePixelConstantData(
                pCommandProxy,
                rResourceSet,
                rDrawCall.blendColor );
            HELIUM_ASSERT( pPixelConstantBuffer );
            pStateCache->SetPixelConstantBuffer( pPixelConstantBuffer );

            RIndexBuffer* pIndexBuffer = rDrawCall.spIndexBuffer;
            if( pIndexBuffer )
            {
                pStateCache->SetIndexBuffer( pIndexBuffer );
                pCommandProxy->DrawIndexed(
                    rDrawCall.primitiveType,
                    rDrawCall.baseVertexIndex,
                    0,
                    rDrawCall.vertexCount,
                    rDrawCall.startIndex,
                    rDrawCall.primitiveCount );
            }
            else
            {
                pCommandProxy->DrawUnindexed(
                    rDrawCall.primitiveType,
                    rDrawCall.baseVertexIndex,
                    rDrawCall.primitiveCount );
            }
        }

        pStateCache->SetRasterizerState( pRasterizerStateWireframe );

        for( size_t drawCallIndex = 0; drawCallIndex < wireBufferDrawCallCount; ++drawCallIndex )
        {
            const UntexturedBufferDrawCall& rDrawCall = rWireBufferDrawCalls[ drawCallIndex ];

            pStateCache->SetVertexBuffer( rDrawCall.spVertexBuffer, static_cast< uint32_t >( sizeof( SimpleVertex ) ) );

            RConstantBuffer* pConstantBuffer = SetInstanceVertexConstantData(
                pCommandProxy,
                rResourceSet,
                rInverseViewProjection,
                rDrawCall.transform );
            HELIUM_ASSERT( pConstantBuffer );
            pStateCache->SetVertexConstantBuffer( pConstantBuffer );

            RConstantBuffer* pPixelConstantBuffer = SetInstancePixelConstantData(
                pCommandProxy,
                rResourceSet,
                rDrawCall.blendColor );
            HELIUM_ASSERT( pPixelConstantBuffer );
            pStateCache->SetPixelConstantBuffer( pPixelConstantBuffer );

            RIndexBuffer* pIndexBuffer = rDrawCall.spIndexBuffer;
            if( pIndexBuffer )
            {
                pStateCache->SetIndexBuffer( pIndexBuffer );
                pCommandProxy->DrawIndexed(
                    rDrawCall.primitiveType,
                    rDrawCall.baseVertexIndex,
                    0,
                    rDrawCall.vertexCount,
                    rDrawCall.startIndex,
                    rDrawCall.primitiveCount );
            }
            else
            {
                pCommandProxy->DrawUnindexed(
                    rDrawCall.primitiveType,
                    rDrawCall.baseVertexIndex,
                    rDrawCall.primitiveCount );
            }
        }
    }

    if( rResourceSet.spUntexturedVertexBuffer && rResourceSet.spUntexturedIndexBuffer )
    {
        const DynArray< UntexturedDrawCall >& rSolidDrawCalls = m_solidDrawCalls[ depthMode ];
        const DynArray< UntexturedDrawCall >& rWireDrawCalls = m_wireDrawCalls[ depthMode ];
        size_t solidDrawCallCount = rSolidDrawCalls.GetSize();
        size_t wireDrawCallCount = rWireDrawCalls.GetSize();
        if( ( solidDrawCallCount | wireDrawCallCount ) != 0 )
        {
            pStateCache->SetVertexShader( rWorldResources.spUntexturedVertexShader );
            pStateCache->SetPixelShader( rWorldResources.spUntexturedPixelShader );

            pStateCache->SetVertexBuffer(
                rResourceSet.spUntexturedVertexBuffer,
                static_cast< uint32_t >( sizeof( SimpleVertex ) ) );
            pStateCache->SetIndexBuffer( rResourceSet.spUntexturedIndexBuffer );

            rWorldResources.spUntexturedVertexShader->CacheDescription(
                pRenderer,
                rWorldResources.spSimpleVertexDescription );
            RVertexInputLayout* pVertexInputLayout = rWorldResources.spUntexturedVertexShader->GetCachedInputLayout();
            HELIUM_ASSERT( pVertexInputLayout );
            pStateCache->SetVertexInputLayout( pVertexInputLayout );

            pStateCache->SetRasterizerState( pRasterizerStateDefault );
            pStateCache->SetBlendState( pBlendStateTransparent );

            pStateCache->SetTexture( NULL );

            RConstantBuffer* pConstantBuffer = SetInstanceVertexConstantData(
                pCommandProxy,
                rResourceSet,
                rInverseViewProjection,
                Simd::Matrix44::IDENTITY );
            HELIUM_ASSERT( pConstantBuffer );
            pStateCache->SetVertexConstantBuffer( pConstantBuffer );

            for( size_t drawCallIndex = 0; drawCallIndex < solidDrawCallCount; ++drawCallIndex )
            {
                const UntexturedDrawCall& rDrawCall = rSolidDrawCalls[ drawCallIndex ];

                RConstantBuffer* pPixelConstantBuffer = SetInstancePixelConstantData(
                    pCommandProxy,
                    rResourceSet,
                    rDrawCall.blendColor );
                HELIUM_ASSERT( pPixelConstantBuffer );
                pStateCache->SetPixelConstantBuffer( pPixelConstantBuffer );

                uint32_t startIndex = rDrawCall.startIndex;
                if( IsValid( startIndex ) )
                {
                    pCommandProxy->DrawIndexed(
                        rDrawCall.primitiveType,
                        rDrawCall.baseVertexIndex,
                        0,
                        rDrawCall.vertexCount,
                        startIndex,
                        rDrawCall.primitiveCount );
                }
                else
                {
                    pCommandProxy->DrawUnindexed(
                        rDrawCall.primitiveType,
                        rDrawCall.baseVertexIndex,
                        rDrawCall.primitiveCount );
                }
            }

            pStateCache->SetRasterizerState( pRasterizerStateWireframe );

            for( size_t drawCallIndex = 0; drawCallIndex < wireDrawCallCount; ++drawCallIndex )
            {
                const UntexturedDrawCall& rDrawCall = rWireDrawCalls[ drawCallIndex ];

                RConstantBuffer* pPixelConstantBuffer = SetInstancePixelConstantData(
                    pCommandProxy,
                    rResourceSet,
                    rDrawCall.blendColor );
                HELIUM_ASSERT( pPixelConstantBuffer );
                pStateCache->SetPixelConstantBuffer( pPixelConstantBuffer );

                uint32_t startIndex = rDrawCall.startIndex;
                if( IsValid( startIndex ) )
                {
                    pCommandProxy->DrawIndexed(
                        rDrawCall.primitiveType,
                        rDrawCall.baseVertexIndex,
                        0,
                        rDrawCall.vertexCount,
                        startIndex,
                        rDrawCall.primitiveCount );
                }
                else
                {
                    pCommandProxy->DrawUnindexed(
                        rDrawCall.primitiveType,
                        rDrawCall.baseVertexIndex,
                        rDrawCall.primitiveCount );
                }
            }
        }
    }

    // Draw non-pixel points.
    const DynArray< UntexturedBufferDrawCall >& rPointBufferDrawCalls = m_pointBufferDrawCalls[ depthMode ];
    size_t pointBufferDrawCallCount = rPointBufferDrawCalls.GetSize();
    if( pointBufferDrawCallCount != 0 )
    {
        pStateCache->SetVertexShader( rWorldResources.spUntexturedPointsVertexShader );
        pStateCache->SetPixelShader( rWorldResources.spUntexturedPixelShader );

        rWorldResources.spUntexturedPointsVertexShader->CacheDescription(
            pRenderer,
            rWorldResources.spSimpleVertexDescription );
        RVertexInputLayout* pVertexInputLayout = rWorldResources.spUntexturedPointsVertexShader->GetCachedInputLayout();
        HELIUM_ASSERT( pVertexInputLayout );
        pStateCache->SetVertexInputLayout( pVertexInputLayout );

        pStateCache->SetRasterizerState( pRasterizerStateDefault );
        pStateCache->SetBlendState( pBlendStateTransparent );

        pStateCache->SetTexture( NULL );

        for( size_t drawCallIndex = 0; drawCallIndex < pointBufferDrawCallCount; ++drawCallIndex )
        {
            const UntexturedBufferDrawCall& rDrawCall = rPointBufferDrawCalls[ drawCallIndex ];

            pStateCache->SetVertexBuffer( rDrawCall.spVertexBuffer, static_cast< uint32_t >( sizeof( SimpleVertex ) ) );

            RConstantBuffer* pConstantBuffer = SetInstanceVertexConstantData(
                pCommandProxy,
                rResourceSet,
                rInverseViewProjection,
                rDrawCall.transform );
            HELIUM_ASSERT( pConstantBuffer );
            pStateCache->SetVertexConstantBuffer( pConstantBuffer );

            RConstantBuffer* pPixelConstantBuffer = SetInstancePixelConstantData(
                pCommandProxy,
                rResourceSet,
                rDrawCall.blendColor );
            HELIUM_ASSERT( pPixelConstantBuffer );
            pStateCache->SetPixelConstantBuffer( pPixelConstantBuffer );

            HELIUM_ASSERT( !rDrawCall.spIndexBuffer );  // No index buffer is given for points.
            pCommandProxy->DrawUnindexed(
                rDrawCall.primitiveType,
                rDrawCall.baseVertexIndex,
                rDrawCall.primitiveCount );
        }
    }

    if( rResourceSet.spUntexturedVertexBuffer && rResourceSet.spUntexturedIndexBuffer )
    {
        const DynArray< UntexturedDrawCall >& rPointDrawCalls = m_pointDrawCalls[ depthMode ];
        size_t pointDrawCallCount = rPointDrawCalls.GetSize();
        if( pointDrawCallCount != 0 )
        {
            pStateCache->SetVertexShader( rWorldResources.spUntexturedPointsVertexShader );
            pStateCache->SetPixelShader( rWorldResources.spUntexturedPixelShader );

            pStateCache->SetVertexBuffer(
                rResourceSet.spUntexturedVertexBuffer,
                static_cast< uint32_t >( sizeof( SimpleVertex ) ) );
            // No index buffer is given for points.

            rWorldResources.spUntexturedPointsVertexShader->CacheDescription(
                pRenderer,
                rWorldResources.spSimpleVertexDescription );
            RVertexInputLayout* pVertexInputLayout =
                rWorldResources.spUntexturedPointsVertexShader->GetCachedInputLayout();
            HELIUM_ASSERT( pVertexInputLayout );
            pStateCache->SetVertexInputLayout( pVertexInputLayout );

            pStateCache->SetRasterizerState( pRasterizerStateDefault );
            pStateCache->SetBlendState( pBlendStateTransparent );

            pStateCache->SetTexture( NULL );

            RConstantBuffer* pConstantBuffer = SetInstanceVertexConstantData(
                pCommandProxy,
                rResourceSet,
                rInverseViewProjection,
                Simd::Matrix44::IDENTITY );
            HELIUM_ASSERT( pConstantBuffer );
            pStateCache->SetVertexConstantBuffer( pConstantBuffer );

            for( size_t drawCallIndex = 0; drawCallIndex < pointDrawCallCount; ++drawCallIndex )
            {
                const UntexturedDrawCall& rDrawCall = rPointDrawCalls[ drawCallIndex ];

                RConstantBuffer* pPixelConstantBuffer = SetInstancePixelConstantData(
                    pCommandProxy,
                    rResourceSet,
                    rDrawCall.blendColor );
                HELIUM_ASSERT( pPixelConstantBuffer );
                pStateCache->SetPixelConstantBuffer( pPixelConstantBuffer );

                // No index buffer is given for points.
                pCommandProxy->DrawUnindexed(
                    rDrawCall.primitiveType,
                    rDrawCall.baseVertexIndex,
                    rDrawCall.primitiveCount );
            }
        }
    }
}

/// Set the vertex shader constant data for the current draw instance.
///
/// @param[in] pCommandProxy           Interface through which render commands should be issued.
/// @param[in] rResourceSet            Active resource set data for the current frame.
/// @param[in] rInverseViewProjection  Current view/projection matrix.
/// @param[in] rTransform              World transform to apply.
///
/// @return  Vertex shader constant buffer to use for the current instance.
///
/// @see SetInstancePixelConstantData()
RConstantBuffer* BufferedDrawer::SetInstanceVertexConstantData(
    RRenderCommandProxy* pCommandProxy,
    ResourceSet& rResourceSet,
    const Simd::Matrix44& rInverseViewProjection,
    const Simd::Matrix44& rTransform )
{
    HELIUM_ASSERT( pCommandProxy );

    uint32_t bufferIndex = rResourceSet.instanceVertexConstantBufferIndex;
    bool bFirstUpdate = IsInvalid( bufferIndex );

    if( bFirstUpdate || rResourceSet.instanceVertexConstantTransform != rTransform )
    {
        Renderer* pRenderer = Renderer::GetStaticInstance();
        HELIUM_ASSERT( pRenderer );

        if( !bFirstUpdate )
        {
            HELIUM_ASSERT( bufferIndex < HELIUM_ARRAY_COUNT( m_instanceVertexConstantFences ) );
            HELIUM_ASSERT( !m_instanceVertexConstantFences[ bufferIndex ] );

            RFence* pFence = pRenderer->CreateFence();
            HELIUM_ASSERT( pFence );
            m_instanceVertexConstantFences[ bufferIndex ] = pFence;

            pCommandProxy->SetFence( pFence );
        }

        Simd::Matrix44 worldInverseViewProjection;
        worldInverseViewProjection.MultiplySet( rTransform, rInverseViewProjection );
        worldInverseViewProjection.Transpose();

        bufferIndex = ( bufferIndex + 1 ) % HELIUM_ARRAY_COUNT( rResourceSet.instanceVertexConstantBuffers );
        rResourceSet.instanceVertexConstantBufferIndex = bufferIndex;

        RFence* pFence = m_instanceVertexConstantFences[ bufferIndex ];
        if( pFence )
        {
            pRenderer->SyncFence( pFence );
            m_instanceVertexConstantFences[ bufferIndex ].Release();
        }

        RConstantBuffer* pConstantBuffer = rResourceSet.instanceVertexConstantBuffers[ bufferIndex ];
        HELIUM_ASSERT( pConstantBuffer );
        float32_t* pConstantValues =
            static_cast< float32_t* >( pConstantBuffer->Map( RENDERER_BUFFER_MAP_HINT_DISCARD ) );
        HELIUM_ASSERT( pConstantValues );

        MemoryCopy( pConstantValues, &worldInverseViewProjection, sizeof( worldInverseViewProjection ) );

        pConstantBuffer->Unmap();

        rResourceSet.instanceVertexConstantTransform = rTransform;
    }

    return rResourceSet.instanceVertexConstantBuffers[ bufferIndex ];
}

/// Set the pixel shader constant data for the current draw instance.
///
/// @param[in] pCommandProxy  Interface through which render commands should be issued.
/// @param[in] rResourceSet   Active resource set data for the current frame.
/// @param[in] blendColor     Color with which to blend each vertex color during rendering.
///
/// @return  Pixel shader constant buffer to use for the current instance.
///
/// @see SetInstanceVertexConstantData()
RConstantBuffer* BufferedDrawer::SetInstancePixelConstantData(
    RRenderCommandProxy* pCommandProxy,
    ResourceSet& rResourceSet,
    Color blendColor )
{
    HELIUM_ASSERT( pCommandProxy );

    uint32_t bufferIndex = rResourceSet.instancePixelConstantBufferIndex;
    bool bFirstUpdate = IsInvalid( bufferIndex );

    if( bFirstUpdate || rResourceSet.instancePixelConstantBlendColor != blendColor )
    {
        Renderer* pRenderer = Renderer::GetStaticInstance();
        HELIUM_ASSERT( pRenderer );

        if( !bFirstUpdate )
        {
            HELIUM_ASSERT( bufferIndex < HELIUM_ARRAY_COUNT( m_instancePixelConstantFences ) );
            HELIUM_ASSERT( !m_instancePixelConstantFences[ bufferIndex ] );

            RFence* pFence = pRenderer->CreateFence();
            HELIUM_ASSERT( pFence );
            m_instancePixelConstantFences[ bufferIndex ] = pFence;

            pCommandProxy->SetFence( pFence );
        }

        bufferIndex = ( bufferIndex + 1 ) % HELIUM_ARRAY_COUNT( rResourceSet.instancePixelConstantBuffers );
        rResourceSet.instancePixelConstantBufferIndex = bufferIndex;

        RFence* pFence = m_instancePixelConstantFences[ bufferIndex ];
        if( pFence )
        {
            pRenderer->SyncFence( pFence );
            m_instancePixelConstantFences[ bufferIndex ].Release();
        }

        RConstantBuffer* pConstantBuffer = rResourceSet.instancePixelConstantBuffers[ bufferIndex ];
        HELIUM_ASSERT( pConstantBuffer );
        float32_t* pConstantValues =
            static_cast< float32_t* >( pConstantBuffer->Map( RENDERER_BUFFER_MAP_HINT_DISCARD ) );
        HELIUM_ASSERT( pConstantValues );

        *( pConstantValues++ ) = static_cast< float32_t >( blendColor.GetR() ) / 255.0f;
        *( pConstantValues++ ) = static_cast< float32_t >( blendColor.GetG() ) / 255.0f;
        *( pConstantValues++ ) = static_cast< float32_t >( blendColor.GetB() ) / 255.0f;
        *pConstantValues = static_cast< float32_t >( blendColor.GetA() ) / 255.0f;

        pConstantBuffer->Unmap();

        rResourceSet.instancePixelConstantBlendColor = blendColor;
    }

    return rResourceSet.instancePixelConstantBuffers[ bufferIndex ];
}

/// Constructor.
///
/// @param[in] pCommandProxy  Render command proxy interface to use when issuing state changes.
BufferedDrawer::StateCache::StateCache( RRenderCommandProxy* pCommandProxy )
    : m_spRenderCommandProxy( pCommandProxy )
{
    ResetStateCache();
}

/// Set the render command proxy to use for issuing state changes.
///
/// @param[in] pCommandProxy  Render command proxy interface to use when issuing state changes.
void BufferedDrawer::StateCache::SetRenderCommandProxy( RRenderCommandProxy* pCommandProxy )
{
    if( m_spRenderCommandProxy != pCommandProxy )
    {
        m_spRenderCommandProxy = pCommandProxy;
        ResetStateCache();
    }
}

/// Set the rasterizer state.
///
/// @param[in] pState  Rasterizer state to set.
void BufferedDrawer::StateCache::SetRasterizerState( RRasterizerState* pState )
{
    HELIUM_ASSERT( m_spRenderCommandProxy );

    if( m_pRasterizerState != pState )
    {
        m_pRasterizerState = pState;
        m_spRenderCommandProxy->SetRasterizerState( pState );
    }
}

/// Set the blend state.
///
/// @param[in] pState  Blend state to set.
void BufferedDrawer::StateCache::SetBlendState( RBlendState* pState )
{
    HELIUM_ASSERT( m_spRenderCommandProxy );

    if( m_pBlendState != pState )
    {
        m_pBlendState = pState;
        m_spRenderCommandProxy->SetBlendState( pState );
    }
}

/// Set the current vertex buffer.
///
/// @param[in] pBuffer  Vertex buffer to set.
/// @param[in] stride   Bytes between consecutive vertices.
void BufferedDrawer::StateCache::SetVertexBuffer( RVertexBuffer* pBuffer, uint32_t stride )
{
    HELIUM_ASSERT( m_spRenderCommandProxy );

    if( m_pVertexBuffer != pBuffer || m_vertexStride != stride )
    {
        m_pVertexBuffer = pBuffer;
        m_vertexStride = stride;

        uint32_t offset = 0;
        m_spRenderCommandProxy->SetVertexBuffers( 0, 1, &pBuffer, &stride, &offset );
    }
}

/// Set the current index buffer.
///
/// @param[in] pBuffer  Index buffer to set.
void BufferedDrawer::StateCache::SetIndexBuffer( RIndexBuffer* pBuffer )
{
    HELIUM_ASSERT( m_spRenderCommandProxy );

    if( m_pIndexBuffer != pBuffer )
    {
        m_pIndexBuffer = pBuffer;
        m_spRenderCommandProxy->SetIndexBuffer( pBuffer );
    }
}

/// Set the current vertex shader.
///
/// @param[in] pShader  Vertex shader to set.
void BufferedDrawer::StateCache::SetVertexShader( RVertexShader* pShader )
{
    HELIUM_ASSERT( m_spRenderCommandProxy );

    if( m_pVertexShader != pShader )
    {
        m_pVertexShader = pShader;
        m_spRenderCommandProxy->SetVertexShader( pShader );
    }
}

/// Set the current pixel shader.
///
/// @param[in] pShader  Pixel shader to set.
void BufferedDrawer::StateCache::SetPixelShader( RPixelShader* pShader )
{
    HELIUM_ASSERT( m_spRenderCommandProxy );

    if( m_pPixelShader != pShader )
    {
        m_pPixelShader = pShader;
        m_spRenderCommandProxy->SetPixelShader( pShader );
    }
}

/// Set the current vertex input layout.
///
/// @param[in] pLayout  Vertex input layout to set.
void BufferedDrawer::StateCache::SetVertexInputLayout( RVertexInputLayout* pLayout )
{
    HELIUM_ASSERT( m_spRenderCommandProxy );

    if( m_pVertexInputLayout != pLayout )
    {
        m_pVertexInputLayout = pLayout;
        m_spRenderCommandProxy->SetVertexInputLayout( pLayout );
    }
}

/// Set the current vertex shader constant buffer.
///
/// @param[in] pConstantBuffer  Constant buffer to set.
void BufferedDrawer::StateCache::SetVertexConstantBuffer( RConstantBuffer* pConstantBuffer )
{
    HELIUM_ASSERT( m_spRenderCommandProxy );

    if( m_pVertexConstantBuffer != pConstantBuffer )
    {
        m_pVertexConstantBuffer = pConstantBuffer;
        m_spRenderCommandProxy->SetVertexConstantBuffers( 0, 1, &pConstantBuffer );
    }
}

/// Set the current pixel shader constant buffer.
///
/// @param[in] pConstantBuffer  Constant buffer to set.
void BufferedDrawer::StateCache::SetPixelConstantBuffer( RConstantBuffer* pConstantBuffer )
{
    HELIUM_ASSERT( m_spRenderCommandProxy );

    if( m_pPixelConstantBuffer != pConstantBuffer )
    {
        m_pPixelConstantBuffer = pConstantBuffer;
        m_spRenderCommandProxy->SetPixelConstantBuffers( 0, 1, &pConstantBuffer );
    }
}

/// Set the current texture.
///
/// @param[in] pTexture  Texture to set.
void BufferedDrawer::StateCache::SetTexture( RTexture2d* pTexture )
{
    HELIUM_ASSERT( m_spRenderCommandProxy );

    if( m_pTexture != pTexture )
    {
        m_pTexture = pTexture;
        m_spRenderCommandProxy->SetTexture( 0, pTexture );
    }
}

/// Clear all cached state data.
void BufferedDrawer::StateCache::ResetStateCache()
{
    m_pRasterizerState = NULL;
    m_pBlendState = NULL;

    m_pVertexBuffer = NULL;
    m_vertexStride = 0;

    m_pIndexBuffer = NULL;

    m_pVertexShader = NULL;
    m_pPixelShader = NULL;
    m_pVertexInputLayout = NULL;

    m_pVertexConstantBuffer = NULL;
    m_pPixelConstantBuffer = NULL;

    m_pTexture = NULL;
}

/// Constructor.
///
/// @param[in] pDrawer     Buffered drawer instance being used to perform the rendering.
/// @param[in] pFont       Font being used for rendering.
/// @param[in] color       Text color.
/// @param[in] depthMode   Mode in which to handle depth testing and writing.
/// @param[in] rTransform  World-space transform matrix.
BufferedDrawer::WorldSpaceTextGlyphHandler::WorldSpaceTextGlyphHandler(
    BufferedDrawer* pDrawer,
    Font* pFont,
    Color color,
    EDepthMode depthMode,
    const Simd::Matrix44& rTransform )
    : m_rTransform( rTransform )
    , m_pDrawer( pDrawer )
    , m_pFont( pFont )
    , m_color( color )
    , m_depthMode( depthMode )
    , m_inverseTextureWidth( 1.0f / static_cast< float32_t >( pFont->GetTextureSheetWidth() ) )
    , m_inverseTextureHeight( 1.0f / static_cast< float32_t >( pFont->GetTextureSheetHeight() ) )
    , m_penX( 0.0f )
{
    m_quadIndices[ 0 ] = 0;
    m_quadIndices[ 1 ] = 1;
    m_quadIndices[ 2 ] = 2;
    m_quadIndices[ 3 ] = 0;
    m_quadIndices[ 4 ] = 2;
    m_quadIndices[ 5 ] = 3;
}

/// Draw the specified character.
///
/// @param[in] pCharacter  Character to draw.
void BufferedDrawer::WorldSpaceTextGlyphHandler::operator()( const Font::Character* pCharacter )
{
    HELIUM_ASSERT( pCharacter );

    RTexture2d* pTexture = m_pFont->GetTextureSheet( pCharacter->texture );
    if( !pTexture )
    {
        return;
    }

    float32_t imageWidthFloat = static_cast< float32_t >( pCharacter->imageWidth );
    float32_t imageHeightFloat = static_cast< float32_t >( pCharacter->imageHeight );

    float32_t cornerMinX = Floor( m_penX + 0.5f ) + static_cast< float32_t >( pCharacter->bearingX >> 6 );
    float32_t cornerMinY = static_cast< float32_t >( pCharacter->bearingY >> 6 );
    float32_t cornerMaxX = cornerMinX + imageWidthFloat;
    float32_t cornerMaxY = cornerMinY - imageHeightFloat;

    Simd::Vector3 corners[] =
    {
        Simd::Vector3( cornerMinX, cornerMinY, 0.0f ),
        Simd::Vector3( cornerMaxX, cornerMinY, 0.0f ),
        Simd::Vector3( cornerMaxX, cornerMaxY, 0.0f ),
        Simd::Vector3( cornerMinX, cornerMaxY, 0.0f )
    };

    m_rTransform.TransformPoint( corners[ 0 ], corners[ 0 ] );
    m_rTransform.TransformPoint( corners[ 1 ], corners[ 1 ] );
    m_rTransform.TransformPoint( corners[ 2 ], corners[ 2 ] );
    m_rTransform.TransformPoint( corners[ 3 ], corners[ 3 ] );

    float32_t texCoordMinX = static_cast< float32_t >( pCharacter->imageX );
    float32_t texCoordMinY = static_cast< float32_t >( pCharacter->imageY );
    float32_t texCoordMaxX = texCoordMinX + imageWidthFloat;
    float32_t texCoordMaxY = texCoordMinY + imageHeightFloat;

    texCoordMinX *= m_inverseTextureWidth;
    texCoordMinY *= m_inverseTextureHeight;
    texCoordMaxX *= m_inverseTextureWidth;
    texCoordMaxY *= m_inverseTextureHeight;

    const SimpleTexturedVertex vertices[] =
    {
        SimpleTexturedVertex( corners[ 0 ], Simd::Vector2( texCoordMinX, texCoordMinY ), m_color ),
        SimpleTexturedVertex( corners[ 1 ], Simd::Vector2( texCoordMaxX, texCoordMinY ), m_color ),
        SimpleTexturedVertex( corners[ 2 ], Simd::Vector2( texCoordMaxX, texCoordMaxY ), m_color ),
        SimpleTexturedVertex( corners[ 3 ], Simd::Vector2( texCoordMinX, texCoordMaxY ), m_color )
    };

    uint32_t baseVertexIndex = static_cast< uint32_t >( m_pDrawer->m_texturedVertices.GetSize() );
    uint32_t startIndex = static_cast< uint32_t >( m_pDrawer->m_texturedIndices.GetSize() );

    m_pDrawer->m_texturedVertices.AddArray( vertices, 4 );
    m_pDrawer->m_texturedIndices.AddArray( m_quadIndices, 6 );

    TexturedDrawCall* pDrawCall = m_pDrawer->m_worldTextDrawCalls[ m_depthMode ].New();
    HELIUM_ASSERT( pDrawCall );
    pDrawCall->primitiveType = RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST;
    pDrawCall->baseVertexIndex = baseVertexIndex;
    pDrawCall->vertexCount = 4;
    pDrawCall->startIndex = startIndex;
    pDrawCall->primitiveCount = 2;
    pDrawCall->blendColor = Color( 0xffffffff );
    pDrawCall->spTexture = pTexture;

    m_penX += Font::Fixed26x6ToFloat32( pCharacter->advance );
}

/// Constructor.
///
/// @param[in] pDrawer  Buffered drawer instance being used to perform the rendering.
/// @param[in] pFont    Font being used for rendering.
/// @param[in] x        Pixel x-coordinate at which to begin rendering the text.
/// @param[in] y        Pixel y-coordinate at which to begin rendering the text.
/// @param[in] color    Color with which to render the text.
/// @param[in] size     Size at which to render the text.
BufferedDrawer::ScreenSpaceTextGlyphHandler::ScreenSpaceTextGlyphHandler(
    BufferedDrawer* pDrawer,
    Font* pFont,
    int32_t x,
    int32_t y,
    Color color,
    RenderResourceManager::EDebugFontSize size )
    : m_pDrawer( pDrawer )
    , m_pFont( pFont )
    , m_pDrawCall( NULL )
    , m_x( x )
    , m_y( y )
    , m_color( color )
    , m_size( size )
{
}

/// Draw the specified character.
///
/// @param[in] pCharacter  Character to draw.
void BufferedDrawer::ScreenSpaceTextGlyphHandler::operator()( const Font::Character* pCharacter )
{
    HELIUM_ASSERT( pCharacter );

    uint32_t characterIndex = m_pFont->GetCharacterIndex( pCharacter );

    m_pDrawer->m_screenTextGlyphIndices.Push( characterIndex );

    if( !m_pDrawCall )
    {
        m_pDrawCall = m_pDrawer->m_screenTextDrawCalls.New();
        HELIUM_ASSERT( m_pDrawCall );
        m_pDrawCall->x = m_x;
        m_pDrawCall->y = m_y;
        m_pDrawCall->color = m_color;
        m_pDrawCall->size = m_size;
        m_pDrawCall->glyphCount = 0;
    }

    ++m_pDrawCall->glyphCount;
}
