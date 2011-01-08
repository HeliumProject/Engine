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

namespace Lunar
{
    L_DECLARE_RPTR( RRenderCommandProxy );
}

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
        rResourceSet.textureBlendVertexBufferSize = 0;
        rResourceSet.textureBlendIndexBufferSize = 0;
        rResourceSet.textureAlphaVertexBufferSize = 0;
        rResourceSet.textureAlphaIndexBufferSize = 0;
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
    m_textureBlendVertices.Clear();
    m_textureAlphaVertices.Clear();

    m_untexturedIndices.Clear();
    m_textureBlendIndices.Clear();
    m_textureAlphaIndices.Clear();

    m_lineDrawCalls.Clear();
    m_wireMeshDrawCalls.Clear();
    m_solidMeshDrawCalls.Clear();
    m_texturedMeshDrawCalls.Clear();
    m_worldTextDrawCalls.Clear();

    m_screenTextDrawCalls.Clear();
    m_screenTextGlyphIndices.Clear();

    m_spScreenSpaceTextIndexBuffer.Release();

    for( size_t resourceSetIndex = 0; resourceSetIndex < HELIUM_ARRAY_COUNT( m_resourceSets ); ++resourceSetIndex )
    {
        ResourceSet& rResourceSet = m_resourceSets[ resourceSetIndex ];
        rResourceSet.spUntexturedVertexBuffer.Release();
        rResourceSet.spUntexturedIndexBuffer.Release();
        rResourceSet.spTextureBlendVertexBuffer.Release();
        rResourceSet.spTextureBlendIndexBuffer.Release();
        rResourceSet.spTextureAlphaVertexBuffer.Release();
        rResourceSet.spTextureAlphaIndexBuffer.Release();
        rResourceSet.spScreenSpaceTextVertexBuffer.Release();
        rResourceSet.untexturedVertexBufferSize = 0;
        rResourceSet.untexturedIndexBufferSize = 0;
        rResourceSet.textureBlendVertexBufferSize = 0;
        rResourceSet.textureBlendIndexBufferSize = 0;
        rResourceSet.textureAlphaVertexBufferSize = 0;
        rResourceSet.textureAlphaIndexBufferSize = 0;
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
///
/// @see DrawWireMesh(), DrawSolidMesh(), DrawTexturedMesh()
void BufferedDrawer::DrawLines(
    const SimpleVertex* pVertices,
    uint32_t vertexCount,
    const uint16_t* pIndices,
    uint32_t lineCount )
{
    HELIUM_ASSERT( pVertices );
    HELIUM_ASSERT( vertexCount );
    HELIUM_ASSERT( pIndices );
    HELIUM_ASSERT( lineCount );

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

    UntexturedDrawCall* pDrawCall = m_lineDrawCalls.New();
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
///
/// @see DrawLines(), DrawSolidMesh(), DrawTexturedMesh()
void BufferedDrawer::DrawWireMesh(
    const SimpleVertex* pVertices,
    uint32_t vertexCount,
    const uint16_t* pIndices,
    uint32_t triangleCount )
{
    HELIUM_ASSERT( pVertices );
    HELIUM_ASSERT( vertexCount );
    HELIUM_ASSERT( pIndices );
    HELIUM_ASSERT( triangleCount );

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

    UntexturedDrawCall* pDrawCall = m_wireMeshDrawCalls.New();
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
///
/// @see DrawLines(), DrawWireMesh(), DrawTexturedMesh()
void BufferedDrawer::DrawSolidMesh(
    const SimpleVertex* pVertices,
    uint32_t vertexCount,
    const uint16_t* pIndices,
    uint32_t triangleCount )
{
    HELIUM_ASSERT( pVertices );
    HELIUM_ASSERT( vertexCount );
    HELIUM_ASSERT( pIndices );
    HELIUM_ASSERT( triangleCount );

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

    UntexturedDrawCall* pDrawCall = m_solidMeshDrawCalls.New();
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
///
/// @see DrawLines(), DrawWireMesh(), DrawSolidMesh()
void BufferedDrawer::DrawTexturedMesh(
    const SimpleTexturedVertex* pVertices,
    uint32_t vertexCount,
    const uint16_t* pIndices,
    uint32_t triangleCount,
    RTexture2d* pTexture )
{
    HELIUM_ASSERT( pVertices );
    HELIUM_ASSERT( vertexCount );
    HELIUM_ASSERT( pIndices );
    HELIUM_ASSERT( triangleCount );
    HELIUM_ASSERT( pTexture );

    // Cannot add draw calls while rendering.
    HELIUM_ASSERT( !m_bDrawing );

    // Don't buffer any drawing information if we have no renderer.
    if( !Renderer::GetStaticInstance() )
    {
        return;
    }

    uint32_t baseVertexIndex = static_cast< uint32_t >( m_textureBlendVertices.GetSize() );
    uint32_t startIndex = static_cast< uint32_t >( m_textureBlendIndices.GetSize() );

    m_textureBlendVertices.AddArray( pVertices, vertexCount );
    m_textureBlendIndices.AddArray( pIndices, triangleCount * 3 );

    TexturedDrawCall* pDrawCall = m_texturedMeshDrawCalls.New();
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
///
/// @see DrawScreenText()
void BufferedDrawer::DrawWorldText(
    const Simd::Matrix44& rTransform,
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

    // Render the text.
    WorldSpaceTextGlyphHandler glyphHandler( this, pFont, rColor, rTransform );
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
        HELIUM_ASSERT( m_textureBlendVertices.IsEmpty() );
        HELIUM_ASSERT( m_textureBlendIndices.IsEmpty() );
        HELIUM_ASSERT( m_textureAlphaVertices.IsEmpty() );
        HELIUM_ASSERT( m_textureAlphaIndices.IsEmpty() );
        HELIUM_ASSERT( m_screenTextGlyphIndices.IsEmpty() );

        return;
    }

    // Prepare the vertex and index buffers with the buffered data.
    ResourceSet& rResourceSet = m_resourceSets[ m_currentResourceSetIndex ];

    uint_fast32_t untexturedVertexCount = static_cast< uint_fast32_t >( m_untexturedVertices.GetSize() );
    uint_fast32_t untexturedIndexCount = static_cast< uint_fast32_t >( m_untexturedIndices.GetSize() );
    uint_fast32_t textureBlendVertexCount = static_cast< uint_fast32_t >( m_textureBlendVertices.GetSize() );
    uint_fast32_t textureBlendIndexCount = static_cast< uint_fast32_t >( m_textureBlendIndices.GetSize() );
    uint_fast32_t textureAlphaVertexCount = static_cast< uint_fast32_t >( m_textureAlphaVertices.GetSize() );
    uint_fast32_t textureAlphaIndexCount = static_cast< uint_fast32_t >( m_textureAlphaIndices.GetSize() );

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

    if( textureBlendVertexCount > rResourceSet.textureBlendVertexBufferSize )
    {
        rResourceSet.spTextureBlendVertexBuffer.Release();
        rResourceSet.spTextureBlendVertexBuffer = pRenderer->CreateVertexBuffer(
            textureBlendVertexCount * sizeof( SimpleTexturedVertex ),
            RENDERER_BUFFER_USAGE_DYNAMIC );
        if( !rResourceSet.spTextureBlendVertexBuffer )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "Failed to create vertex buffer for blended texture debug drawing of %" ) TPRIuFAST32
                  TXT( " vertices.\n" ) ),
                textureBlendVertexCount );

            rResourceSet.textureBlendVertexBufferSize = 0;
        }
        else
        {
            rResourceSet.textureBlendVertexBufferSize = static_cast< uint32_t >( textureBlendVertexCount );
        }
    }

    if( textureBlendIndexCount > rResourceSet.textureBlendIndexBufferSize )
    {
        rResourceSet.spTextureBlendIndexBuffer.Release();
        rResourceSet.spTextureBlendIndexBuffer = pRenderer->CreateIndexBuffer(
            textureBlendIndexCount * sizeof( uint16_t ),
            RENDERER_BUFFER_USAGE_DYNAMIC,
            RENDERER_INDEX_FORMAT_UINT16 );
        if( !rResourceSet.spTextureBlendIndexBuffer )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "Failed to create index buffer for blended texture debug drawing of %" ) TPRIuFAST32
                  TXT( " indices.\n" ) ),
                textureBlendIndexCount );

            rResourceSet.textureBlendIndexBufferSize = 0;
        }
        else
        {
            rResourceSet.textureBlendIndexBufferSize = static_cast< uint32_t >( textureBlendIndexCount );
        }
    }

    if( textureAlphaVertexCount > rResourceSet.textureAlphaVertexBufferSize )
    {
        rResourceSet.spTextureAlphaVertexBuffer.Release();
        rResourceSet.spTextureAlphaVertexBuffer = pRenderer->CreateVertexBuffer(
            textureAlphaVertexCount * sizeof( SimpleTexturedVertex ),
            RENDERER_BUFFER_USAGE_DYNAMIC );
        if( !rResourceSet.spTextureAlphaVertexBuffer )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "Failed to create vertex buffer for alpha texture debug drawing of %" ) TPRIuFAST32
                  TXT( " vertices.\n" ) ),
                textureAlphaVertexCount );

            rResourceSet.textureAlphaVertexBufferSize = 0;
        }
        else
        {
            rResourceSet.textureAlphaVertexBufferSize = static_cast< uint32_t >( textureAlphaVertexCount );
        }
    }

    if( textureAlphaIndexCount > rResourceSet.textureAlphaIndexBufferSize )
    {
        rResourceSet.spTextureAlphaIndexBuffer.Release();
        rResourceSet.spTextureAlphaIndexBuffer = pRenderer->CreateIndexBuffer(
            textureAlphaIndexCount * sizeof( uint16_t ),
            RENDERER_BUFFER_USAGE_DYNAMIC,
            RENDERER_INDEX_FORMAT_UINT16 );
        if( !rResourceSet.spTextureAlphaIndexBuffer )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "Failed to create index buffer for blended texture debug drawing of %" ) TPRIuFAST32
                  TXT( " indices.\n" ) ),
                textureAlphaIndexCount );

            rResourceSet.textureAlphaIndexBufferSize = 0;
        }
        else
        {
            rResourceSet.textureAlphaIndexBufferSize = static_cast< uint32_t >( textureAlphaIndexCount );
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

    if( textureBlendVertexCount && textureBlendIndexCount &&
        rResourceSet.spTextureBlendVertexBuffer && rResourceSet.spTextureBlendIndexBuffer )
    {
        void* pMappedVertexBuffer = rResourceSet.spTextureBlendVertexBuffer->Map( RENDERER_BUFFER_MAP_HINT_DISCARD );
        HELIUM_ASSERT( pMappedVertexBuffer );
        MemoryCopy(
            pMappedVertexBuffer,
            m_textureBlendVertices.GetData(),
            textureBlendVertexCount * sizeof( SimpleTexturedVertex ) );
        rResourceSet.spTextureBlendVertexBuffer->Unmap();

        void* pMappedIndexBuffer = rResourceSet.spTextureBlendIndexBuffer->Map( RENDERER_BUFFER_MAP_HINT_DISCARD );
        HELIUM_ASSERT( pMappedIndexBuffer );
        MemoryCopy(
            pMappedIndexBuffer,
            m_textureBlendIndices.GetData(),
            textureBlendIndexCount * sizeof( uint16_t ) );
        rResourceSet.spTextureBlendIndexBuffer->Unmap();
    }

    if( textureAlphaVertexCount && textureAlphaIndexCount &&
        rResourceSet.spTextureAlphaVertexBuffer && rResourceSet.spTextureAlphaIndexBuffer )
    {
        void* pMappedVertexBuffer = rResourceSet.spTextureAlphaVertexBuffer->Map( RENDERER_BUFFER_MAP_HINT_DISCARD );
        HELIUM_ASSERT( pMappedVertexBuffer );
        MemoryCopy(
            pMappedVertexBuffer,
            m_textureAlphaVertices.GetData(),
            textureAlphaVertexCount * sizeof( SimpleTexturedVertex ) );
        rResourceSet.spTextureAlphaVertexBuffer->Unmap();

        void* pMappedIndexBuffer = rResourceSet.spTextureAlphaIndexBuffer->Map( RENDERER_BUFFER_MAP_HINT_DISCARD );
        HELIUM_ASSERT( pMappedIndexBuffer );
        MemoryCopy(
            pMappedIndexBuffer,
            m_textureAlphaIndices.GetData(),
            textureAlphaIndexCount * sizeof( uint16_t ) );
        rResourceSet.spTextureAlphaIndexBuffer->Unmap();
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
                    pScreenVertices->color = color.GetArgb();
                    pScreenVertices->texCoords[ 0 ] = texCoordMinX;
                    pScreenVertices->texCoords[ 1 ] = texCoordMinY;
                    ++pScreenVertices;

                    pScreenVertices->position[ 0 ] = cornerMaxX;
                    pScreenVertices->position[ 1 ] = cornerMinY;
                    pScreenVertices->color = color.GetArgb();
                    pScreenVertices->texCoords[ 0 ] = texCoordMaxX;
                    pScreenVertices->texCoords[ 1 ] = texCoordMinY;
                    ++pScreenVertices;

                    pScreenVertices->position[ 0 ] = cornerMaxX;
                    pScreenVertices->position[ 1 ] = cornerMaxY;
                    pScreenVertices->color = color.GetArgb();
                    pScreenVertices->texCoords[ 0 ] = texCoordMaxX;
                    pScreenVertices->texCoords[ 1 ] = texCoordMaxY;
                    ++pScreenVertices;

                    pScreenVertices->position[ 0 ] = cornerMinX;
                    pScreenVertices->position[ 1 ] = cornerMaxY;
                    pScreenVertices->color = color.GetArgb();
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
    m_textureBlendVertices.Remove( 0, m_textureBlendVertices.GetSize() );
    m_textureAlphaVertices.Remove( 0, m_textureAlphaVertices.GetSize() );
    m_untexturedIndices.Remove( 0, m_untexturedIndices.GetSize() );
    m_textureBlendIndices.Remove( 0, m_textureBlendIndices.GetSize() );
    m_textureAlphaIndices.Remove( 0, m_textureAlphaIndices.GetSize() );
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
        HELIUM_ASSERT( m_textureBlendVertices.IsEmpty() );
        HELIUM_ASSERT( m_textureBlendIndices.IsEmpty() );
        HELIUM_ASSERT( m_textureAlphaVertices.IsEmpty() );
        HELIUM_ASSERT( m_textureAlphaIndices.IsEmpty() );
        HELIUM_ASSERT( m_screenTextGlyphIndices.IsEmpty() );

        return;
    }

    // Clear all buffered draw call data and swap rendering resources for the next set of buffered draw calls.
    m_screenTextGlyphIndices.Remove( 0, m_screenTextGlyphIndices.GetSize() );
    m_screenTextDrawCalls.Remove( 0, m_screenTextDrawCalls.GetSize() );

    m_worldTextDrawCalls.Remove( 0, m_worldTextDrawCalls.GetSize() );
    m_texturedMeshDrawCalls.Remove( 0, m_texturedMeshDrawCalls.GetSize() );
    m_solidMeshDrawCalls.Remove( 0, m_solidMeshDrawCalls.GetSize() );
    m_wireMeshDrawCalls.Remove( 0, m_wireMeshDrawCalls.GetSize() );
    m_lineDrawCalls.Remove( 0, m_lineDrawCalls.GetSize() );

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
/// - The rasterizer and blend states may be altered when this function returns.
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
        HELIUM_ASSERT( m_textureBlendVertices.IsEmpty() );
        HELIUM_ASSERT( m_textureBlendIndices.IsEmpty() );
        HELIUM_ASSERT( m_textureAlphaVertices.IsEmpty() );
        HELIUM_ASSERT( m_textureAlphaIndices.IsEmpty() );

        return;
    }

    RRenderCommandProxyPtr spCommandProxy = pRenderer->GetImmediateCommandProxy();

    ResourceSet& rResourceSet = m_resourceSets[ m_currentResourceSetIndex ];

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

    Shader* pShader = StaticCast< Shader >( pVertexShaderVariant->GetOwner() );
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
    RVertexShaderPtr spUntexturedVertexShader = static_cast< RVertexShader* >( pShaderResource );

    optionSetIndex = rSystemOptions.GetOptionSetIndex(
        RShader::TYPE_PIXEL,
        NULL,
        0,
        untexturedSelectOptions,
        HELIUM_ARRAY_COUNT( untexturedSelectOptions ) );
    pShaderResource = pPixelShaderVariant->GetRenderResource( optionSetIndex );
    HELIUM_ASSERT( !pShaderResource || pShaderResource->GetType() == RShader::TYPE_PIXEL );
    RPixelShaderPtr spUntexturedPixelShader = static_cast< RPixelShader* >( pShaderResource );

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
    RVertexShaderPtr spTextureBlendVertexShader = static_cast< RVertexShader* >( pShaderResource );

    optionSetIndex = rSystemOptions.GetOptionSetIndex(
        RShader::TYPE_PIXEL,
        NULL,
        0,
        textureBlendSelectOptions,
        HELIUM_ARRAY_COUNT( textureBlendSelectOptions ) );
    pShaderResource = pPixelShaderVariant->GetRenderResource( optionSetIndex );
    HELIUM_ASSERT( !pShaderResource || pShaderResource->GetType() == RShader::TYPE_PIXEL );
    RPixelShaderPtr spTextureBlendPixelShader = static_cast< RPixelShader* >( pShaderResource );

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
    RVertexShaderPtr spTextureAlphaVertexShader = static_cast< RVertexShader* >( pShaderResource );

    optionSetIndex = rSystemOptions.GetOptionSetIndex(
        RShader::TYPE_PIXEL,
        NULL,
        0,
        textureAlphaSelectOptions,
        HELIUM_ARRAY_COUNT( textureAlphaSelectOptions ) );
    pShaderResource = pPixelShaderVariant->GetRenderResource( optionSetIndex );
    HELIUM_ASSERT( !pShaderResource || pShaderResource->GetType() == RShader::TYPE_PIXEL );
    RPixelShaderPtr spTextureAlphaPixelShader = static_cast< RPixelShader* >( pShaderResource );

    // Get the vertex description resources for the untextured and textured vertex types.
    RVertexDescriptionPtr spUntexturedVertexDescription = rRenderResourceManager.GetSimpleVertexDescription();
    HELIUM_ASSERT( spUntexturedVertexDescription );

    RVertexDescriptionPtr spTexturedVertexDescription = rRenderResourceManager.GetSimpleTexturedVertexDescription();
    HELIUM_ASSERT( spTexturedVertexDescription );

    // Get the transparent blend state object.
    RBlendState* pBlendState = rRenderResourceManager.GetBlendState( RenderResourceManager::BLEND_STATE_TRANSPARENT );
    bool bSetBlendState = false;

    // Draw textured meshes first.
    if( spTextureBlendVertexShader && spTextureBlendPixelShader &&
        rResourceSet.spTextureBlendVertexBuffer && rResourceSet.spTextureBlendIndexBuffer )
    {
        size_t texturedMeshDrawCallCount = m_texturedMeshDrawCalls.GetSize();
        if( texturedMeshDrawCallCount != 0 )
        {
            spCommandProxy->SetVertexShader( spTextureBlendVertexShader );
            spCommandProxy->SetPixelShader( spTextureBlendPixelShader );

            uint32_t vertexStride = static_cast< uint32_t >( sizeof( SimpleTexturedVertex ) );
            uint32_t vertexOffset = 0;
            spCommandProxy->SetVertexBuffers(
                0,
                1,
                &rResourceSet.spTextureBlendVertexBuffer,
                &vertexStride,
                &vertexOffset );
            spCommandProxy->SetIndexBuffer( rResourceSet.spTextureBlendIndexBuffer );

            spTextureBlendVertexShader->CacheDescription( pRenderer, spTexturedVertexDescription );
            RVertexInputLayout* pVertexInputLayout = spTextureBlendVertexShader->GetCachedInputLayout();
            HELIUM_ASSERT( pVertexInputLayout );
            spCommandProxy->SetVertexInputLayout( pVertexInputLayout );

            if( !bSetBlendState )
            {
                spCommandProxy->SetBlendState( pBlendState );
            }

            RRasterizerState* pRasterizerState = rRenderResourceManager.GetRasterizerState(
                RenderResourceManager::RASTERIZER_STATE_DEFAULT );
            spCommandProxy->SetRasterizerState( pRasterizerState );

            for( size_t drawCallIndex = 0; drawCallIndex < texturedMeshDrawCallCount; ++drawCallIndex )
            {
                TexturedDrawCall& rDrawCall = m_texturedMeshDrawCalls[ drawCallIndex ];

                spCommandProxy->SetTexture( 0, rDrawCall.spTexture );

                spCommandProxy->DrawIndexed(
                    RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST,
                    rDrawCall.baseVertexIndex,
                    0,
                    rDrawCall.vertexCount,
                    rDrawCall.startIndex,
                    rDrawCall.primitiveCount );
            }

            spCommandProxy->SetTexture( 0, NULL );
        }
    }

    if( spTextureAlphaVertexShader && spTextureAlphaPixelShader &&
        rResourceSet.spTextureAlphaVertexBuffer && rResourceSet.spTextureAlphaIndexBuffer )
    {
        size_t textDrawCallCount = m_worldTextDrawCalls.GetSize();
        if( textDrawCallCount != 0 )
        {
            spCommandProxy->SetVertexShader( spTextureAlphaVertexShader );
            spCommandProxy->SetPixelShader( spTextureAlphaPixelShader );

            uint32_t vertexStride = static_cast< uint32_t >( sizeof( SimpleTexturedVertex ) );
            uint32_t vertexOffset = 0;
            spCommandProxy->SetVertexBuffers(
                0,
                1,
                &rResourceSet.spTextureAlphaVertexBuffer,
                &vertexStride,
                &vertexOffset );
            spCommandProxy->SetIndexBuffer( rResourceSet.spTextureAlphaIndexBuffer );

            spTextureAlphaVertexShader->CacheDescription( pRenderer, spTexturedVertexDescription );
            RVertexInputLayout* pVertexInputLayout = spTextureAlphaVertexShader->GetCachedInputLayout();
            HELIUM_ASSERT( pVertexInputLayout );
            spCommandProxy->SetVertexInputLayout( pVertexInputLayout );

            if( !bSetBlendState )
            {
                spCommandProxy->SetBlendState( pBlendState );
            }

            RRasterizerState* pRasterizerState = rRenderResourceManager.GetRasterizerState(
                RenderResourceManager::RASTERIZER_STATE_DEFAULT );
            spCommandProxy->SetRasterizerState( pRasterizerState );

            for( size_t drawCallIndex = 0; drawCallIndex < textDrawCallCount; ++drawCallIndex )
            {
                TexturedDrawCall& rDrawCall = m_worldTextDrawCalls[ drawCallIndex ];

                spCommandProxy->SetTexture( 0, rDrawCall.spTexture );

                spCommandProxy->DrawIndexed(
                    RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST,
                    rDrawCall.baseVertexIndex,
                    0,
                    rDrawCall.vertexCount,
                    rDrawCall.startIndex,
                    rDrawCall.primitiveCount );
            }

            spCommandProxy->SetTexture( 0, NULL );
        }
    }

    // Draw untextured data.
    if( spUntexturedVertexShader && spUntexturedPixelShader &&
        rResourceSet.spUntexturedVertexBuffer && rResourceSet.spUntexturedIndexBuffer )
    {
        size_t solidMeshDrawCallCount = m_solidMeshDrawCalls.GetSize();
        size_t wireMeshDrawCallCount = m_wireMeshDrawCalls.GetSize();
        size_t lineDrawCallCount = m_lineDrawCalls.GetSize();
        if( ( solidMeshDrawCallCount | wireMeshDrawCallCount | lineDrawCallCount ) != 0 )
        {
            spCommandProxy->SetVertexShader( spUntexturedVertexShader );
            spCommandProxy->SetPixelShader( spUntexturedPixelShader );

            uint32_t vertexStride = static_cast< uint32_t >( sizeof( SimpleVertex ) );
            uint32_t vertexOffset = 0;
            spCommandProxy->SetVertexBuffers(
                0,
                1,
                &rResourceSet.spUntexturedVertexBuffer,
                &vertexStride,
                &vertexOffset );
            spCommandProxy->SetIndexBuffer( rResourceSet.spUntexturedIndexBuffer );

            spUntexturedVertexShader->CacheDescription( pRenderer, spUntexturedVertexDescription );
            RVertexInputLayout* pVertexInputLayout = spUntexturedVertexShader->GetCachedInputLayout();
            HELIUM_ASSERT( pVertexInputLayout );
            spCommandProxy->SetVertexInputLayout( pVertexInputLayout );

            if( !bSetBlendState )
            {
                spCommandProxy->SetBlendState( pBlendState );
            }

            RRasterizerState* pRasterizerState = rRenderResourceManager.GetRasterizerState(
                RenderResourceManager::RASTERIZER_STATE_DEFAULT );
            spCommandProxy->SetRasterizerState( pRasterizerState );

            for( size_t drawCallIndex = 0; drawCallIndex < solidMeshDrawCallCount; ++drawCallIndex )
            {
                UntexturedDrawCall& rDrawCall = m_solidMeshDrawCalls[ drawCallIndex ];

                spCommandProxy->DrawIndexed(
                    RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST,
                    rDrawCall.baseVertexIndex,
                    0,
                    rDrawCall.vertexCount,
                    rDrawCall.startIndex,
                    rDrawCall.primitiveCount );
            }

            pRasterizerState = rRenderResourceManager.GetRasterizerState(
                RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
            spCommandProxy->SetRasterizerState( pRasterizerState );

            for( size_t drawCallIndex = 0; drawCallIndex < wireMeshDrawCallCount; ++drawCallIndex )
            {
                UntexturedDrawCall& rDrawCall = m_wireMeshDrawCalls[ drawCallIndex ];

                spCommandProxy->DrawIndexed(
                    RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST,
                    rDrawCall.baseVertexIndex,
                    0,
                    rDrawCall.vertexCount,
                    rDrawCall.startIndex,
                    rDrawCall.primitiveCount );
            }

            for( size_t drawCallIndex = 0; drawCallIndex < lineDrawCallCount; ++drawCallIndex )
            {
                UntexturedDrawCall& rDrawCall = m_lineDrawCalls[ drawCallIndex ];

                spCommandProxy->DrawIndexed(
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
        HELIUM_ASSERT( m_textureBlendVertices.IsEmpty() );
        HELIUM_ASSERT( m_textureBlendIndices.IsEmpty() );
        HELIUM_ASSERT( m_textureAlphaVertices.IsEmpty() );
        HELIUM_ASSERT( m_textureAlphaIndices.IsEmpty() );

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

/// Constructor.
///
/// @param[in] pDrawer     Buffered drawer instance being used to perform the rendering.
/// @param[in] pFont       Font being used for rendering.
/// @param[in] rColor      Text color.
/// @param[in] rTransform  World-space transform matrix.
BufferedDrawer::WorldSpaceTextGlyphHandler::WorldSpaceTextGlyphHandler(
    BufferedDrawer* pDrawer,
    Font* pFont,
    const Color& rColor,
    const Simd::Matrix44& rTransform )
    : m_rTransform( rTransform )
    , m_pDrawer( pDrawer )
    , m_pFont( pFont )
    , m_color( rColor )
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

    uint32_t baseVertexIndex = static_cast< uint32_t >( m_pDrawer->m_textureAlphaVertices.GetSize() );
    uint32_t startIndex = static_cast< uint32_t >( m_pDrawer->m_textureAlphaIndices.GetSize() );

    m_pDrawer->m_textureAlphaVertices.AddArray( vertices, 4 );
    m_pDrawer->m_textureAlphaIndices.AddArray( m_quadIndices, 6 );

    TexturedDrawCall* pDrawCall = m_pDrawer->m_worldTextDrawCalls.New();
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
