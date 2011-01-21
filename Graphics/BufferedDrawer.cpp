#include "GraphicsPch.h"
#include "Graphics/BufferedDrawer.h"

#include "Platform/Math/Simd/Matrix44.h"
#include "Foundation/StringConverter.h"
#include "Rendering/Renderer.h"
#include "Rendering/RConstantBuffer.h"
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
        m_lineDrawCalls[ depthModeIndex ].Clear();
        m_wireMeshDrawCalls[ depthModeIndex ].Clear();
        m_solidMeshDrawCalls[ depthModeIndex ].Clear();
        m_texturedMeshDrawCalls[ depthModeIndex ].Clear();
        m_worldTextDrawCalls[ depthModeIndex ].Clear();
    }

    m_screenTextDrawCalls.Clear();
    m_screenTextGlyphIndices.Clear();

    m_spScreenSpaceTextIndexBuffer.Release();

    for( size_t resourceSetIndex = 0; resourceSetIndex < HELIUM_ARRAY_COUNT( m_resourceSets ); ++resourceSetIndex )
    {
        ResourceSet& rResourceSet = m_resourceSets[ resourceSetIndex ];
        rResourceSet.spUntexturedVertexBuffer.Release();
        rResourceSet.spUntexturedIndexBuffer.Release();
        rResourceSet.spTexturedVertexBuffer.Release();
        rResourceSet.spTexturedIndexBuffer.Release();
        rResourceSet.spScreenSpaceTextVertexBuffer.Release();
        rResourceSet.untexturedVertexBufferSize = 0;
        rResourceSet.untexturedIndexBufferSize = 0;
        rResourceSet.texturedVertexBufferSize = 0;
        rResourceSet.texturedIndexBufferSize = 0;
        rResourceSet.screenSpaceTextVertexBufferSize = 0;
    }

    m_currentResourceSetIndex = 0;

    m_bDrawing = false;
}

/// Buffer a line list draw call.
///
/// @param[in] pVertices    Vertices to use for drawing.
/// @param[in] vertexCount  Number of vertices used for drawing.
/// @param[in] pIndices     Indices to use for drawing.
/// @param[in] lineCount    Number of lines to draw.
/// @param[in] depthMode    Mode in which to handle depth testing and writing.
///
/// @see DrawWireMesh(), DrawSolidMesh(), DrawTexturedMesh()
void BufferedDrawer::DrawLines(
    const SimpleVertex* pVertices,
    uint32_t vertexCount,
    const uint16_t* pIndices,
    uint32_t lineCount,
    EDepthMode depthMode )
{
    HELIUM_ASSERT( pVertices );
    HELIUM_ASSERT( vertexCount );
    HELIUM_ASSERT( pIndices );
    HELIUM_ASSERT( lineCount );
    HELIUM_ASSERT( static_cast< size_t >( depthMode ) < static_cast< size_t >( DEPTH_MODE_MAX ) );

    // Cannot add draw calls while rendering.
    HELIUM_ASSERT( !m_bDrawing );

    // Don't buffer any drawing information if we have no renderer.
    if( !Renderer::GetStaticInstance() )
    {
        return;
    }

    uint32_t baseVertexIndex = static_cast< uint32_t >( m_untexturedVertices.GetSize() );
    uint32_t startIndex = static_cast< uint32_t >( m_untexturedIndices.GetSize() );

    m_untexturedVertices.AddArray( pVertices, vertexCount );
    m_untexturedIndices.AddArray( pIndices, lineCount * 2 );

    UntexturedDrawCall* pDrawCall = m_lineDrawCalls[ depthMode ].New();
    HELIUM_ASSERT( pDrawCall );
    pDrawCall->baseVertexIndex = baseVertexIndex;
    pDrawCall->vertexCount = vertexCount;
    pDrawCall->startIndex = startIndex;
    pDrawCall->primitiveCount = lineCount;
}

/// Buffer a wireframe mesh draw call.
///
/// @param[in] pVertices      Vertices to use for drawing.
/// @param[in] vertexCount    Number of vertices used for drawing.
/// @param[in] pIndices       Indices to use for drawing.
/// @param[in] triangleCount  Number of triangles to draw.
/// @param[in] depthMode      Mode in which to handle depth testing and writing.
///
/// @see DrawLines(), DrawSolidMesh(), DrawTexturedMesh()
void BufferedDrawer::DrawWireMesh(
    const SimpleVertex* pVertices,
    uint32_t vertexCount,
    const uint16_t* pIndices,
    uint32_t triangleCount,
    EDepthMode depthMode )
{
    HELIUM_ASSERT( pVertices );
    HELIUM_ASSERT( vertexCount );
    HELIUM_ASSERT( pIndices );
    HELIUM_ASSERT( triangleCount );
    HELIUM_ASSERT( static_cast< size_t >( depthMode ) < static_cast< size_t >( DEPTH_MODE_MAX ) );

    // Cannot add draw calls while rendering.
    HELIUM_ASSERT( !m_bDrawing );

    // Don't buffer any drawing information if we have no renderer.
    if( !Renderer::GetStaticInstance() )
    {
        return;
    }

    uint32_t baseVertexIndex = static_cast< uint32_t >( m_untexturedVertices.GetSize() );
    uint32_t startIndex = static_cast< uint32_t >( m_untexturedIndices.GetSize() );

    m_untexturedVertices.AddArray( pVertices, vertexCount );
    m_untexturedIndices.AddArray( pIndices, triangleCount * 3 );

    UntexturedDrawCall* pDrawCall = m_wireMeshDrawCalls[ depthMode ].New();
    HELIUM_ASSERT( pDrawCall );
    pDrawCall->baseVertexIndex = baseVertexIndex;
    pDrawCall->vertexCount = vertexCount;
    pDrawCall->startIndex = startIndex;
    pDrawCall->primitiveCount = triangleCount;
}

/// Buffer a solid mesh draw call.
///
/// @param[in] pVertices      Vertices to use for drawing.
/// @param[in] vertexCount    Number of vertices used for drawing.
/// @param[in] pIndices       Indices to use for drawing.
/// @param[in] triangleCount  Number of triangles to draw.
/// @param[in] depthMode      Mode in which to handle depth testing and writing.
///
/// @see DrawLines(), DrawWireMesh(), DrawTexturedMesh()
void BufferedDrawer::DrawSolidMesh(
    const SimpleVertex* pVertices,
    uint32_t vertexCount,
    const uint16_t* pIndices,
    uint32_t triangleCount,
    EDepthMode depthMode )
{
    HELIUM_ASSERT( pVertices );
    HELIUM_ASSERT( vertexCount );
    HELIUM_ASSERT( pIndices );
    HELIUM_ASSERT( triangleCount );
    HELIUM_ASSERT( static_cast< size_t >( depthMode ) < static_cast< size_t >( DEPTH_MODE_MAX ) );

    // Cannot add draw calls while rendering.
    HELIUM_ASSERT( !m_bDrawing );

    // Don't buffer any drawing information if we have no renderer.
    if( !Renderer::GetStaticInstance() )
    {
        return;
    }

    uint32_t baseVertexIndex = static_cast< uint32_t >( m_untexturedVertices.GetSize() );
    uint32_t startIndex = static_cast< uint32_t >( m_untexturedIndices.GetSize() );

    m_untexturedVertices.AddArray( pVertices, vertexCount );
    m_untexturedIndices.AddArray( pIndices, triangleCount * 3 );

    UntexturedDrawCall* pDrawCall = m_solidMeshDrawCalls[ depthMode ].New();
    HELIUM_ASSERT( pDrawCall );
    pDrawCall->baseVertexIndex = baseVertexIndex;
    pDrawCall->vertexCount = vertexCount;
    pDrawCall->startIndex = startIndex;
    pDrawCall->primitiveCount = triangleCount;
}

/// Buffer a textured mesh draw call.
///
/// @param[in] pVertices      Vertices to use for drawing.
/// @param[in] vertexCount    Number of vertices used for drawing.
/// @param[in] pIndices       Indices to use for drawing.
/// @param[in] triangleCount  Number of triangles to draw.
/// @param[in] pTexture       Texture to apply to the mesh.
/// @param[in] depthMode      Mode in which to handle depth testing and writing.
///
/// @see DrawLines(), DrawWireMesh(), DrawSolidMesh()
void BufferedDrawer::DrawTexturedMesh(
    const SimpleTexturedVertex* pVertices,
    uint32_t vertexCount,
    const uint16_t* pIndices,
    uint32_t triangleCount,
    RTexture2d* pTexture,
    EDepthMode depthMode )
{
    HELIUM_ASSERT( pVertices );
    HELIUM_ASSERT( vertexCount );
    HELIUM_ASSERT( pIndices );
    HELIUM_ASSERT( triangleCount );
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
    uint32_t startIndex = static_cast< uint32_t >( m_texturedIndices.GetSize() );

    m_texturedVertices.AddArray( pVertices, vertexCount );
    m_texturedIndices.AddArray( pIndices, triangleCount * 3 );

    TexturedDrawCall* pDrawCall = m_texturedMeshDrawCalls[ depthMode ].New();
    HELIUM_ASSERT( pDrawCall );
    pDrawCall->baseVertexIndex = baseVertexIndex;
    pDrawCall->vertexCount = vertexCount;
    pDrawCall->startIndex = startIndex;
    pDrawCall->primitiveCount = triangleCount;
    pDrawCall->spTexture = pTexture;
}

/// Draw text in world space at a specific transform.
///
/// @param[in] rTransform  World transform at which to start the text.
/// @param[in] rText       Text to draw.
/// @param[in] rColor      Color to blend with the text.
/// @param[in] size        Identifier of the font size to use.
/// @param[in] depthMode   Mode in which to handle depth testing and writing.
///
/// @see DrawScreenText()
void BufferedDrawer::DrawWorldText(
    const Simd::Matrix44& rTransform,
    const String& rText,
    const Color& rColor,
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
    WorldSpaceTextGlyphHandler glyphHandler( this, pFont, rColor, depthMode, rTransform );
    pFont->ProcessText( rText, glyphHandler );
}

/// Draw text in screen space at a specific transform.
///
/// @param[in] x       X-coordinate of the screen pixel at which to begin drawing the text.
/// @param[in] y       Y-coordinate of the screen pixel at which to begin drawing the text.
/// @param[in] rText   Text to draw.
/// @param[in] rColor  Color to blend with the text.
/// @param[in] size    Identifier of the font size to use.
///
/// @see DrawWorldText()
void BufferedDrawer::DrawScreenText(
    int32_t x,
    int32_t y,
    const String& rText,
    const Color& rColor,
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
    ScreenSpaceTextGlyphHandler glyphHandler( this, pFont, x, y, rColor, size );
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
    m_untexturedVertices.Remove( 0, m_untexturedVertices.GetSize() );
    m_texturedVertices.Remove( 0, m_texturedVertices.GetSize() );
    m_untexturedIndices.Remove( 0, m_untexturedIndices.GetSize() );
    m_texturedIndices.Remove( 0, m_texturedIndices.GetSize() );
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

    // Clear all buffered draw call data and swap rendering resources for the next set of buffered draw calls.
    m_screenTextGlyphIndices.Remove( 0, m_screenTextGlyphIndices.GetSize() );
    m_screenTextDrawCalls.Remove( 0, m_screenTextDrawCalls.GetSize() );

    for( size_t depthModeIndex = 0; depthModeIndex < static_cast< size_t >( DEPTH_MODE_MAX ); ++depthModeIndex )
    {
        m_worldTextDrawCalls[ depthModeIndex ].Remove( 0, m_worldTextDrawCalls[ depthModeIndex ].GetSize() );
        m_texturedMeshDrawCalls[ depthModeIndex ].Remove( 0, m_texturedMeshDrawCalls[ depthModeIndex ].GetSize() );
        m_solidMeshDrawCalls[ depthModeIndex ].Remove( 0, m_solidMeshDrawCalls[ depthModeIndex ].GetSize() );
        m_wireMeshDrawCalls[ depthModeIndex ].Remove( 0, m_wireMeshDrawCalls[ depthModeIndex ].GetSize() );
        m_lineDrawCalls[ depthModeIndex ].Remove( 0, m_lineDrawCalls[ depthModeIndex ].GetSize() );
    }

    m_currentResourceSetIndex = ( m_currentResourceSetIndex + 1 ) % HELIUM_ARRAY_COUNT( m_resourceSets );
}

/// Issue draw commands for buffered development-mode draw calls in world space.
///
/// BeginDrawing() must be called before issuing calls to this function.  This function can be called multiple times
/// between a BeginDrawing() and EndDrawing() pair.
///
/// Special care should be taken with regards to the following:
/// - This function expects the proper global shader constant data (view/projection matrices) to be already set in
///   vertex constant buffer 0.
/// - The rasterizer, blend, and depth-stencil states may be altered when this function returns.
///
/// @see BeginDrawing(), EndDrawing(), DrawScreenElements()
void BufferedDrawer::DrawWorldElements()
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
    worldResources.bSetBlendState = false;

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

    // Draw textured meshes first.
    if( rResourceSet.spTexturedVertexBuffer && rResourceSet.spTexturedIndexBuffer )
    {
        const DynArray< TexturedDrawCall >& rTexturedMeshDrawCalls = m_texturedMeshDrawCalls[ depthMode ];
        const DynArray< TexturedDrawCall >& rWorldTextDrawCalls = m_worldTextDrawCalls[ depthMode ];
        size_t texturedMeshDrawCallCount = rTexturedMeshDrawCalls.GetSize();
        size_t textDrawCallCount = rWorldTextDrawCalls.GetSize();
        if( texturedMeshDrawCallCount | textDrawCallCount )
        {
            uint32_t vertexStride = static_cast< uint32_t >( sizeof( SimpleTexturedVertex ) );
            uint32_t vertexOffset = 0;
            pCommandProxy->SetVertexBuffers(
                0,
                1,
                &rResourceSet.spTexturedVertexBuffer,
                &vertexStride,
                &vertexOffset );

            pCommandProxy->SetIndexBuffer( rResourceSet.spTexturedIndexBuffer );

            if( texturedMeshDrawCallCount != 0 &&
                rWorldResources.spTextureBlendVertexShader && rWorldResources.spTextureBlendPixelShader )
            {
                pCommandProxy->SetVertexShader( rWorldResources.spTextureBlendVertexShader );
                pCommandProxy->SetPixelShader( rWorldResources.spTextureBlendPixelShader );

                rWorldResources.spTextureBlendVertexShader->CacheDescription(
                    pRenderer,
                    rWorldResources.spSimpleTexturedVertexDescription );
                RVertexInputLayout* pVertexInputLayout =
                    rWorldResources.spTextureBlendVertexShader->GetCachedInputLayout();
                HELIUM_ASSERT( pVertexInputLayout );
                pCommandProxy->SetVertexInputLayout( pVertexInputLayout );

                if( !rWorldResources.bSetBlendState )
                {
                    RBlendState* pBlendState = rRenderResourceManager.GetBlendState(
                        RenderResourceManager::BLEND_STATE_TRANSPARENT );
                    HELIUM_ASSERT( pBlendState );
                    pCommandProxy->SetBlendState( pBlendState );
                }

                RRasterizerState* pRasterizerState = rRenderResourceManager.GetRasterizerState(
                    RenderResourceManager::RASTERIZER_STATE_DEFAULT );
                pCommandProxy->SetRasterizerState( pRasterizerState );

                for( size_t drawCallIndex = 0; drawCallIndex < texturedMeshDrawCallCount; ++drawCallIndex )
                {
                    const TexturedDrawCall& rDrawCall = rTexturedMeshDrawCalls[ drawCallIndex ];

                    pCommandProxy->SetTexture( 0, rDrawCall.spTexture );

                    pCommandProxy->DrawIndexed(
                        RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST,
                        rDrawCall.baseVertexIndex,
                        0,
                        rDrawCall.vertexCount,
                        rDrawCall.startIndex,
                        rDrawCall.primitiveCount );
                }

                pCommandProxy->SetTexture( 0, NULL );
            }

            if( textDrawCallCount != 0 &&
                rWorldResources.spTextureAlphaVertexShader && rWorldResources.spTextureAlphaPixelShader )
            {
                pCommandProxy->SetVertexShader( rWorldResources.spTextureAlphaVertexShader );
                pCommandProxy->SetPixelShader( rWorldResources.spTextureAlphaPixelShader );

                rWorldResources.spTextureAlphaVertexShader->CacheDescription(
                    pRenderer,
                    rWorldResources.spSimpleTexturedVertexDescription );
                RVertexInputLayout* pVertexInputLayout =
                    rWorldResources.spTextureAlphaVertexShader->GetCachedInputLayout();
                HELIUM_ASSERT( pVertexInputLayout );
                pCommandProxy->SetVertexInputLayout( pVertexInputLayout );

                if( !rWorldResources.bSetBlendState )
                {
                    RBlendState* pBlendState = rRenderResourceManager.GetBlendState(
                        RenderResourceManager::BLEND_STATE_TRANSPARENT );
                    HELIUM_ASSERT( pBlendState );
                    pCommandProxy->SetBlendState( pBlendState );
                }

                RRasterizerState* pRasterizerState = rRenderResourceManager.GetRasterizerState(
                    RenderResourceManager::RASTERIZER_STATE_DEFAULT );
                pCommandProxy->SetRasterizerState( pRasterizerState );

                for( size_t drawCallIndex = 0; drawCallIndex < textDrawCallCount; ++drawCallIndex )
                {
                    const TexturedDrawCall& rDrawCall = rWorldTextDrawCalls[ drawCallIndex ];

                    pCommandProxy->SetTexture( 0, rDrawCall.spTexture );

                    pCommandProxy->DrawIndexed(
                        RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST,
                        rDrawCall.baseVertexIndex,
                        0,
                        rDrawCall.vertexCount,
                        rDrawCall.startIndex,
                        rDrawCall.primitiveCount );
                }

                pCommandProxy->SetTexture( 0, NULL );
            }
        }
    }

    // Draw untextured data.
    if( rWorldResources.spUntexturedVertexShader && rWorldResources.spUntexturedPixelShader &&
        rResourceSet.spUntexturedVertexBuffer && rResourceSet.spUntexturedIndexBuffer )
    {
        const DynArray< UntexturedDrawCall >& rSolidMeshDrawCalls = m_solidMeshDrawCalls[ depthMode ];
        const DynArray< UntexturedDrawCall >& rWireMeshDrawCalls = m_wireMeshDrawCalls[ depthMode ];
        const DynArray< UntexturedDrawCall >& rLineDrawCalls = m_lineDrawCalls[ depthMode ];
        size_t solidMeshDrawCallCount = rSolidMeshDrawCalls.GetSize();
        size_t wireMeshDrawCallCount = rWireMeshDrawCalls.GetSize();
        size_t lineDrawCallCount = rLineDrawCalls.GetSize();
        if( ( solidMeshDrawCallCount | wireMeshDrawCallCount | lineDrawCallCount ) != 0 )
        {
            pCommandProxy->SetVertexShader( rWorldResources.spUntexturedVertexShader );
            pCommandProxy->SetPixelShader( rWorldResources.spUntexturedPixelShader );

            uint32_t vertexStride = static_cast< uint32_t >( sizeof( SimpleVertex ) );
            uint32_t vertexOffset = 0;
            pCommandProxy->SetVertexBuffers(
                0,
                1,
                &rResourceSet.spUntexturedVertexBuffer,
                &vertexStride,
                &vertexOffset );
            pCommandProxy->SetIndexBuffer( rResourceSet.spUntexturedIndexBuffer );

            rWorldResources.spUntexturedVertexShader->CacheDescription(
                pRenderer,
                rWorldResources.spSimpleVertexDescription );
            RVertexInputLayout* pVertexInputLayout = rWorldResources.spUntexturedVertexShader->GetCachedInputLayout();
            HELIUM_ASSERT( pVertexInputLayout );
            pCommandProxy->SetVertexInputLayout( pVertexInputLayout );

            if( !rWorldResources.bSetBlendState )
            {
                RBlendState* pBlendState = rRenderResourceManager.GetBlendState(
                    RenderResourceManager::BLEND_STATE_TRANSPARENT );
                HELIUM_ASSERT( pBlendState );
                pCommandProxy->SetBlendState( pBlendState );
            }

            RRasterizerState* pRasterizerState = rRenderResourceManager.GetRasterizerState(
                RenderResourceManager::RASTERIZER_STATE_DEFAULT );
            pCommandProxy->SetRasterizerState( pRasterizerState );

            for( size_t drawCallIndex = 0; drawCallIndex < solidMeshDrawCallCount; ++drawCallIndex )
            {
                const UntexturedDrawCall& rDrawCall = rSolidMeshDrawCalls[ drawCallIndex ];

                pCommandProxy->DrawIndexed(
                    RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST,
                    rDrawCall.baseVertexIndex,
                    0,
                    rDrawCall.vertexCount,
                    rDrawCall.startIndex,
                    rDrawCall.primitiveCount );
            }

            pRasterizerState = rRenderResourceManager.GetRasterizerState(
                RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
            pCommandProxy->SetRasterizerState( pRasterizerState );

            for( size_t drawCallIndex = 0; drawCallIndex < wireMeshDrawCallCount; ++drawCallIndex )
            {
                const UntexturedDrawCall& rDrawCall = rWireMeshDrawCalls[ drawCallIndex ];

                pCommandProxy->DrawIndexed(
                    RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST,
                    rDrawCall.baseVertexIndex,
                    0,
                    rDrawCall.vertexCount,
                    rDrawCall.startIndex,
                    rDrawCall.primitiveCount );
            }

            for( size_t drawCallIndex = 0; drawCallIndex < lineDrawCallCount; ++drawCallIndex )
            {
                const UntexturedDrawCall& rDrawCall = rLineDrawCalls[ drawCallIndex ];

                pCommandProxy->DrawIndexed(
                    RENDERER_PRIMITIVE_TYPE_LINE_LIST,
                    rDrawCall.baseVertexIndex,
                    0,
                    rDrawCall.vertexCount,
                    rDrawCall.startIndex,
                    rDrawCall.primitiveCount );
            }
        }
    }
}

/// Constructor.
///
/// @param[in] pDrawer     Buffered drawer instance being used to perform the rendering.
/// @param[in] pFont       Font being used for rendering.
/// @param[in] rColor      Text color.
/// @param[in] depthMode   Mode in which to handle depth testing and writing.
/// @param[in] rTransform  World-space transform matrix.
BufferedDrawer::WorldSpaceTextGlyphHandler::WorldSpaceTextGlyphHandler(
    BufferedDrawer* pDrawer,
    Font* pFont,
    const Color& rColor,
    EDepthMode depthMode,
    const Simd::Matrix44& rTransform )
    : m_rTransform( rTransform )
    , m_pDrawer( pDrawer )
    , m_pFont( pFont )
    , m_color( rColor )
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
    pDrawCall->baseVertexIndex = baseVertexIndex;
    pDrawCall->vertexCount = 4;
    pDrawCall->startIndex = startIndex;
    pDrawCall->primitiveCount = 2;
    pDrawCall->spTexture = pTexture;

    m_penX += Font::Fixed26x6ToFloat32( pCharacter->advance );
}

/// Constructor.
///
/// @param[in] pDrawer  Buffered drawer instance being used to perform the rendering.
/// @param[in] pFont    Font being used for rendering.
/// @param[in] x        Pixel x-coordinate at which to begin rendering the text.
/// @param[in] y        Pixel y-coordinate at which to begin rendering the text.
/// @param[in] rColor   Color with which to render the text.
/// @param[in] size     Size at which to render the text.
BufferedDrawer::ScreenSpaceTextGlyphHandler::ScreenSpaceTextGlyphHandler(
    BufferedDrawer* pDrawer,
    Font* pFont,
    int32_t x,
    int32_t y,
    const Color& rColor,
    RenderResourceManager::EDebugFontSize size )
    : m_pDrawer( pDrawer )
    , m_pFont( pFont )
    , m_pDrawCall( NULL )
    , m_x( x )
    , m_y( y )
    , m_color( rColor )
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
