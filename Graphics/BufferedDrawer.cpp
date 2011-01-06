#include "GraphicsPch.h"
#include "Graphics/BufferedDrawer.h"

#include "Platform/Math/Simd/Matrix44.h"
#include "Foundation/StringConverter.h"
#include "Rendering/Renderer.h"
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
        rResourceSet.texturedVertexBufferSize = 0;
        rResourceSet.texturedIndexBufferSize = 0;
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

    // Currently, we don't allocate anything up front, so we're done here.
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

    m_lineDrawCalls.Clear();
    m_wireMeshDrawCalls.Clear();
    m_solidMeshDrawCalls.Clear();
    m_texturedMeshDrawCalls.Clear();

    for( size_t resourceSetIndex = 0; resourceSetIndex < HELIUM_ARRAY_COUNT( m_resourceSets ); ++resourceSetIndex )
    {
        ResourceSet& rResourceSet = m_resourceSets[ resourceSetIndex ];
        rResourceSet.spUntexturedVertexBuffer.Release();
        rResourceSet.spUntexturedIndexBuffer.Release();
        rResourceSet.spTexturedVertexBuffer.Release();
        rResourceSet.spTexturedIndexBuffer.Release();
        rResourceSet.untexturedVertexBufferSize = 0;
        rResourceSet.untexturedIndexBufferSize = 0;
        rResourceSet.texturedVertexBufferSize = 0;
        rResourceSet.texturedIndexBufferSize = 0;
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

    uint32_t baseVertexIndex = static_cast< uint32_t >( m_texturedVertices.GetSize() );
    uint32_t startIndex = static_cast< uint32_t >( m_texturedIndices.GetSize() );

    m_texturedVertices.AddArray( pVertices, vertexCount );
    m_texturedIndices.AddArray( pIndices, triangleCount * 3 );

    TexturedDrawCall* pDrawCall = m_texturedMeshDrawCalls.New();
    HELIUM_ASSERT( pDrawCall );
    pDrawCall->baseVertexIndex = baseVertexIndex;
    pDrawCall->vertexCount = vertexCount;
    pDrawCall->startIndex = startIndex;
    pDrawCall->primitiveCount = triangleCount;
    pDrawCall->spTexture = pTexture;
}

/// Draw text at a specific world transform.
///
/// @param[in] rTransform  World transform at which to start the text.
/// @param[in] rText       Text to draw.
/// @param[in] rColor      Color to blend with the text.
/// @param[in] size        Identifier of the font size to use.
void BufferedDrawer::DrawText(
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

    // Convert the text to wide characters if necessary.
#if HELIUM_UNICODE
    const String& wideText = rText;
#else
    String wideText;
    StringConverter< char, wchar_t >::Convert( wideText, rText );
#endif

    // Begin rendering the text (remember to check for surrogate pairs for UTF-16 text).
    float32_t penX = 0.0f;

    bool bHaveSurrogate = false;
    uint32_t highSurrogate = 0;

    const uint16_t quadIndices[] = { 0, 1, 2, 0, 2, 3 };

    float32_t invTextureWidth = 1.0f / static_cast< float32_t >( pFont->GetTextureSheetWidth() );
    float32_t invTextureHeight = 1.0f / static_cast< float32_t >( pFont->GetTextureSheetHeight() );

    size_t characterCount = wideText.GetSize();
    for( size_t characterIndex = 0; characterIndex < characterCount; ++characterIndex )
    {
        uint32_t character = wideText[ characterIndex ];
        if( !bHaveSurrogate )
        {
            if( character >= 0xd800 && character < 0xe000 )
            {
                if( character < 0xdc00 )
                {
                    bHaveSurrogate = true;
                    highSurrogate = character;
                }

                continue;
            }
        }
        else
        {
            bHaveSurrogate = false;

            if( character < 0xdc00 || character >= 0xe000 )
            {
                continue;
            }

            character = ( ( ( highSurrogate - 0xd800 ) << 10 ) | ( character - 0xdc00 ) ) + 0x10000;
        }

        const Font::Character* pCharacter = pFont->FindCharacter( character );
        if( !pCharacter )
        {
            continue;
        }

        RTexture2d* pTexture = pFont->GetTextureSheet( pCharacter->texture );
        if( !pTexture )
        {
            continue;
        }

        float32_t imageWidthFloat = static_cast< float32_t >( pCharacter->imageWidth );
        float32_t imageHeightFloat = static_cast< float32_t >( pCharacter->imageHeight );

        float32_t cornerMinX = Floor( penX + 0.5f ) + static_cast< float32_t >( pCharacter->bearingX >> 6 );
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

        rTransform.TransformPoint( corners[ 0 ], corners[ 0 ] );
        rTransform.TransformPoint( corners[ 1 ], corners[ 1 ] );
        rTransform.TransformPoint( corners[ 2 ], corners[ 2 ] );
        rTransform.TransformPoint( corners[ 3 ], corners[ 3 ] );

        float32_t texCoordMinX = static_cast< float32_t >( pCharacter->imageX );
        float32_t texCoordMinY = static_cast< float32_t >( pCharacter->imageY );
        float32_t texCoordMaxX = texCoordMinX + imageWidthFloat;
        float32_t texCoordMaxY = texCoordMinY + imageHeightFloat;

        texCoordMinX *= invTextureWidth;
        texCoordMinY *= invTextureHeight;
        texCoordMaxX *= invTextureWidth;
        texCoordMaxY *= invTextureHeight;

        const SimpleTexturedVertex vertices[] =
        {
            SimpleTexturedVertex( corners[ 0 ], Simd::Vector2( texCoordMinX, texCoordMinY ), rColor ),
            SimpleTexturedVertex( corners[ 1 ], Simd::Vector2( texCoordMaxX, texCoordMinY ), rColor ),
            SimpleTexturedVertex( corners[ 2 ], Simd::Vector2( texCoordMaxX, texCoordMaxY ), rColor ),
            SimpleTexturedVertex( corners[ 3 ], Simd::Vector2( texCoordMinX, texCoordMaxY ), rColor )
        };

        DrawTexturedMesh( vertices, 4, quadIndices, 2, pTexture );

        penX += Font::Fixed26x6ToFloat32( pCharacter->advance );
    }
}

/// Push buffered draw command data into vertex and index buffers for rendering.
///
/// This must be called prior to calling Draw().  EndDrawing() should be called when rendering is complete.  No new draw
/// calls can be added between a BeginDrawing() and EndDrawing() call pair.
///
/// @see EndDrawing(), Draw()
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

        return;
    }

    // Prepare the vertex and index buffers with the buffered data.
    ResourceSet& rResourceSet = m_resourceSets[ m_currentResourceSetIndex ];

    uint_fast32_t untexturedVertexCount = static_cast< uint_fast32_t >( m_untexturedVertices.GetSize() );
    uint_fast32_t untexturedIndexCount = static_cast< uint_fast32_t >( m_untexturedIndices.GetSize() );
    uint_fast32_t texturedVertexCount = static_cast< uint_fast32_t >( m_texturedVertices.GetSize() );
    uint_fast32_t texturedIndexCount = static_cast< uint_fast32_t >( m_texturedIndices.GetSize() );

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
                TXT( "Failed to create index buffer for textured debug drawing of %" ) TPRIuFAST32 TXT( " indices.\n" ),
                texturedIndexCount );

            rResourceSet.texturedIndexBufferSize = 0;
        }
        else
        {
            rResourceSet.texturedIndexBufferSize = static_cast< uint32_t >( texturedIndexCount );
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
/// @see BeginDrawing(), Draw()
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

        return;
    }

    // Clear all buffered draw call data and swap rendering resources for the next set of buffered draw calls.
    m_texturedMeshDrawCalls.Remove( 0, m_texturedMeshDrawCalls.GetSize() );
    m_solidMeshDrawCalls.Remove( 0, m_solidMeshDrawCalls.GetSize() );
    m_wireMeshDrawCalls.Remove( 0, m_wireMeshDrawCalls.GetSize() );
    m_lineDrawCalls.Remove( 0, m_lineDrawCalls.GetSize() );

    m_currentResourceSetIndex = ( m_currentResourceSetIndex + 1 ) % HELIUM_ARRAY_COUNT( m_resourceSets );
}

/// Issue draw commands for buffered development-mode draw calls.
///
/// BeginDrawing() must be called before issuing calls to this function.  This function can be called multiple times
/// between a BeginDrawing() and EndDrawing() pair.
///
/// Special care should be taken with regards to the following:
/// - This function expects the proper global shader constant data (view/projection matrices) to be already set in
///   vertex constant buffer 0.
/// - The rasterizer state may be altered when this function returns.
///
/// @see BeginDrawing(), EndDrawing()
void BufferedDrawer::Draw()
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

    RRenderCommandProxyPtr spCommandProxy = pRenderer->GetImmediateCommandProxy();

    ResourceSet& rResourceSet = m_resourceSets[ m_currentResourceSetIndex ];

    // Get the shaders to use for debug drawing.
    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();
    ShaderVariant* pShaderVariant;
    RShader* pShaderResource;

    pShaderVariant = rRenderResourceManager.GetSimpleWorldSpaceVertexShader();
    if( !pShaderVariant )
    {
        return;
    }

    pShaderResource = pShaderVariant->GetRenderResource( 0 );
    HELIUM_ASSERT( !pShaderResource || pShaderResource->GetType() == RShader::TYPE_VERTEX );
    RVertexShaderPtr spUntexturedVertexShader = static_cast< RVertexShader* >( pShaderResource );

    pShaderResource = pShaderVariant->GetRenderResource( 1 );
    HELIUM_ASSERT( !pShaderResource || pShaderResource->GetType() == RShader::TYPE_VERTEX );
    RVertexShaderPtr spTexturedVertexShader = static_cast< RVertexShader* >( pShaderResource );

    pShaderVariant = rRenderResourceManager.GetSimpleWorldSpacePixelShader();
    if( !pShaderVariant )
    {
        return;
    }

    pShaderResource = pShaderVariant->GetRenderResource( 0 );
    HELIUM_ASSERT( !pShaderResource || pShaderResource->GetType() == RShader::TYPE_PIXEL );
    RPixelShaderPtr spUntexturedPixelShader = static_cast< RPixelShader* >( pShaderResource );

    pShaderResource = pShaderVariant->GetRenderResource( 1 );
    HELIUM_ASSERT( !pShaderResource || pShaderResource->GetType() == RShader::TYPE_PIXEL );
    RPixelShaderPtr spTexturedPixelShader = static_cast< RPixelShader* >( pShaderResource );

    // Get the vertex description resources for the untextured and textured vertex types.
    RVertexDescriptionPtr spUntexturedVertexDescription = rRenderResourceManager.GetSimpleVertexDescription();
    HELIUM_ASSERT( spUntexturedVertexDescription );

    RVertexDescriptionPtr spTexturedVertexDescription = rRenderResourceManager.GetSimpleTexturedVertexDescription();
    HELIUM_ASSERT( spTexturedVertexDescription );

    // Draw textured meshes first.
    if( spTexturedVertexShader && spTexturedPixelShader &&
        rResourceSet.spTexturedVertexBuffer && rResourceSet.spTexturedIndexBuffer )
    {
        size_t texturedMeshDrawCallCount = m_texturedMeshDrawCalls.GetSize();
        if( texturedMeshDrawCallCount )
        {
            spCommandProxy->SetVertexShader( spTexturedVertexShader );
            spCommandProxy->SetPixelShader( spTexturedPixelShader );

            uint32_t vertexStride = static_cast< uint32_t >( sizeof( SimpleTexturedVertex ) );
            uint32_t vertexOffset = 0;
            spCommandProxy->SetVertexBuffers(
                0,
                1,
                &rResourceSet.spTexturedVertexBuffer,
                &vertexStride,
                &vertexOffset );
            spCommandProxy->SetIndexBuffer( rResourceSet.spTexturedIndexBuffer );

            spTexturedVertexShader->CacheDescription( pRenderer, spTexturedVertexDescription );
            RVertexInputLayout* pVertexInputLayout = spTexturedVertexShader->GetCachedInputLayout();
            HELIUM_ASSERT( pVertexInputLayout );
            spCommandProxy->SetVertexInputLayout( pVertexInputLayout );

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

    // Draw untextured data.
    if( spUntexturedVertexShader && spUntexturedPixelShader &&
        rResourceSet.spUntexturedVertexBuffer && rResourceSet.spUntexturedIndexBuffer )
    {
        size_t solidMeshDrawCallCount = m_solidMeshDrawCalls.GetSize();
        size_t wireMeshDrawCallCount = m_wireMeshDrawCalls.GetSize();
        size_t lineDrawCallCount = m_lineDrawCalls.GetSize();
        if( solidMeshDrawCallCount | wireMeshDrawCallCount | lineDrawCallCount )
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
