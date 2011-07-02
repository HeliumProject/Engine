//----------------------------------------------------------------------------------------------------------------------
// DynamicDrawer.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsPch.h"
#include "Graphics/DynamicDrawer.h"

#include "Rendering/Renderer.h"
#include "Rendering/RFence.h"
#include "Rendering/RIndexBuffer.h"
#include "Rendering/RPixelShader.h"
#include "Rendering/RRenderCommandProxy.h"
#include "Rendering/RVertexBuffer.h"
#include "Rendering/RVertexShader.h"
#include "Graphics/RenderResourceManager.h"
#include "Graphics/Shader.h"

namespace Helium
{
    HELIUM_DECLARE_RPTR( RRenderCommandProxy );
}

using namespace Helium;

DynamicDrawer* DynamicDrawer::sm_pInstance = NULL;

/// Constructor.
DynamicDrawer::DynamicDrawer()
    : m_pActiveDescription( NULL )
{
}

/// Destructor.
DynamicDrawer::~DynamicDrawer()
{
    Shutdown();
}

/// Initialize dynamic drawing, allocating any necessary resources.
///
/// @return  True if initialization was successful, false if not.
///
/// @see Shutdown()
bool DynamicDrawer::Initialize()
{
    Shutdown();

    // If no renderer exists, no resources need to be allocated.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( !pRenderer )
    {
        return true;
    }

    // Allocate dynamic buffers.
    if( !m_untexturedTriangles.Initialize() )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "DynamicDrawer::Initialize(): Failed to initialize buffers for untextured triangle " )
            TXT( "rendering.\n" ) ) );

        Shutdown();

        return false;
    }

    for( size_t bufferSetIndex = 0; bufferSetIndex < HELIUM_ARRAY_COUNT( m_texturedTriangles ); ++bufferSetIndex )
    {
        if( !m_texturedTriangles[ bufferSetIndex ].Initialize() )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "DynamicDrawer::Initialize(): Failed to initialize buffers for textured triangle " )
                TXT( "rendering.\n" ) ) );

            Shutdown();

            return false;
        }
    }

    return true;
}

/// Shut down dynamic drawing and free any allocated resources.
///
/// @see Initialize()
void DynamicDrawer::Shutdown()
{
    m_spUntexturedScreenVertexShader.Release();
    m_spUntexturedScreenPixelShader.Release();
    m_spTexturedScreenVertexShader.Release();
    m_spTexturedScreenPixelShader.Release();

    m_pActiveDescription = NULL;

    m_untexturedTriangles.Shutdown();

    for( size_t bufferSetIndex = 0; bufferSetIndex < HELIUM_ARRAY_COUNT( m_texturedTriangles ); ++bufferSetIndex )
    {
        m_texturedTriangles[ bufferSetIndex ].Shutdown();
        m_texturedTriangleTextures[ bufferSetIndex ].Release();
    }
}

/// Reset the internal state to begin drawing dynamic elements for the current frame or portion of a frame.
///
/// This should be called when first using dynamic drawing after other elements have been rendered.
void DynamicDrawer::Begin()
{
    // Clear out the currently active vertex description (used to know when we need to switch shaders and vertex
    // input layouts).
    m_pActiveDescription = NULL;

    // Grab the screen-space shaders.
    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();

    ShaderVariant* pVertexShaderVariant = rRenderResourceManager.GetSimpleScreenSpaceVertexShader();
    if( pVertexShaderVariant )
    {
        RShader* pShader = pVertexShaderVariant->GetRenderResource( 0 );
        HELIUM_ASSERT( !pShader || pShader->GetType() == RShader::TYPE_VERTEX );
        m_spUntexturedScreenVertexShader = static_cast< RVertexShader* >( pShader );

        pShader = pVertexShaderVariant->GetRenderResource( 1 );
        HELIUM_ASSERT( !pShader || pShader->GetType() == RShader::TYPE_VERTEX );
        m_spTexturedScreenVertexShader = static_cast< RVertexShader* >( pShader );
    }

    ShaderVariant* pPixelShaderVariant = rRenderResourceManager.GetSimpleScreenSpacePixelShader();
    if( pPixelShaderVariant )
    {
        RShader* pShader = pPixelShaderVariant->GetRenderResource( 0 );
        HELIUM_ASSERT( !pShader || pShader->GetType() == RShader::TYPE_PIXEL );
        m_spUntexturedScreenPixelShader = static_cast< RPixelShader* >( pShader );

        pShader = pPixelShaderVariant->GetRenderResource( 1 );
        HELIUM_ASSERT( !pShader || pShader->GetType() == RShader::TYPE_PIXEL );
        m_spTexturedScreenPixelShader = static_cast< RPixelShader* >( pShader );
    }
}

/// Queue an untextured screen-space quad for drawing.
///
/// Quad vertices should be specified in clockwise order.
///
/// @param[in] rVertex0  First quad vertex.
/// @param[in] rVertex1  Second quad vertex.
/// @param[in] rVertex2  Third quad vertex.
/// @param[in] rVertex3  Fourth quad vertex.
/// @param[in] bFlush    True to flush the dynamic vertex buffers for the quad immediately, false to buffer drawing.
void DynamicDrawer::DrawScreenSpaceQuad(
    const SimpleVertex& rVertex0,
    const SimpleVertex& rVertex1,
    const SimpleVertex& rVertex2,
    const SimpleVertex& rVertex3,
    bool bFlush )
{
    // Do nothing if we have no untextured dynamic buffers.
    if( !m_untexturedTriangles.m_spVertices )
    {
        return;
    }

    HELIUM_ASSERT( m_untexturedTriangles.m_spIndices );

    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();

    Renderer* pRenderer = Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );

    RRenderCommandProxyPtr spCommandProxy = pRenderer->GetImmediateCommandProxy();
    HELIUM_ASSERT( spCommandProxy );

    // Flush the untextured dynamic buffers if we don't have enough space left in the vertex and index buffers for
    // this quad.
    if( m_untexturedTriangles.m_vertexCountTotal > BUFFER_DIVISION_VERTEX_COUNT - 4 ||
        m_untexturedTriangles.m_indexCountTotal > BUFFER_DIVISION_INDEX_COUNT - 6 )
    {
        FlushUntexturedTriangles( rRenderResourceManager, pRenderer, spCommandProxy, true );
    }

    // Add the quad vertices.
    uint8_t* pMappedVertices;
    uint16_t* pMappedIndices;
    m_untexturedTriangles.Map( pRenderer, pMappedVertices, pMappedIndices );
    HELIUM_ASSERT( pMappedVertices );
    HELIUM_ASSERT( pMappedIndices );

    pMappedVertices += m_untexturedTriangles.m_vertexCountTotal * sizeof( SimpleVertex );
    pMappedIndices += m_untexturedTriangles.m_indexCountTotal;

    MemoryCopy( pMappedVertices, &rVertex0, sizeof( rVertex0 ) );
    pMappedVertices += sizeof( rVertex0 );
    MemoryCopy( pMappedVertices, &rVertex1, sizeof( rVertex1 ) );
    pMappedVertices += sizeof( rVertex1 );
    MemoryCopy( pMappedVertices, &rVertex2, sizeof( rVertex2 ) );
    pMappedVertices += sizeof( rVertex2 );
    MemoryCopy( pMappedVertices, &rVertex3, sizeof( rVertex3 ) );

    uint16_t startVertexIndex = static_cast< uint16_t >( m_untexturedTriangles.m_vertexCountTotal );
    *( pMappedIndices++ ) = startVertexIndex;
    *( pMappedIndices++ ) = startVertexIndex + 1;
    *( pMappedIndices++ ) = startVertexIndex + 2;
    *( pMappedIndices++ ) = startVertexIndex;
    *( pMappedIndices++ ) = startVertexIndex + 2;
    *pMappedIndices       = startVertexIndex + 3;

    m_untexturedTriangles.m_vertexCountTotal += 4;
    m_untexturedTriangles.m_indexCountTotal += 6;

    m_untexturedTriangles.m_vertexCountPending += 4;
    m_untexturedTriangles.m_indexCountPending += 6;

    // Flush if requested.
    if( bFlush )
    {
        FlushUntexturedTriangles( rRenderResourceManager, pRenderer, spCommandProxy, false );
    }
}

/// Queue a textured screen-space quad for drawing.
///
/// Quad vertices should be specified in clockwise order.
///
/// @param[in] rVertex0  First quad vertex.
/// @param[in] rVertex1  Second quad vertex.
/// @param[in] rVertex2  Third quad vertex.
/// @param[in] rVertex3  Fourth quad vertex.
/// @param[in] pTexture  Texture to apply.
/// @param[in] bFlush    True to flush the dynamic vertex buffers for the quad immediately, false to buffer drawing.
void DynamicDrawer::DrawScreenSpaceQuad(
    const SimpleTexturedVertex& rVertex0,
    const SimpleTexturedVertex& rVertex1,
    const SimpleTexturedVertex& rVertex2,
    const SimpleTexturedVertex& rVertex3,
    RTexture2d* pTexture,
    bool bFlush )
{
    // Do nothing if we have no dynamic buffers.
    if( !m_untexturedTriangles.m_spVertices )
    {
        return;
    }

    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();

    Renderer* pRenderer = Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );

    RRenderCommandProxyPtr spCommandProxy = pRenderer->GetImmediateCommandProxy();
    HELIUM_ASSERT( spCommandProxy );

    // Attempt to find a textured triangle buffer set using the same texture as the one specified.  If one cannot be
    // located, flush and use the one with the most triangles already set.
    size_t largestUsedBufferSetIndex = 0;
    uint32_t largestUsedBufferSetIndexCount = m_texturedTriangles[ 0 ].m_indexCountTotal;

    size_t bufferSetIndex;
    for( bufferSetIndex = 0; bufferSetIndex < HELIUM_ARRAY_COUNT( m_texturedTriangles ); ++bufferSetIndex )
    {
        uint32_t indexCount = m_texturedTriangles[ bufferSetIndex ].m_indexCountTotal;
        if( indexCount == 0 )
        {
            continue;
        }

        if( m_texturedTriangleTextures[ bufferSetIndex ] == pTexture )
        {
            break;
        }

        if( indexCount > largestUsedBufferSetIndexCount )
        {
            largestUsedBufferSetIndex = bufferSetIndex;
            largestUsedBufferSetIndexCount = indexCount;
        }
    }

    if( bufferSetIndex >= HELIUM_ARRAY_COUNT( m_texturedTriangles ) )
    {
        bufferSetIndex = largestUsedBufferSetIndex;
        FlushTexturedTriangles( rRenderResourceManager, pRenderer, spCommandProxy, bufferSetIndex, true );
    }

    // Flush the buffers if we don't have enough space left in the vertex and index buffers for this quad.
    BufferData<
        SimpleTexturedVertex,
        TexturedBufferFunctions,
        BUFFER_DIVISION_COUNT,
        BUFFER_DIVISION_VERTEX_COUNT,
        BUFFER_DIVISION_INDEX_COUNT >& rBufferData = m_texturedTriangles[ bufferSetIndex ];
    if( rBufferData.m_vertexCountTotal > BUFFER_DIVISION_VERTEX_COUNT - 4 ||
        rBufferData.m_indexCountTotal > BUFFER_DIVISION_INDEX_COUNT - 6 )
    {
        FlushTexturedTriangles( rRenderResourceManager, pRenderer, spCommandProxy, bufferSetIndex, true );
    }

    // Add the quad vertices.
    uint8_t* pMappedVertices;
    uint16_t* pMappedIndices;
    rBufferData.Map( pRenderer, pMappedVertices, pMappedIndices );
    HELIUM_ASSERT( pMappedVertices );
    HELIUM_ASSERT( pMappedIndices );

    pMappedVertices += rBufferData.m_vertexCountTotal * sizeof( SimpleTexturedVertex );
    pMappedIndices += rBufferData.m_indexCountTotal;

    MemoryCopy( pMappedVertices, &rVertex0, sizeof( rVertex0 ) );
    pMappedVertices += sizeof( rVertex0 );
    MemoryCopy( pMappedVertices, &rVertex1, sizeof( rVertex1 ) );
    pMappedVertices += sizeof( rVertex1 );
    MemoryCopy( pMappedVertices, &rVertex2, sizeof( rVertex2 ) );
    pMappedVertices += sizeof( rVertex2 );
    MemoryCopy( pMappedVertices, &rVertex3, sizeof( rVertex3 ) );

    uint16_t startVertexIndex = static_cast< uint16_t >( m_untexturedTriangles.m_vertexCountTotal );
    *( pMappedIndices++ ) = startVertexIndex;
    *( pMappedIndices++ ) = startVertexIndex + 1;
    *( pMappedIndices++ ) = startVertexIndex + 2;
    *( pMappedIndices++ ) = startVertexIndex;
    *( pMappedIndices++ ) = startVertexIndex + 2;
    *pMappedIndices       = startVertexIndex + 3;

    rBufferData.m_vertexCountTotal += 4;
    rBufferData.m_indexCountTotal += 6;

    rBufferData.m_vertexCountPending += 4;
    rBufferData.m_indexCountPending += 6;

    // Store the quad texture.
    m_texturedTriangleTextures[ bufferSetIndex ] = pTexture;

    // Flush if requested.
    if( bFlush )
    {
        FlushTexturedTriangles( rRenderResourceManager, pRenderer, spCommandProxy, bufferSetIndex, false );
    }
}

/// Flush all buffered draw commands and reset the internal state.
///
/// This should be called after dynamic drawing has completed for a frame or portion of a frame.
void DynamicDrawer::Flush()
{
    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( !pRenderer )
    {
        return;
    }

    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();

    RRenderCommandProxyPtr spCommandProxy = pRenderer->GetImmediateCommandProxy();
    HELIUM_ASSERT( spCommandProxy );

    FlushUntexturedTriangles( rRenderResourceManager, pRenderer, spCommandProxy, true );

    for( size_t bufferSetIndex = 0; bufferSetIndex < HELIUM_ARRAY_COUNT( m_texturedTriangles ); ++bufferSetIndex )
    {
        FlushTexturedTriangles( rRenderResourceManager, pRenderer, spCommandProxy, bufferSetIndex, true );
    }

    m_pActiveDescription = NULL;

    m_spUntexturedScreenVertexShader.Release();
    m_spUntexturedScreenPixelShader.Release();
    m_spTexturedScreenVertexShader.Release();
    m_spTexturedScreenPixelShader.Release();
}

/// Get the singleton DynamicDrawer instance, creating it if necessary.
///
/// Note that this expects the 
///
/// @return  Reference to the DynamicDrawer instance.
///
/// @see DestroyStaticInstance()
DynamicDrawer& DynamicDrawer::GetStaticInstance()
{
    if( !sm_pInstance )
    {
        sm_pInstance = new DynamicDrawer;
        HELIUM_ASSERT( sm_pInstance );
    }

    return *sm_pInstance;
}

/// Destroy the singleton DynamicDrawer instance.
///
/// @see GetStaticInstance()
void DynamicDrawer::DestroyStaticInstance()
{
    delete sm_pInstance;
    sm_pInstance = NULL;
}

/// Flush buffered drawing of untextured triangles.
///
/// @param[in] rRenderResourceManager  Render resource manager instance.
/// @param[in] pRenderer               Renderer interface.
/// @param[in] pCommandProxy           Interface to use for issuing render commands.
/// @param[in] bAdvanceDivision        True to advance to the next dynamic buffer division, false to continue writing
///                                    vertex data to the current division.
///
/// @see FlushTexturedTriangles()
void DynamicDrawer::FlushUntexturedTriangles(
    RenderResourceManager& rRenderResourceManager,
    Renderer* pRenderer,
    RRenderCommandProxy* pCommandProxy,
    bool bAdvanceDivision )
{
    m_untexturedTriangles.FlushTriangles( this, rRenderResourceManager, pRenderer, pCommandProxy, bAdvanceDivision );
}

/// Flush buffered drawing of textured triangles.
///
/// @param[in] pRenderer               Renderer interface.
/// @param[in] rRenderResourceManager  Render resource manager instance.
/// @param[in] pCommandProxy           Interface to use for issuing render commands.
/// @param[in] bufferSetIndex          Index of the textured triangle buffer set to flush.
/// @param[in] bAdvanceDivision        True to advance to the next dynamic buffer division, false to continue writing
///                                    vertex data to the current division.
///
/// @see FlushUntexturedTriangles()
void DynamicDrawer::FlushTexturedTriangles(
    RenderResourceManager& rRenderResourceManager,
    Renderer* pRenderer,
    RRenderCommandProxy* pCommandProxy,
    size_t bufferSetIndex,
    bool bAdvanceDivision )
{
    HELIUM_ASSERT( bufferSetIndex < HELIUM_ARRAY_COUNT( m_texturedTriangles ) );

    m_texturedTriangles[ bufferSetIndex ].FlushTriangles(
        this,
        rRenderResourceManager,
        pRenderer,
        pCommandProxy,
        bAdvanceDivision );
}

/// Constructor.
template<
typename VertexType,
typename Functions,
uint32_t DivisionCount,
uint32_t DivisionVertexCount,
uint32_t DivisionIndexCount >
DynamicDrawer::BufferData< VertexType, Functions, DivisionCount, DivisionVertexCount, DivisionIndexCount >::BufferData()
: m_pMappedVertices( NULL )
, m_pMappedIndices( NULL )
, m_divisionIndex( 0 )
, m_vertexCountTotal( 0 )
, m_indexCountTotal( 0 )
, m_vertexCountPending( 0 )
, m_indexCountPending( 0 )
{
}

/// Allocate vertex and index buffers and initialize this buffer data object for use.
///
/// @return  True if initialization was successful, false if not.
///
/// @see Shutdown()
template<
typename VertexType,
typename Functions,
uint32_t DivisionCount,
uint32_t DivisionVertexCount,
uint32_t DivisionIndexCount >
bool DynamicDrawer::BufferData< VertexType, Functions, DivisionCount, DivisionVertexCount, DivisionIndexCount >::Initialize()
{
    Shutdown();

    Renderer* pRenderer = Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );

    size_t vertexBufferSize = DivisionVertexCount * DivisionCount * sizeof( VertexType );
    m_spVertices = pRenderer->CreateVertexBuffer( vertexBufferSize, RENDERER_BUFFER_USAGE_DYNAMIC );
    HELIUM_ASSERT( m_spVertices );
    if( !m_spVertices )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "DynamicDrawer::BufferData::Initialize(): Failed to allocate vertex buffer of %" ) TPRIuSZ
            TXT( " bytes.\n" ) ),
            vertexBufferSize );

        Shutdown();

        return false;
    }

    size_t indexBufferSize = DivisionIndexCount * DivisionCount * sizeof( uint16_t );
    m_spIndices = pRenderer->CreateIndexBuffer(
        indexBufferSize,
        RENDERER_BUFFER_USAGE_DYNAMIC,
        RENDERER_INDEX_FORMAT_UINT16 );
    HELIUM_ASSERT( m_spIndices );
    if( !m_spIndices )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "DynamicDrawer::BufferData::Initialize(): Failed to allocate index buffer of %" ) TPRIuSZ
            TXT( " bytes.\n" ) ),
            indexBufferSize );

        Shutdown();

        return false;
    }

    return true;
}

/// Free all allocated resources and reset this object to its initial state.
///
/// @see Initialize()
template<
typename VertexType,
typename Functions,
uint32_t DivisionCount,
uint32_t DivisionVertexCount,
uint32_t DivisionIndexCount >
void DynamicDrawer::BufferData< VertexType, Functions, DivisionCount, DivisionVertexCount, DivisionIndexCount >::Shutdown()
{
    if( m_pMappedVertices )
    {
        HELIUM_ASSERT( m_spVertices );
        m_spVertices->Unmap();
        m_pMappedVertices = NULL;
    }

    if( m_pMappedIndices )
    {
        HELIUM_ASSERT( m_spIndices );
        m_spIndices->Unmap();
        m_pMappedIndices = NULL;
    }

    m_spVertices.Release();
    m_spIndices.Release();

    for( size_t divisionIndex = 0; divisionIndex < HELIUM_ARRAY_COUNT( m_divisionFences ); ++divisionIndex )
    {
        m_divisionFences[ divisionIndex ].Release();
    }

    m_divisionIndex = 0;
    m_vertexCountTotal = 0;
    m_indexCountTotal = 0;
    m_vertexCountPending = 0;
    m_indexCountPending = 0;
}

/// Get mapped pointers to the vertex and index buffers in this buffer set.
///
/// @param[in]  pRenderer         Renderer interface.
/// @param[out] rpMappedVertices  Base address of the mapped vertex buffer.
/// @param[out] rpMappedIndices   Base address of the mapped index buffer.
template<
typename VertexType,
typename Functions,
uint32_t DivisionCount,
uint32_t DivisionVertexCount,
uint32_t DivisionIndexCount >
void DynamicDrawer::BufferData< VertexType, Functions, DivisionCount, DivisionVertexCount, DivisionIndexCount >::Map(
    Renderer* pRenderer,
    uint8_t*& rpMappedVertices,
    uint16_t*& rpMappedIndices )
{
    HELIUM_ASSERT( pRenderer );

    ERendererBufferMapHint mapHint =
        ( m_divisionIndex == 0 ? RENDERER_BUFFER_MAP_HINT_DISCARD : RENDERER_BUFFER_MAP_HINT_NO_OVERWRITE );

    if( !m_pMappedVertices )
    {
        HELIUM_ASSERT( !m_pMappedIndices );

        RFencePtr& rspDivisionFence = m_divisionFences[ m_divisionIndex ];
        RFence* pDivisionFence = rspDivisionFence;
        if( pDivisionFence )
        {
            pRenderer->SyncFence( pDivisionFence );
            rspDivisionFence.Release();
        }

        HELIUM_ASSERT( m_spVertices );
        m_pMappedVertices = static_cast< uint8_t* >( m_spVertices->Map( mapHint ) );
        HELIUM_ASSERT( m_pMappedVertices );
        m_pMappedVertices += m_divisionIndex * DivisionVertexCount * sizeof( VertexType );

        HELIUM_ASSERT( m_spIndices );
        m_pMappedIndices = static_cast< uint16_t* >( m_spIndices->Map( mapHint ) );
        HELIUM_ASSERT( m_pMappedIndices );
        m_pMappedIndices += m_divisionIndex * DivisionIndexCount;
    }

    rpMappedVertices = m_pMappedVertices;
    rpMappedIndices = m_pMappedIndices;
}

/// Flush buffered drawing of triangles.
///
/// @param[in] pDynamicDrawer          Dynamic drawer instance.
/// @param[in] rRenderResourceManager  Render resource manager instance.
/// @param[in] pRenderer               Renderer interface.
/// @param[in] pCommandProxy           Interface to use for issuing render commands.
/// @param[in] bAdvanceDivision        True to advance to the next dynamic buffer division, false to continue writing
///                                    vertex data to the current division.
template<
    typename VertexType,
    typename Functions,
    uint32_t DivisionCount,
    uint32_t DivisionVertexCount,
    uint32_t DivisionIndexCount >
void DynamicDrawer::BufferData< VertexType, Functions, DivisionCount, DivisionVertexCount, DivisionIndexCount >::FlushTriangles(
    DynamicDrawer* pDynamicDrawer,
    RenderResourceManager& rRenderResourceManager,
    Renderer* pRenderer,
    RRenderCommandProxy* pCommandProxy,
    bool bAdvanceDivision )
{
    HELIUM_ASSERT( pDynamicDrawer );
    HELIUM_ASSERT( pRenderer );
    HELIUM_ASSERT( pCommandProxy );

    if( m_indexCountPending != 0 )
    {
        HELIUM_ASSERT( m_spVertices );
        HELIUM_ASSERT( m_spIndices );
        HELIUM_ASSERT( m_pMappedVertices );
        HELIUM_ASSERT( m_pMappedIndices );

        m_spVertices->Unmap();
        m_spIndices->Unmap();
        m_pMappedVertices = NULL;
        m_pMappedIndices = NULL;

        RVertexDescription* pVertexDescription = m_functions.GetVertexDescription( rRenderResourceManager );
        HELIUM_ASSERT( pVertexDescription );
        if( pDynamicDrawer->m_pActiveDescription != pVertexDescription )
        {
            pDynamicDrawer->m_pActiveDescription = pVertexDescription;

            RVertexShader* pVertexShader = m_functions.GetVertexShader( pDynamicDrawer );
            HELIUM_ASSERT( pVertexShader );
            RPixelShader* pPixelShader = m_functions.GetPixelShader( pDynamicDrawer );
            HELIUM_ASSERT( pPixelShader );

            pVertexShader->CacheDescription( pRenderer, pVertexDescription );
            RVertexInputLayout* pVertexInputLayout = pVertexShader->GetCachedInputLayout();
            HELIUM_ASSERT( pVertexInputLayout );

            pCommandProxy->SetVertexShader( pVertexShader );
            pCommandProxy->SetPixelShader( pPixelShader );
            pCommandProxy->SetVertexInputLayout( pVertexInputLayout );
        }

        HELIUM_ASSERT( m_indexCountPending % 3 == 0 );
        uint32_t triangleCount = m_indexCountPending / 3;

        uint32_t minIndexValue = m_vertexCountTotal - m_vertexCountPending;

        uint32_t startIndex = m_divisionIndex * DivisionIndexCount + m_indexCountTotal - m_indexCountPending;

        uint32_t stride = static_cast< uint32_t >( sizeof( VertexType ) );
        uint32_t offset = 0;
        pCommandProxy->SetVertexBuffers( 0, 1, &m_spVertices, &stride, &offset );
        pCommandProxy->SetIndexBuffer( m_spIndices );
        m_functions.PrepareDraw( pDynamicDrawer, pCommandProxy, this );
        pCommandProxy->DrawIndexed(
            RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST,
            m_divisionIndex * DivisionVertexCount,
            minIndexValue,
            m_vertexCountPending,
            startIndex,
            triangleCount );

        m_vertexCountPending = 0;
        m_indexCountPending = 0;
    }

    HELIUM_ASSERT( m_vertexCountPending == 0 );

    if( bAdvanceDivision )
    {
        if( m_indexCountTotal != 0 )
        {
            RFencePtr& rspDivisionFence = m_divisionFences[ m_divisionIndex ];
            HELIUM_ASSERT( !rspDivisionFence );
            rspDivisionFence = pRenderer->CreateFence();
            HELIUM_ASSERT( rspDivisionFence );
            pCommandProxy->SetFence( rspDivisionFence );

            m_divisionIndex = ( m_divisionIndex + 1 ) % DivisionCount;
            m_vertexCountTotal = 0;
            m_indexCountTotal = 0;
        }

        HELIUM_ASSERT( m_vertexCountTotal == 0 );
    }
}

/// Get the description for untextured vertices.
///
/// @param[in] rRenderResourceManager  Render resource manager instance.
///
/// @return  Untextured vertex description.
///
/// @see GetVertexShader(), GetPixelShader()
RVertexDescription* DynamicDrawer::UntexturedBufferFunctions::GetVertexDescription(
    RenderResourceManager& rRenderResourceManager ) const
{
    return rRenderResourceManager.GetSimpleVertexDescription();
}

/// Get the vertex shader for untextured screen-space rendering.
///
/// @param[in] pDynamicDrawer  Dynamic drawing interface.
///
/// @return  Untextured screen-space vertex shader.
///
/// @see GetPixelShader(), GetVertexDescription()
RVertexShader* DynamicDrawer::UntexturedBufferFunctions::GetVertexShader( DynamicDrawer* pDynamicDrawer ) const
{
    HELIUM_ASSERT( pDynamicDrawer );

    return pDynamicDrawer->m_spUntexturedScreenVertexShader;
}

/// Get the pixel shader for untextured screen-space rendering.
///
/// @param[in] pDynamicDrawer  Dynamic drawing interface.
///
/// @return  Untextured screen-space pixel shader.
///
/// @see GetVertexShader(), GetVertexDescription()
RPixelShader* DynamicDrawer::UntexturedBufferFunctions::GetPixelShader( DynamicDrawer* pDynamicDrawer ) const
{
    HELIUM_ASSERT( pDynamicDrawer );

    return pDynamicDrawer->m_spUntexturedScreenPixelShader;
}

/// Perform any necessary renderer setup prior to issuing a draw command for untextured primitive rendering.
///
/// @param[in] pDynamicDrawer  Dynamic drawing interface.
/// @param[in] pCommandProxy   Renderer command proxy interface.
/// @param[in] pBufferData     Primitive buffer data.
void DynamicDrawer::UntexturedBufferFunctions::PrepareDraw(
    DynamicDrawer* /*pDynamicDrawer*/,
    RRenderCommandProxy* /*pCommandProxy*/,
    BufferData<
        SimpleVertex,
        UntexturedBufferFunctions,
        BUFFER_DIVISION_COUNT,
        BUFFER_DIVISION_VERTEX_COUNT,
        BUFFER_DIVISION_INDEX_COUNT >* /*pBufferData*/ ) const
{
    // Nothing needs to be done for untextured rendering.
}

/// Get the description for textured vertices.
///
/// @param[in] rRenderResourceManager  Render resource manager instance.
///
/// @return  Textured vertex description.
///
/// @see GetVertexShader(), GetPixelShader()
RVertexDescription* DynamicDrawer::TexturedBufferFunctions::GetVertexDescription(
    RenderResourceManager& rRenderResourceManager ) const
{
    return rRenderResourceManager.GetSimpleTexturedVertexDescription();
}

/// Get the vertex shader for textured screen-space rendering.
///
/// @param[in] pDynamicDrawer  Dynamic drawing interface.
///
/// @return  Textured screen-space vertex shader.
///
/// @see GetPixelShader(), GetVertexDescription()
RVertexShader* DynamicDrawer::TexturedBufferFunctions::GetVertexShader( DynamicDrawer* pDynamicDrawer ) const
{
    HELIUM_ASSERT( pDynamicDrawer );

    return pDynamicDrawer->m_spTexturedScreenVertexShader;
}

/// Get the pixel shader for textured screen-space rendering.
///
/// @param[in] pDynamicDrawer  Dynamic drawing interface.
///
/// @return  Textured screen-space pixel shader.
///
/// @see GetVertexShader(), GetVertexDescription()
RPixelShader* DynamicDrawer::TexturedBufferFunctions::GetPixelShader( DynamicDrawer* pDynamicDrawer ) const
{
    HELIUM_ASSERT( pDynamicDrawer );

    return pDynamicDrawer->m_spTexturedScreenPixelShader;
}

/// Perform any necessary renderer setup prior to issuing a draw command for textured primitive rendering.
///
/// @param[in] pDynamicDrawer  Dynamic drawing interface.
/// @param[in] pCommandProxy   Renderer command proxy interface.
/// @param[in] pBufferData     Primitive buffer data.
void DynamicDrawer::TexturedBufferFunctions::PrepareDraw(
    DynamicDrawer* pDynamicDrawer,
    RRenderCommandProxy* pCommandProxy,
    BufferData<
        SimpleTexturedVertex,
        TexturedBufferFunctions,
        BUFFER_DIVISION_COUNT,
        BUFFER_DIVISION_VERTEX_COUNT,
        BUFFER_DIVISION_INDEX_COUNT >* pBufferData ) const
{
    HELIUM_ASSERT( pDynamicDrawer );
    HELIUM_ASSERT( pCommandProxy );
    HELIUM_ASSERT( pBufferData );

    size_t bufferIndex = static_cast< size_t >( pBufferData - &pDynamicDrawer->m_texturedTriangles[ 0 ] );
    HELIUM_ASSERT( bufferIndex < HELIUM_ARRAY_COUNT( pDynamicDrawer->m_texturedTriangles ) );

    pCommandProxy->SetTexture( 0, pDynamicDrawer->m_texturedTriangleTextures[ bufferIndex ] );
}
