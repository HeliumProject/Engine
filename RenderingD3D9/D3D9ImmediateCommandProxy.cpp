#include "Precompile.h"
#include "RenderingD3D9/D3D9ImmediateCommandProxy.h"

#include "RenderingD3D9/D3D9BlendState.h"
#include "RenderingD3D9/D3D9ConstantBuffer.h"
#include "RenderingD3D9/D3D9DepthStencilState.h"
#include "RenderingD3D9/D3D9Fence.h"
#include "RenderingD3D9/D3D9IndexBuffer.h"
#include "RenderingD3D9/D3D9PixelShader.h"
#include "RenderingD3D9/D3D9RasterizerState.h"
#include "RenderingD3D9/D3D9RenderCommandList.h"
#include "RenderingD3D9/D3D9SamplerState.h"
#include "RenderingD3D9/D3D9Surface.h"
#include "RenderingD3D9/D3D9Texture2d.h"
#include "RenderingD3D9/D3D9VertexBuffer.h"
#include "RenderingD3D9/D3D9VertexInputLayout.h"
#include "RenderingD3D9/D3D9VertexShader.h"

using namespace Helium;

/// Constructor.
///
/// @param[in] pD3DDevice  Direct3D 9 device to use for rendering.  Its reference count will be incremented when
///                        this object is constructed and decremented back when this object is destroyed.
D3D9ImmediateCommandProxy::D3D9ImmediateCommandProxy( IDirect3DDevice9* pD3DDevice )
: m_pDevice( pD3DDevice )
, m_srgbTextureFlags( 0 )
{
    HELIUM_ASSERT( pD3DDevice );
    pD3DDevice->AddRef();
}

/// Destructor.
D3D9ImmediateCommandProxy::~D3D9ImmediateCommandProxy()
{
    m_pDevice->Release();
}

/// @copydoc RRenderCommandProxy::SetRasterizerState()
void D3D9ImmediateCommandProxy::SetRasterizerState( RRasterizerState* pState )
{
    D3D9RasterizerState* pD3D9State = static_cast< D3D9RasterizerState* >( pState );
    D3D9RasterizerState* pCurrentState = m_spRasterizerState;
    if( pCurrentState == pD3D9State )
    {
        return;
    }

    m_spRasterizerState = pD3D9State;

    if( !pD3D9State )
    {
        return;
    }

    if( pCurrentState )
    {
        if( pCurrentState->m_fillMode != pD3D9State->m_fillMode )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_FILLMODE, pD3D9State->m_fillMode ) );
        }

        if( pCurrentState->m_cullMode != pD3D9State->m_cullMode )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_CULLMODE, pD3D9State->m_cullMode ) );
        }

        if( pCurrentState->m_depthBias != pD3D9State->m_depthBias )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState(
                D3DRS_DEPTHBIAS,
                reinterpret_cast< const DWORD& >( pD3D9State->m_depthBias ) ) );
        }

        if( pCurrentState->m_slopeScaledDepthBias != pD3D9State->m_slopeScaledDepthBias )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState(
                D3DRS_SLOPESCALEDEPTHBIAS,
                reinterpret_cast< const DWORD& >( pD3D9State->m_slopeScaledDepthBias ) ) );
        }
    }
    else
    {
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_FILLMODE, pD3D9State->m_fillMode ) );
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_CULLMODE, pD3D9State->m_cullMode ) );
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState(
            D3DRS_DEPTHBIAS,
            reinterpret_cast< const DWORD& >( pD3D9State->m_depthBias ) ) );
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState(
            D3DRS_SLOPESCALEDEPTHBIAS,
            reinterpret_cast< const DWORD& >( pD3D9State->m_slopeScaledDepthBias ) ) );
    }
}

/// @copydoc RRenderCommandProxy::SetBlendState()
void D3D9ImmediateCommandProxy::SetBlendState( RBlendState* pState )
{
    D3D9BlendState* pD3D9State = static_cast< D3D9BlendState* >( pState );
    D3D9BlendState* pCurrentState = m_spBlendState;
    if( pCurrentState == pD3D9State )
    {
        return;
    }

    m_spBlendState = pD3D9State;

    if( !pD3D9State )
    {
        return;
    }

    if( pCurrentState )
    {
        if( pCurrentState->m_sourceFactor != pD3D9State->m_sourceFactor )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_SRCBLEND, pD3D9State->m_sourceFactor ) );
        }

        if( pCurrentState->m_destinationFactor != pD3D9State->m_destinationFactor )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_DESTBLEND, pD3D9State->m_destinationFactor ) );
        }

        if( pCurrentState->m_function != pD3D9State->m_function )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_BLENDOP, pD3D9State->m_function ) );
        }

        if( pCurrentState->m_colorWriteMask != pD3D9State->m_colorWriteMask )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_COLORWRITEENABLE, pD3D9State->m_colorWriteMask ) );
        }

        if( pCurrentState->m_bBlendEnable != pD3D9State->m_bBlendEnable )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, pD3D9State->m_bBlendEnable ) );
        }
    }
    else
    {
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_SRCBLEND, pD3D9State->m_sourceFactor ) );
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_DESTBLEND, pD3D9State->m_destinationFactor ) );
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_BLENDOP, pD3D9State->m_function ) );
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_COLORWRITEENABLE, pD3D9State->m_colorWriteMask ) );
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, pD3D9State->m_bBlendEnable ) );
    }
}

/// @copydoc RRenderCommandProxy::SetDepthStencilState()
void D3D9ImmediateCommandProxy::SetDepthStencilState( RDepthStencilState* pState, uint8_t stencilReferenceValue )
{
    HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_STENCILREF, stencilReferenceValue ) );

    D3D9DepthStencilState* pD3D9State = static_cast< D3D9DepthStencilState* >( pState );
    D3D9DepthStencilState* pCurrentState = m_spDepthStencilState;
    if( pCurrentState == pD3D9State )
    {
        return;
    }

    m_spDepthStencilState = pD3D9State;

    if( !pD3D9State )
    {
        return;
    }

    if( pCurrentState )
    {
        if( pCurrentState->m_depthFunction != pD3D9State->m_depthFunction )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_ZFUNC, pD3D9State->m_depthFunction ) );
        }

        if( pCurrentState->m_stencilFailOperation != pD3D9State->m_stencilFailOperation )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_STENCILFAIL, pD3D9State->m_stencilFailOperation ) );
        }

        if( pCurrentState->m_stencilDepthFailOperation != pD3D9State->m_stencilDepthFailOperation )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_STENCILZFAIL, pD3D9State->m_stencilDepthFailOperation ) );
        }

        if( pCurrentState->m_stencilDepthPassOperation != pD3D9State->m_stencilDepthPassOperation )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_STENCILPASS, pD3D9State->m_stencilDepthPassOperation ) );
        }

        if( pCurrentState->m_stencilFunction != pD3D9State->m_stencilFunction )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_STENCILFUNC, pD3D9State->m_stencilFunction ) );
        }

        if( pCurrentState->m_stencilReadMask != pD3D9State->m_stencilReadMask )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_STENCILMASK, pD3D9State->m_stencilReadMask ) );
        }

        if( pCurrentState->m_stencilWriteMask != pD3D9State->m_stencilWriteMask )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_STENCILWRITEMASK, pD3D9State->m_stencilWriteMask ) );
        }

        if( pCurrentState->m_bDepthTestEnable != pD3D9State->m_bDepthTestEnable )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_ZENABLE, pD3D9State->m_bDepthTestEnable ) );
        }

        if( pCurrentState->m_bDepthWriteEnable != pD3D9State->m_bDepthWriteEnable )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, pD3D9State->m_bDepthWriteEnable ) );
        }

        if( pCurrentState->m_bStencilTestEnable != pD3D9State->m_bStencilTestEnable )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_STENCILENABLE, pD3D9State->m_bStencilTestEnable ) );
        }
    }
    else
    {
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_ZFUNC, pD3D9State->m_depthFunction ) );
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_STENCILFAIL, pD3D9State->m_stencilFailOperation ) );
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_STENCILZFAIL, pD3D9State->m_stencilDepthFailOperation ) );
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_STENCILPASS, pD3D9State->m_stencilDepthPassOperation ) );
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_STENCILFUNC, pD3D9State->m_stencilFunction ) );
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_STENCILMASK, pD3D9State->m_stencilReadMask ) );
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_STENCILWRITEMASK, pD3D9State->m_stencilWriteMask ) );
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_ZENABLE, pD3D9State->m_bDepthTestEnable ) );
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, pD3D9State->m_bDepthWriteEnable ) );
        HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_STENCILENABLE, pD3D9State->m_bStencilTestEnable ) );
    }
}

/// @copydoc RRenderCommandProxy::SetSamplerStates()
void D3D9ImmediateCommandProxy::SetSamplerStates(
    size_t startIndex,
    size_t samplerCount,
    RSamplerState* const* ppStates )
{
    HELIUM_ASSERT( ppStates || samplerCount == 0 );

    if( startIndex >= HELIUM_ARRAY_COUNT( m_samplerStates ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "D3D9ImmediateCommandProxy::SetSamplerStates(): Start index (%" PRIuSZ ") exceeds the number of sampler stages available (%" PRIuSZ ").\n",
            startIndex,
            HELIUM_ARRAY_COUNT( m_samplerStates ) );

        return;
    }

    size_t samplerCountMax = HELIUM_ARRAY_COUNT( m_samplerStates ) - startIndex;
    if( samplerCount > samplerCountMax )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "D3D9ImmediateCommandProxy::SetSamplerStates(): Input sampler state array (start index: %" PRIuSZ "; sampler count: %" PRIuSZ ") exceeds the available sampler stage range (%" PRIuSZ ").  State object range will be clamped.\n",
            startIndex,
            samplerCount,
            HELIUM_ARRAY_COUNT( m_samplerStates ) );

        samplerCount = samplerCountMax;
    }

    size_t samplerMax = startIndex + samplerCount;
    for( size_t samplerIndex = startIndex; samplerIndex < samplerMax; ++samplerIndex )
    {
        D3D9SamplerState* pD3D9State = static_cast< D3D9SamplerState* >( *ppStates );
        ++ppStates;

        D3D9SamplerState* pCurrentState = m_samplerStates[ samplerIndex ];
        if( pCurrentState == pD3D9State )
        {
            continue;;
        }

        m_samplerStates[ samplerIndex ] = pD3D9State;

        if( !pD3D9State )
        {
            continue;;
        }

        DWORD indexDword = static_cast< DWORD >( samplerIndex );

        if( pCurrentState )
        {
            if( pCurrentState->m_minFilter != pD3D9State->m_minFilter )
            {
                HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState(
                    indexDword,
                    D3DSAMP_MINFILTER,
                    pD3D9State->m_minFilter ) );
            }

            if( pCurrentState->m_magFilter != pD3D9State->m_magFilter )
            {
                HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState(
                    indexDword,
                    D3DSAMP_MAGFILTER,
                    pD3D9State->m_magFilter ) );
            }

            if( pCurrentState->m_mipFilter != pD3D9State->m_mipFilter )
            {
                HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState(
                    indexDword,
                    D3DSAMP_MIPFILTER,
                    pD3D9State->m_mipFilter ) );
            }

            if( pCurrentState->m_addressModeU != pD3D9State->m_addressModeU )
            {
                HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState(
                    indexDword,
                    D3DSAMP_ADDRESSU,
                    pD3D9State->m_addressModeU ) );
            }

            if( pCurrentState->m_addressModeV != pD3D9State->m_addressModeV )
            {
                HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState(
                    indexDword,
                    D3DSAMP_ADDRESSV,
                    pD3D9State->m_addressModeV ) );
            }

            if( pCurrentState->m_addressModeW != pD3D9State->m_addressModeW )
            {
                HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState(
                    indexDword,
                    D3DSAMP_ADDRESSW,
                    pD3D9State->m_addressModeW ) );
            }

            if( pCurrentState->m_mipLodBias != pD3D9State->m_mipLodBias )
            {
                HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState(
                    indexDword,
                    D3DSAMP_MIPMAPLODBIAS,
                    pD3D9State->m_mipLodBias ) );
            }

            if( pCurrentState->m_maxAnisotropy != pD3D9State->m_maxAnisotropy )
            {
                HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState(
                    indexDword,
                    D3DSAMP_MAXANISOTROPY,
                    pD3D9State->m_maxAnisotropy ) );
            }
        }
        else
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState( indexDword, D3DSAMP_MINFILTER, pD3D9State->m_minFilter ) );
            HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState( indexDword, D3DSAMP_MAGFILTER, pD3D9State->m_magFilter ) );
            HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState( indexDword, D3DSAMP_MIPFILTER, pD3D9State->m_mipFilter ) );
            HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState( indexDword, D3DSAMP_ADDRESSU, pD3D9State->m_addressModeU ) );
            HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState( indexDword, D3DSAMP_ADDRESSV, pD3D9State->m_addressModeV ) );
            HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState( indexDword, D3DSAMP_ADDRESSW, pD3D9State->m_addressModeW ) );
            HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState(
                indexDword,
                D3DSAMP_MIPMAPLODBIAS,
                pD3D9State->m_mipLodBias ) );
            HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState(
                indexDword,
                D3DSAMP_MAXANISOTROPY,
                pD3D9State->m_maxAnisotropy ) );
        }
    }
}

/// @copydoc RRenderCommandProxy::SetRenderSurfaces()
void D3D9ImmediateCommandProxy::SetRenderSurfaces( RSurface* pRenderTargetSurface, RSurface* pDepthStencilSurface )
{
    HELIUM_ASSERT( pRenderTargetSurface );
    // pDepthStencilSurface can be null.

    IDirect3DSurface9* pD3DRenderSurface = static_cast< D3D9Surface* >( pRenderTargetSurface )->GetD3DSurface();
    HELIUM_ASSERT( pD3DRenderSurface );
    BOOL bSrgb = static_cast< D3D9Surface* >( pRenderTargetSurface )->IsSrgb();

    IDirect3DSurface9* pD3DDepthSurface = NULL;
    if( pDepthStencilSurface )
    {
        pD3DDepthSurface = static_cast< D3D9Surface* >( pDepthStencilSurface )->GetD3DSurface();
        HELIUM_ASSERT( pD3DDepthSurface );
    }

    HELIUM_D3D9_VERIFY( m_pDevice->SetRenderTarget( 0, pD3DRenderSurface ) );
    HELIUM_D3D9_VERIFY( m_pDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, bSrgb ) );
    HELIUM_D3D9_VERIFY( m_pDevice->SetDepthStencilSurface( pD3DDepthSurface ) );
}

/// @copydoc RRenderCommandProxy::SetViewport()
void D3D9ImmediateCommandProxy::SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height )
{
    D3DVIEWPORT9 viewport;
    viewport.X = x;
    viewport.Y = y;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;

    HELIUM_D3D9_VERIFY( m_pDevice->SetViewport( &viewport ) );
}

/// @copydoc RRenderCommandProxy::BeginScene()
void D3D9ImmediateCommandProxy::BeginScene()
{
    HELIUM_D3D9_VERIFY( m_pDevice->BeginScene() );
}

/// @copydoc RRenderCommandProxy::EndScene()
void D3D9ImmediateCommandProxy::EndScene()
{
    HELIUM_D3D9_VERIFY( m_pDevice->EndScene() );
}

/// @copydoc RRenderCommandProxy::Clear()
void D3D9ImmediateCommandProxy::Clear( uint32_t clearFlags, const Color& rColor, float32_t depth, uint8_t stencil )
{
    DWORD d3d9Flags = 0;
    if( clearFlags & RENDERER_CLEAR_FLAG_TARGET )
    {
        d3d9Flags |= D3DCLEAR_TARGET;
    }

    if( clearFlags & RENDERER_CLEAR_FLAG_DEPTH )
    {
        d3d9Flags |= D3DCLEAR_ZBUFFER;
    }

    if( clearFlags & RENDERER_CLEAR_FLAG_STENCIL )
    {
        d3d9Flags |= D3DCLEAR_STENCIL;
    }

    HELIUM_D3D9_VERIFY( m_pDevice->Clear( 0, NULL, d3d9Flags, rColor.GetArgb(), depth, stencil ) );
}

/// @copydoc RRenderCommandProxy::SetIndexBuffer()
void D3D9ImmediateCommandProxy::SetIndexBuffer( RIndexBuffer* pBuffer )
{
    IDirect3DIndexBuffer9* pD3DBuffer = NULL;
    if( pBuffer )
    {
        pD3DBuffer = static_cast< D3D9IndexBuffer* >( pBuffer )->GetD3DBuffer();
    }

    HELIUM_D3D9_VERIFY( m_pDevice->SetIndices( pD3DBuffer ) );
}

/// @copydoc RRenderCommandProxy::SetVertexBuffers()
void D3D9ImmediateCommandProxy::SetVertexBuffers(
    size_t startIndex,
    size_t bufferCount,
    RVertexBuffer* const* ppBuffers,
    uint32_t* pStrides,
    uint32_t* pOffsets )
{
    HELIUM_ASSERT( ppBuffers || bufferCount == 0 );
    HELIUM_ASSERT( pStrides || bufferCount == 0 );
    HELIUM_ASSERT( pOffsets || bufferCount == 0 );

    if( startIndex >= STREAM_SOURCE_COUNT )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "D3D9ImmediateCommandProxy::SetVertexBuffers(): Start index (%" PRIuSZ ") exceeds the number of stream inputs available (%" PRIuSZ ").\n",
            startIndex,
            STREAM_SOURCE_COUNT );

        return;
    }

    size_t bufferCountMax = STREAM_SOURCE_COUNT - startIndex;
    if( bufferCount > bufferCountMax )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "D3D9ImmediateCommandProxy::SetVertexBuffers(): Input vertex buffer array (start index: %" PRIuSZ "; buffer count: %" PRIuSZ ") exceeds the available stream input range (%" PRIuSZ ").  Vertex buffer range will be clamped.\n",
            startIndex,
            bufferCount,
            STREAM_SOURCE_COUNT );

        bufferCount = bufferCountMax;
    }

    size_t bufferMax = startIndex + bufferCount;
    for( size_t bufferIndex = startIndex; bufferIndex < bufferMax; ++bufferIndex )
    {
        RVertexBuffer* pBuffer = static_cast< RVertexBuffer* >( *ppBuffers );
        ++ppBuffers;

        uint32_t stride = *pStrides;
        ++pStrides;

        uint32_t offset = *pOffsets;
        ++pOffsets;

        IDirect3DVertexBuffer9* pD3DBuffer = NULL;
        if( pBuffer )
        {
            pD3DBuffer = static_cast< D3D9VertexBuffer* >( pBuffer )->GetD3DBuffer();
            HELIUM_ASSERT( pD3DBuffer );
        }

        HELIUM_D3D9_VERIFY( m_pDevice->SetStreamSource(
            static_cast< UINT >( bufferIndex ),
            pD3DBuffer,
            offset,
            stride ) );
    }
}

/// @copydoc RRenderCommandProxy::SetVertexInputLayout()
void D3D9ImmediateCommandProxy::SetVertexInputLayout( RVertexInputLayout* pLayout )
{
    IDirect3DVertexDeclaration9* pD3DDeclaration = NULL;
    if( pLayout )
    {
        pD3DDeclaration = static_cast< D3D9VertexInputLayout* >( pLayout )->GetD3DDeclaration();
        HELIUM_ASSERT( pD3DDeclaration );
    }

    HELIUM_D3D9_VERIFY( m_pDevice->SetVertexDeclaration( pD3DDeclaration ) );
}

/// @copydoc RRenderCommandProxy::SetVertexShader()
void D3D9ImmediateCommandProxy::SetVertexShader( RVertexShader* pShader )
{
    IDirect3DVertexShader9* pD3DShader = NULL;
    if( pShader )
    {
        pD3DShader = static_cast< D3D9VertexShader* >( pShader )->GetD3DShader();
        HELIUM_ASSERT( pD3DShader );
    }

    HELIUM_D3D9_VERIFY( m_pDevice->SetVertexShader( pD3DShader ) );
}

/// @copydoc RRenderCommandProxy::SetPixelShader()
void D3D9ImmediateCommandProxy::SetPixelShader( RPixelShader* pShader )
{
    IDirect3DPixelShader9* pD3DShader = NULL;
    if( pShader )
    {
        pD3DShader = static_cast< D3D9PixelShader* >( pShader )->GetD3DShader();
        HELIUM_ASSERT( pD3DShader );
    }

    HELIUM_D3D9_VERIFY( m_pDevice->SetPixelShader( pD3DShader ) );
}

/// @copydoc RRenderCommandProxy::SetVertexConstantBuffers()
void D3D9ImmediateCommandProxy::SetVertexConstantBuffers(
    size_t startIndex,
    size_t bufferCount,
    RConstantBuffer* const* ppBuffers,
    const size_t* pLimitSizes )
{
    HELIUM_ASSERT( ppBuffers || bufferCount == 0 );

    if( startIndex >= CONSTANT_BUFFER_SLOT_COUNT )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "D3D9ImmediateCommandProxy::SetVertexConstantBuffers(): Start index (%" PRIuSZ ") exceeds the range allowed by the number of constant buffer slots (%" PRIuSZ ").\n",
            startIndex,
            CONSTANT_BUFFER_SLOT_COUNT );

        return;
    }

    size_t availableSlots = CONSTANT_BUFFER_SLOT_COUNT - startIndex;
    if( availableSlots < bufferCount )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "D3D9ImmediateCommandProxy::SetVertexConstantBuffers(): Buffer range (start: %" PRIuSZ "; count: %" PRIuSZ ") exceeds the range allowed by the number of constant buffer slots (%" PRIuSZ ").  Range will be clamped.\n",
            startIndex,
            bufferCount,
            CONSTANT_BUFFER_SLOT_COUNT );

        bufferCount = availableSlots;
    }

    if( pLimitSizes )
    {
        for( size_t bufferIndex = 0; bufferIndex < bufferCount; ++bufferIndex )
        {
            m_vertexConstantManager.SetBuffer(
                startIndex + bufferIndex,
                static_cast< D3D9ConstantBuffer* >( ppBuffers[ bufferIndex ] ),
                pLimitSizes[ bufferIndex ] );
        }
    }
    else
    {
        for( size_t bufferIndex = 0; bufferIndex < bufferCount; ++bufferIndex )
        {
            m_vertexConstantManager.SetBuffer(
                startIndex + bufferIndex,
                static_cast< D3D9ConstantBuffer* >( ppBuffers[ bufferIndex ] ),
                Invalid< size_t >() );
        }
    }
}

/// @copydoc RRenderCommandProxy::SetPixelConstantBuffers()
void D3D9ImmediateCommandProxy::SetPixelConstantBuffers(
    size_t startIndex,
    size_t bufferCount,
    RConstantBuffer* const* ppBuffers,
    const size_t* pLimitSizes )
{
    HELIUM_ASSERT( ppBuffers || bufferCount == 0 );

    if( startIndex >= CONSTANT_BUFFER_SLOT_COUNT )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "D3D9ImmediateCommandProxy::SetPixelConstantBuffers(): Start index (%" PRIuSZ ") exceeds the range allowed by the number of constant buffer slots (%" PRIuSZ ").\n",
            startIndex,
            CONSTANT_BUFFER_SLOT_COUNT );

        return;
    }

    size_t availableSlots = CONSTANT_BUFFER_SLOT_COUNT - startIndex;
    if( availableSlots < bufferCount )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "D3D9ImmediateCommandProxy::SetPixelConstantBuffers(): Buffer range (start: %" PRIuSZ "; count: %" PRIuSZ ") exceeds the range allowed by the number of constant buffer slots (%" PRIuSZ ").  Range will be clamped.\n",
            startIndex,
            bufferCount,
            CONSTANT_BUFFER_SLOT_COUNT );

        bufferCount = availableSlots;
    }

    if( pLimitSizes )
    {
        for( size_t bufferIndex = 0; bufferIndex < bufferCount; ++bufferIndex )
        {
            m_pixelConstantManager.SetBuffer(
                startIndex + bufferIndex,
                static_cast< D3D9ConstantBuffer* >( ppBuffers[ bufferIndex ] ),
                pLimitSizes[ bufferIndex ] );
        }
    }
    else
    {
        for( size_t bufferIndex = 0; bufferIndex < bufferCount; ++bufferIndex )
        {
            m_pixelConstantManager.SetBuffer(
                startIndex + bufferIndex,
                static_cast< D3D9ConstantBuffer* >( ppBuffers[ bufferIndex ] ),
                Invalid< size_t >() );
        }
    }
}

/// @copydoc RRenderCommandProxy::SetTexture()
void D3D9ImmediateCommandProxy::SetTexture( size_t samplerIndex, RTexture* pTexture )
{
    HELIUM_ASSERT( samplerIndex < HELIUM_ARRAY_COUNT( m_textures ) );
    if( samplerIndex >= HELIUM_ARRAY_COUNT( m_textures ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "D3D9ImmediateCommandProxy::SetTexture(): Sampler index %" PRIuSZ " exceeds the number of sampler stages available (%" PRIuSZ ").\n",
            samplerIndex,
            HELIUM_ARRAY_COUNT( m_textures ) );

        return;
    }

    RTexture* pExistingTexture = m_textures[ samplerIndex ];
    if( pExistingTexture == pTexture )
    {
        return;
    }

    m_textures[ samplerIndex ] = pTexture;

    BOOL bSrgb = FALSE;

    IDirect3DBaseTexture9* pD3DTexture = NULL;
    if( pTexture )
    {
        switch( pTexture->GetType() )
        {
            case RTexture::TYPE_2D:
            {
                pD3DTexture = static_cast< D3D9Texture2d* >( pTexture )->GetD3DTexture();
                bSrgb = static_cast< D3D9Texture2d* >( pTexture )->IsSrgb();

                break;
            }
        }
    }

    HELIUM_D3D9_VERIFY( m_pDevice->SetTexture( static_cast< DWORD >( samplerIndex ), pD3DTexture ) );

    if( pTexture )
    {
        uint32_t samplerBitMask = ( 1U << samplerIndex );

        bool bUpdateSrgb = true;
        if( pExistingTexture )
        {
            bool bWasSrgb = ( ( m_srgbTextureFlags & samplerBitMask ) != 0 );
            bUpdateSrgb = ( bSrgb ? !bWasSrgb : bWasSrgb );
        }

        if( bUpdateSrgb )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetSamplerState(
                static_cast< DWORD >( samplerIndex ),
                D3DSAMP_SRGBTEXTURE,
                bSrgb ) );
            if( bSrgb )
            {
                m_srgbTextureFlags |= samplerBitMask;
            }
            else
            {
                m_srgbTextureFlags &= ~samplerBitMask;
            }
        }
    }
}

/// @copydoc RRenderCommandProxy::DrawIndexed()
void D3D9ImmediateCommandProxy::DrawIndexed(
    ERendererPrimitiveType primitiveType,
    uint32_t baseVertexIndex,
    uint32_t minIndex,
    uint32_t usedVertexCount,
    uint32_t startIndex,
    uint32_t primitiveCount )
{
    HELIUM_ASSERT( static_cast< size_t >( primitiveType ) < static_cast< size_t >( RENDERER_PRIMITIVE_TYPE_MAX ) );

    static const D3DPRIMITIVETYPE d3dPrimitiveTypes[] =
    {
        // RENDERER_PRIMITIVE_TYPE_POINT_LIST
        D3DPT_POINTLIST,
        // RENDERER_PRIMITIVE_TYPE_LINE_LIST
        D3DPT_LINELIST,
        // RENDERER_PRIMITIVE_TYPE_LINE_STRIP
        D3DPT_LINESTRIP,
        // RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST
        D3DPT_TRIANGLELIST,
        // RENDERER_PRIMITIVE_TYPE_TRIANGLE_STRIP
        D3DPT_TRIANGLESTRIP,
        // RENDERER_PRIMITIVE_TYPE_TRIANGLE_FAN
        D3DPT_TRIANGLEFAN,
    };

    HELIUM_COMPILE_ASSERT( HELIUM_ARRAY_COUNT( d3dPrimitiveTypes ) == RENDERER_PRIMITIVE_TYPE_MAX );

    m_vertexConstantManager.Push( m_pDevice );
    m_pixelConstantManager.Push( m_pDevice );

    HELIUM_D3D9_VERIFY( m_pDevice->DrawIndexedPrimitive(
        d3dPrimitiveTypes[ primitiveType ],
        baseVertexIndex,
        minIndex,
        usedVertexCount,
        startIndex,
        primitiveCount ) );
}

/// @copydoc RRenderCommandProxy::DrawUnindexed()
void D3D9ImmediateCommandProxy::DrawUnindexed(
    ERendererPrimitiveType primitiveType,
    uint32_t baseVertexIndex,
    uint32_t primitiveCount )
{
    HELIUM_ASSERT( static_cast< size_t >( primitiveType ) < static_cast< size_t >( RENDERER_PRIMITIVE_TYPE_MAX ) );

    static const D3DPRIMITIVETYPE d3dPrimitiveTypes[] =
    {
        // RENDERER_PRIMITIVE_TYPE_POINT_LIST
        D3DPT_POINTLIST,
        // RENDERER_PRIMITIVE_TYPE_LINE_LIST
        D3DPT_LINELIST,
        // RENDERER_PRIMITIVE_TYPE_LINE_STRIP
        D3DPT_LINESTRIP,
        // RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST
        D3DPT_TRIANGLELIST,
        // RENDERER_PRIMITIVE_TYPE_TRIANGLE_STRIP
        D3DPT_TRIANGLESTRIP,
        // RENDERER_PRIMITIVE_TYPE_TRIANGLE_FAN
        D3DPT_TRIANGLEFAN,
    };

    HELIUM_COMPILE_ASSERT( HELIUM_ARRAY_COUNT( d3dPrimitiveTypes ) == RENDERER_PRIMITIVE_TYPE_MAX );

    m_vertexConstantManager.Push( m_pDevice );
    m_pixelConstantManager.Push( m_pDevice );

    HELIUM_D3D9_VERIFY( m_pDevice->DrawPrimitive( d3dPrimitiveTypes[ primitiveType ], baseVertexIndex, primitiveCount ) );
}

/// @copydoc RRenderCommandProxy::SetFence()
void D3D9ImmediateCommandProxy::SetFence( RFence* pFence )
{
    HELIUM_ASSERT( pFence );

    IDirect3DQuery9* pD3DQuery = static_cast< D3D9Fence* >( pFence )->GetQuery();
    HELIUM_ASSERT( pD3DQuery );

    HELIUM_D3D9_VERIFY( pD3DQuery->Issue( D3DISSUE_END ) );
}

/// @copydoc RRenderCommandProxy::UnbindResources()
void D3D9ImmediateCommandProxy::UnbindResources()
{
    m_spRasterizerState.Release();
    m_spBlendState.Release();
    m_spDepthStencilState.Release();

    for( size_t samplerIndex = 0; samplerIndex < HELIUM_ARRAY_COUNT( m_samplerStates ); ++samplerIndex )
    {
        m_samplerStates[ samplerIndex ].Release();

        if( m_textures[ samplerIndex ] )
        {
            HELIUM_D3D9_VERIFY( m_pDevice->SetTexture( static_cast< DWORD >( samplerIndex ), NULL ) );
            m_textures[ samplerIndex ].Release();
        }
    }

    HELIUM_D3D9_VERIFY( m_pDevice->SetIndices( NULL ) );

    DWORD streamSourceCount = static_cast< DWORD >( STREAM_SOURCE_COUNT );
    for( DWORD streamSourceIndex = 0; streamSourceIndex < streamSourceCount; ++streamSourceIndex )
    {
        HELIUM_D3D9_VERIFY( m_pDevice->SetStreamSource( streamSourceIndex, NULL, 0, 0 ) );
    }

    for( size_t constantBufferIndex = 0; constantBufferIndex < CONSTANT_BUFFER_SLOT_COUNT; ++constantBufferIndex )
    {
        m_vertexConstantManager.SetBuffer( constantBufferIndex, NULL, Invalid< size_t >() );
        m_pixelConstantManager.SetBuffer( constantBufferIndex, NULL, Invalid< size_t >() );
    }
}

/// @copydoc RRenderCommandProxy::ExecuteCommandList()
void D3D9ImmediateCommandProxy::ExecuteCommandList( RRenderCommandList* pCommandList )
{
    HELIUM_ASSERT( pCommandList );

    D3D9RenderCommandList* pRenderCommandList = static_cast< D3D9RenderCommandList* >( pCommandList );

    D3D9RenderCommandList::Iterator listEnd = pRenderCommandList->End();
    for( D3D9RenderCommandList::Iterator listIter = pRenderCommandList->Begin(); listIter != listEnd; ++listIter )
    {
        D3D9RenderCommand& rCommand = *listIter;
        rCommand.Execute( this );
    }
}

/// @copydoc RRenderCommandProxy::FinishCommandList()
void D3D9ImmediateCommandProxy::FinishCommandList( RRenderCommandListPtr& rspCommandList )
{
    HELIUM_TRACE(
        TraceLevels::Error,
        "D3D9ImmediateCommandProxy: FinishCommandList() called on an immediate command proxy.\n");

    HELIUM_BREAK_MSG(
        "D3D9ImmediateCommandProxy: FinishCommandList() called on an immediate command proxy");

    rspCommandList.Release();
}

/// Constructor.
template< typename Pusher, size_t RegisterCount >
D3D9ImmediateCommandProxy::ConstantManager< Pusher, RegisterCount >::ConstantManager()
{
}

/// Destructor.
template< typename Pusher, size_t RegisterCount >
D3D9ImmediateCommandProxy::ConstantManager< Pusher, RegisterCount >::~ConstantManager()
{
}

/// Assign a constant buffer to the specified slot.
///
/// @param[in] index      Constant buffer slot index.
/// @param[in] pBuffer    Constant buffer to set.
/// @param[in] limitSize  Number of bytes, starting from the beginning of the buffer, in which to limit updates to
///                       shader constant registers.
///
/// @see GetBuffer()
template< typename Pusher, size_t RegisterCount >
void D3D9ImmediateCommandProxy::ConstantManager< Pusher, RegisterCount >::SetBuffer(
    size_t index,
    D3D9ConstantBuffer* pBuffer,
    size_t limitSize )
{
    HELIUM_ASSERT( index < HELIUM_ARRAY_COUNT( m_buffers ) );

    // Convert constant buffer size limits from bytes to register counts.
    if( IsValid( limitSize ) )
    {
        m_bufferLimitSizes[ index ] = static_cast< uint16_t >( Min< size_t >(
            ( limitSize + ( sizeof( float32_t ) * 4 - 1 ) ) / ( sizeof( float32_t ) * 4 ),
            UINT16_MAX ) );
    }
    else
    {
        SetInvalid( m_bufferLimitSizes[ index ] );
    }

    D3D9ConstantBuffer* pOldBuffer = m_buffers[ index ];
    if( pOldBuffer != pBuffer )
    {
        uint_fast16_t oldRegisterCount = 0;
        if( pOldBuffer )
        {
            oldRegisterCount = pOldBuffer->GetRegisterCount();
        }

        uint_fast16_t newRegisterCount = 0;
        if( pBuffer )
        {
            newRegisterCount = pBuffer->GetRegisterCount();
        }

        if( oldRegisterCount != newRegisterCount )
        {
            // Register count changed, so invalidate all registers in buffers that follow the one being assigned.
            uint_fast16_t invalidRegisterStart = newRegisterCount;
            for( size_t previousIndex = 0; previousIndex < index; ++previousIndex )
            {
                D3D9ConstantBuffer* pPreviousBuffer = m_buffers[ previousIndex ];
                if( pPreviousBuffer )
                {
                    invalidRegisterStart += pPreviousBuffer->GetRegisterCount();
                }
            }

            uint_fast16_t invalidRegisterElementIndex = invalidRegisterStart / ( sizeof( uint32_t ) * 8 );
            if( invalidRegisterElementIndex < HELIUM_ARRAY_COUNT( m_dirtyRegisters ) )
            {
                uint_fast16_t invalidRegisterBit = invalidRegisterStart % ( sizeof( uint32_t ) * 8 );
                if( invalidRegisterBit != 0 )
                {
                    uint32_t bitMask = ~( ( 1U << invalidRegisterBit ) - 1 );
                    m_dirtyRegisters[ invalidRegisterElementIndex ] |= bitMask;

                    ++invalidRegisterElementIndex;
                }

                MemorySet(
                    m_dirtyRegisters,
                    0xff,
                    ( HELIUM_ARRAY_COUNT( m_dirtyRegisters ) - invalidRegisterElementIndex ) * sizeof( uint32_t ) );
            }
        }

        m_buffers[ index ] = pBuffer;
        if( pBuffer )
        {
            // Set the buffer tag as one minus its actual tag to force its contents to be updated during the next
            // Push() call.
            m_bufferTags[ index ] = pBuffer->GetTag() - 1;
        }
    }
}

/// Get the constant buffer assigned to the specified slot.
///
/// @param[in] index  Constant buffer slot index.
///
/// @return  Constant buffer assigned to the slot.
///
/// @see SetBuffer()
template< typename Pusher, size_t RegisterCount >
D3D9ConstantBuffer* D3D9ImmediateCommandProxy::ConstantManager< Pusher, RegisterCount >::GetBuffer(
    size_t index ) const
{
    HELIUM_ASSERT( index < HELIUM_ARRAY_COUNT( m_pBuffers ) );

    return m_buffers[ index ];
}

/// Push shader constant changes to the rendering device.
///
/// @param[in] pDevice  Direct3D device instance.
template< typename Pusher, size_t RegisterCount >
void D3D9ImmediateCommandProxy::ConstantManager< Pusher, RegisterCount >::Push( IDirect3DDevice9* pDevice )
{
    HELIUM_ASSERT( pDevice );

    uint_fast16_t registerIndex = 0;

    size_t dirtyRegisterElementIndex = 0;
    uint32_t dirtyRegisterBitMask = 1;

    uint32_t dirtyRegisterFlags = m_dirtyRegisters[ 0 ];

    for( size_t bufferIndex = 0; bufferIndex < HELIUM_ARRAY_COUNT( m_buffers ); ++bufferIndex )
    {
        D3D9ConstantBuffer* pBuffer = m_buffers[ bufferIndex ];
        if( !pBuffer )
        {
            continue;
        }

        // If the buffer tag has changed (buffer has been updated), the entire buffer needs to be re-pushed.
        bool bTagChanged = false;

        uint32_t bufferTag = pBuffer->GetTag();
        if( bufferTag != m_bufferTags[ bufferIndex ] )
        {
            bTagChanged = true;
            m_bufferTags[ bufferIndex ] = bufferTag;
        }

        // Push dirty registers.
        const float32_t* pData = static_cast< const float32_t* >( pBuffer->GetData() );
        uint_fast16_t bufferRegisterCount = pBuffer->GetRegisterCount();
        HELIUM_ASSERT( pData || bufferRegisterCount == 0 );

        uint_fast16_t bufferRegisterLimit = Min< uint_fast16_t >(
            m_bufferLimitSizes[ bufferIndex ],
            bufferRegisterCount );

        uint_fast16_t pushStartRegister = registerIndex;
        const float32_t* pPushStartBufferAddress = pData;
        uint_fast16_t pushRegisterCount = 0;

        for( uint_fast16_t bufferRegisterOffset = 0;
            bufferRegisterOffset < bufferRegisterLimit;
            ++bufferRegisterOffset )
        {
            pData += 4;
            ++registerIndex;

            if( bTagChanged || dirtyRegisterFlags & dirtyRegisterBitMask )
            {
                // Register needs to be updated, so increment the number of registers to push.
                ++pushRegisterCount;

                // Clear the dirty flag for the current register.
                dirtyRegisterFlags &= ~dirtyRegisterBitMask;
            }
            else
            {
                // Found a clean register, so push any dirty registers immediately prior to the current register.
                if( pushRegisterCount != 0 )
                {
                    m_pusher( pDevice, pushStartRegister, pPushStartBufferAddress, pushRegisterCount );
                    pushRegisterCount = 0;
                }

                pushStartRegister = registerIndex;
                pPushStartBufferAddress = pData;
            }

            // Check if we've reached the limit of hardware registers available.
            HELIUM_ASSERT( registerIndex <= RegisterCount );
            if( registerIndex >= RegisterCount )
            {
                break;
            }

            // Update the dirty register bit mask information.
            dirtyRegisterBitMask <<= 1;
            if( dirtyRegisterBitMask == 0 )
            {
                m_dirtyRegisters[ dirtyRegisterElementIndex ] = dirtyRegisterFlags;

                ++dirtyRegisterElementIndex;
                dirtyRegisterFlags = m_dirtyRegisters[ dirtyRegisterElementIndex ];
                dirtyRegisterBitMask = 1;
            }
        }

        // Push all remaining dirty register values in the current buffer.
        if( pushRegisterCount != 0 )
        {
            m_pusher( pDevice, pushStartRegister, pPushStartBufferAddress, pushRegisterCount );
        }

        // Store updated dirty register flags for the current dirty register bit array element.
        m_dirtyRegisters[ dirtyRegisterElementIndex ] = dirtyRegisterFlags;

        // Check if we've reached the limit of hardware registers available.
        HELIUM_ASSERT( registerIndex <= RegisterCount );
        if( registerIndex >= RegisterCount )
        {
            break;
        }

        // Update the current register index and the dirty register bit mask information for any ignored registers
        // in the buffer.
        uint_fast16_t skippedRegisterCount = bufferRegisterCount - bufferRegisterLimit;
        if( RegisterCount - registerIndex <= skippedRegisterCount )
        {
            break;
        }

        registerIndex += skippedRegisterCount;

        dirtyRegisterElementIndex = registerIndex / ( sizeof( uint32_t ) * 8 );
        dirtyRegisterBitMask = ( 1U << ( registerIndex % ( sizeof( uint32_t ) * 8 ) ) );

        dirtyRegisterFlags = m_dirtyRegisters[ dirtyRegisterElementIndex ];
    }
}

/// Push vertex shader constants to the rendering device.
///
/// @param[in] pDevice     Direct3D device interface.
/// @param[in] startIndex  Starting register index.
/// @param[in] pData       Shader constant data to push.
/// @param[in] count       Number of constants to push.
void D3D9ImmediateCommandProxy::VertexShaderConstantPusher::operator()(
    IDirect3DDevice9* pDevice,
    UINT startIndex,
    const float32_t* pData,
    UINT count )
{
    HELIUM_ASSERT( pDevice );

    HRESULT hResult = pDevice->SetVertexShaderConstantF( startIndex, pData, count );
    if( FAILED( hResult ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "D3D9ImmediateCommandProxy::VertexShaderConstantPusher(): Failed to update %u constants starting at register %u.\n",
            count,
            startIndex );
    }
}

/// Push pixel shader constants to the rendering device.
///
/// @param[in] pDevice     Direct3D device interface.
/// @param[in] startIndex  Starting register index.
/// @param[in] pData       Shader constant data to push.
/// @param[in] count       Number of constants to push.
void D3D9ImmediateCommandProxy::PixelShaderConstantPusher::operator()(
    IDirect3DDevice9* pDevice,
    UINT startIndex,
    const float32_t* pData,
    UINT count )
{
    HELIUM_ASSERT( pDevice );

    HRESULT hResult = pDevice->SetPixelShaderConstantF( startIndex, pData, count );
    if( FAILED( hResult ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "D3D9ImmediateCommandProxy::PixelShaderConstantPusher(): Failed to update %u constants starting at register %u.\n",
            count,
            startIndex );
    }
}
