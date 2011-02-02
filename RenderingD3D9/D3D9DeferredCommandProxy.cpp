//----------------------------------------------------------------------------------------------------------------------
// D3D9DeferredCommandProxy.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingD3D9Pch.h"
#include "RenderingD3D9/D3D9DeferredCommandProxy.h"

#include "Rendering/RConstantBuffer.h"
#include "Rendering/RFence.h"
#include "Rendering/RIndexBuffer.h"
#include "Rendering/RPixelShader.h"
#include "Rendering/RSurface.h"
#include "Rendering/RVertexBuffer.h"
#include "Rendering/RVertexInputLayout.h"
#include "Rendering/RVertexShader.h"
#include "RenderingD3D9/D3D9ImmediateCommandProxy.h"
#include "RenderingD3D9/D3D9RenderCommandList.h"

namespace Lunar
{
    L_DECLARE_RPTR( RRasterizerState );
    L_DECLARE_RPTR( RBlendState );
    L_DECLARE_RPTR( RDepthStencilState );

    L_DECLARE_RPTR( RSurface );

    L_DECLARE_RPTR( RIndexBuffer );
    L_DECLARE_RPTR( RVertexInputLayout );

    L_DECLARE_RPTR( RVertexShader );
    L_DECLARE_RPTR( RPixelShader );

    L_DECLARE_RPTR( RTexture );

    L_DECLARE_RPTR( RFence );
}

using namespace Lunar;

class D3D9SetRasterizerStateCommand : public D3D9RenderCommand
{
public:
    D3D9SetRasterizerStateCommand( RRasterizerState* pState )
        : m_spState( pState )
    {
    }

    ~D3D9SetRasterizerStateCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->SetRasterizerState( m_spState );
    }

private:
    RRasterizerStatePtr m_spState;
};

class D3D9SetBlendStateCommand : public D3D9RenderCommand
{
public:
    D3D9SetBlendStateCommand( RBlendState* pState )
        : m_spState( pState )
    {
    }

    ~D3D9SetBlendStateCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->SetBlendState( m_spState );
    }

private:
    RBlendStatePtr m_spState;
};

class D3D9SetDepthStencilStateCommand : public D3D9RenderCommand
{
public:
    D3D9SetDepthStencilStateCommand( RDepthStencilState* pState, uint8_t stencilReferenceValue )
        : m_spState( pState )
        , m_stencilReferenceValue( stencilReferenceValue )
    {
    }

    ~D3D9SetDepthStencilStateCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->SetDepthStencilState( m_spState, m_stencilReferenceValue );
    }

private:
    RDepthStencilStatePtr m_spState;
    uint8_t m_stencilReferenceValue;
};

class D3D9SetSamplerStatesCommand : public D3D9RenderCommand
{
public:
    static const size_t STATE_COUNT_MAX = 16;

    D3D9SetSamplerStatesCommand( size_t startIndex, size_t samplerCount, RSamplerState* const* ppStates )
        : m_startIndex( startIndex )
    {
        HELIUM_ASSERT_MSG(
            samplerCount < HELIUM_ARRAY_COUNT( m_states ),
            ( TXT( "D3D9DeferredCommandProxy: Sampler state count exceeds the maximum supported for deferred " )
            TXT( "render commands (16)" ) ) );
        samplerCount = Min( samplerCount, HELIUM_ARRAY_COUNT( m_states ) );
        m_samplerCount = samplerCount;

        HELIUM_ASSERT( ppStates || samplerCount == 0 );

        for( size_t samplerIndex = 0; samplerIndex < samplerCount; ++samplerIndex )
        {
            m_states[ samplerIndex ] = ppStates[ samplerIndex ];
        }
    }

    ~D3D9SetSamplerStatesCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->SetSamplerStates(
            m_startIndex,
            m_samplerCount,
            &static_cast< RSamplerState* const& >( m_states[ 0 ] ) );
    }

private:
    size_t m_startIndex;
    size_t m_samplerCount;
    RSamplerStatePtr m_states[ STATE_COUNT_MAX ];
};

class D3D9SetRenderSurfacesCommand : public D3D9RenderCommand
{
public:
    D3D9SetRenderSurfacesCommand( RSurface* pRenderTargetSurface, RSurface* pDepthStencilSurface )
        : m_spRenderTargetSurface( pRenderTargetSurface )
        , m_spDepthStencilSurface( pDepthStencilSurface )
    {
    }

    ~D3D9SetRenderSurfacesCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->SetRenderSurfaces( m_spRenderTargetSurface, m_spDepthStencilSurface );
    }

private:
    RSurfacePtr m_spRenderTargetSurface;
    RSurfacePtr m_spDepthStencilSurface;
};

class D3D9SetViewportCommand : public D3D9RenderCommand
{
public:
    D3D9SetViewportCommand( uint32_t x, uint32_t y, uint32_t width, uint32_t height )
        : m_x( x )
        , m_y( y )
        , m_width( width )
        , m_height( height )
    {
    }

    ~D3D9SetViewportCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->SetViewport( m_x, m_y, m_width, m_height );
    }

private:
    uint32_t m_x;
    uint32_t m_y;
    uint32_t m_width;
    uint32_t m_height;
};

class D3D9BeginSceneCommand : public D3D9RenderCommand
{
public:
    D3D9BeginSceneCommand()
    {
    }

    ~D3D9BeginSceneCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->BeginScene();
    }
};

class D3D9EndSceneCommand : public D3D9RenderCommand
{
public:
    D3D9EndSceneCommand()
    {
    }

    ~D3D9EndSceneCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->EndScene();
    }
};

class D3D9ClearCommand : public D3D9RenderCommand
{
public:
    D3D9ClearCommand( uint32_t clearFlags, const Color& rColor, float32_t depth, uint8_t stencil )
        : m_clearFlags( clearFlags )
        , m_color( rColor )
        , m_depth( depth )
        , m_stencil( stencil )
    {
    }

    ~D3D9ClearCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->Clear( m_clearFlags, m_color, m_depth, m_stencil );
    }

private:
    uint32_t m_clearFlags;
    Color m_color;
    float32_t m_depth;
    uint8_t m_stencil;
};

class D3D9SetIndexBufferCommand : public D3D9RenderCommand
{
public:
    D3D9SetIndexBufferCommand( RIndexBuffer* pBuffer )
        : m_spBuffer( pBuffer )
    {
    }

    ~D3D9SetIndexBufferCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->SetIndexBuffer( m_spBuffer );
    }

private:
    RIndexBufferPtr m_spBuffer;
};

class D3D9SetVertexBuffersCommand : public D3D9RenderCommand
{
public:
    static const size_t BUFFER_COUNT_MAX = 16;

    D3D9SetVertexBuffersCommand(
        size_t startIndex,
        size_t bufferCount,
        RVertexBuffer* const* ppBuffers,
        uint32_t* pStrides,
        uint32_t* pOffsets )
        : m_startIndex( startIndex )
        , m_bufferCount( bufferCount )
    {
        HELIUM_ASSERT_MSG(
            bufferCount < HELIUM_ARRAY_COUNT( m_buffers ),
            ( TXT( "D3D9DeferredCommandProxy: Vertex buffer count exceeds the maximum supported for deferred " )
            TXT( "render commands (16)" ) ) );
        bufferCount = Min( bufferCount, HELIUM_ARRAY_COUNT( m_buffers ) );
        m_bufferCount = bufferCount;

        HELIUM_ASSERT( ppBuffers || bufferCount == 0 );
        HELIUM_ASSERT( pStrides || bufferCount == 0 );
        HELIUM_ASSERT( pOffsets || bufferCount == 0 );

        for( size_t bufferIndex = 0; bufferIndex < bufferCount; ++bufferIndex )
        {
            m_buffers[ bufferIndex ] = ppBuffers[ bufferIndex ];
        }

        MemoryCopy( m_strides, pStrides, sizeof( m_strides[ 0 ] ) * bufferCount );
        MemoryCopy( m_offsets, pOffsets, sizeof( m_offsets[ 0 ] ) * bufferCount );
    }

    ~D3D9SetVertexBuffersCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->SetVertexBuffers(
            m_startIndex,
            m_bufferCount,
            &static_cast< RVertexBuffer* const& >( m_buffers[ 0 ] ),
            m_strides,
            m_offsets );
    }

private:
    size_t m_startIndex;
    size_t m_bufferCount;
    RVertexBufferPtr m_buffers[ BUFFER_COUNT_MAX ];
    uint32_t m_strides[ BUFFER_COUNT_MAX ];
    uint32_t m_offsets[ BUFFER_COUNT_MAX ];
};

class D3D9SetVertexInputLayoutCommand : public D3D9RenderCommand
{
public:
    D3D9SetVertexInputLayoutCommand( RVertexInputLayout* pLayout )
        : m_spLayout( pLayout )
    {
    }

    ~D3D9SetVertexInputLayoutCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->SetVertexInputLayout( m_spLayout );
    }

private:
    RVertexInputLayoutPtr m_spLayout;
};

class D3D9SetVertexShaderCommand : public D3D9RenderCommand
{
public:
    D3D9SetVertexShaderCommand( RVertexShader* pShader )
        : m_spShader( pShader )
    {
    }

    ~D3D9SetVertexShaderCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->SetVertexShader( m_spShader );
    }

private:
    RVertexShaderPtr m_spShader;
};

class D3D9SetPixelShaderCommand : public D3D9RenderCommand
{
public:
    D3D9SetPixelShaderCommand( RPixelShader* pShader )
        : m_spShader( pShader )
    {
    }

    ~D3D9SetPixelShaderCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->SetPixelShader( m_spShader );
    }

private:
    RPixelShaderPtr m_spShader;
};

class D3D9SetConstantBuffersCommand : public D3D9RenderCommand
{
public:
    D3D9SetConstantBuffersCommand(
        size_t startIndex,
        size_t bufferCount,
        RConstantBuffer* const* ppBuffers,
        const size_t* pLimitSizes )
        : m_startIndex( startIndex )
        , m_bufferCount( bufferCount )
    {
        HELIUM_ASSERT_MSG(
            bufferCount < HELIUM_ARRAY_COUNT( m_buffers ),
            ( TXT( "D3D9DeferredCommandProxy: Constant buffer count exceeds the supported number of command " )
            TXT( "buffer slots" ) ) );
        bufferCount = Min( bufferCount, HELIUM_ARRAY_COUNT( m_buffers ) );
        m_bufferCount = bufferCount;

        HELIUM_ASSERT( ppBuffers || bufferCount == 0 );

        for( size_t bufferIndex = 0; bufferIndex < bufferCount; ++bufferIndex )
        {
            m_buffers[ bufferIndex ] = ppBuffers[ bufferIndex ];
        }

        if( pLimitSizes )
        {
            MemoryCopy( m_limitSizes, pLimitSizes, bufferCount * sizeof( size_t ) );
        }
        else
        {
            MemorySet( m_limitSizes, 0xff, bufferCount * sizeof( size_t ) );
        }
    }

    ~D3D9SetConstantBuffersCommand()
    {
    }

protected:
    size_t m_startIndex;
    size_t m_bufferCount;
    RConstantBufferPtr m_buffers[ D3D9ImmediateCommandProxy::CONSTANT_BUFFER_SLOT_COUNT ];
    size_t m_limitSizes[ D3D9ImmediateCommandProxy::CONSTANT_BUFFER_SLOT_COUNT ];
};

class D3D9SetVertexConstantBuffersCommand : public D3D9SetConstantBuffersCommand
{
public:
    D3D9SetVertexConstantBuffersCommand(
        size_t startIndex,
        size_t bufferCount,
        RConstantBuffer* const* ppBuffers,
        const size_t* pLimitSizes )
        : D3D9SetConstantBuffersCommand( startIndex, bufferCount, ppBuffers, pLimitSizes )
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->SetVertexConstantBuffers(
            m_startIndex,
            m_bufferCount,
            &static_cast< RConstantBuffer* const& >( m_buffers[ 0 ] ),
            m_limitSizes );
    }
};

class D3D9SetPixelConstantBuffersCommand : public D3D9SetConstantBuffersCommand
{
public:
    D3D9SetPixelConstantBuffersCommand(
        size_t startIndex,
        size_t bufferCount,
        RConstantBuffer* const* ppBuffers,
        const size_t* pLimitSizes )
        : D3D9SetConstantBuffersCommand( startIndex, bufferCount, ppBuffers, pLimitSizes )
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->SetPixelConstantBuffers(
            m_startIndex,
            m_bufferCount,
            &static_cast< RConstantBuffer* const& >( m_buffers[ 0 ] ),
            m_limitSizes );
    }
};

class D3D9SetTextureCommand : public D3D9RenderCommand
{
public:
    D3D9SetTextureCommand( size_t samplerIndex, RTexture* pTexture )
        : m_samplerIndex( samplerIndex )
        , m_spTexture( pTexture )
    {
    }

    ~D3D9SetTextureCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->SetTexture( m_samplerIndex, m_spTexture );
    }

private:
    size_t m_samplerIndex;
    RTexturePtr m_spTexture;
};

class D3D9DrawIndexedCommand : public D3D9RenderCommand
{
public:
    D3D9DrawIndexedCommand(
        ERendererPrimitiveType primitiveType,
        uint32_t baseVertexIndex,
        uint32_t minIndex,
        uint32_t usedVertexCount,
        uint32_t startIndex,
        uint32_t primitiveCount )
        : m_primitiveType( primitiveType )
        , m_baseVertexIndex( baseVertexIndex )
        , m_minIndex( minIndex )
        , m_usedVertexCount( usedVertexCount )
        , m_startIndex( startIndex )
        , m_primitiveCount( primitiveCount )
    {
    }

    ~D3D9DrawIndexedCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->DrawIndexed(
            m_primitiveType,
            m_baseVertexIndex,
            m_minIndex,
            m_usedVertexCount,
            m_startIndex,
            m_primitiveType );
    }

private:
    ERendererPrimitiveType m_primitiveType;
    uint32_t m_baseVertexIndex;
    uint32_t m_minIndex;
    uint32_t m_usedVertexCount;
    uint32_t m_startIndex;
    uint32_t m_primitiveCount;
};

class D3D9DrawUnindexedCommand : public D3D9RenderCommand
{
public:
    D3D9DrawUnindexedCommand( ERendererPrimitiveType primitiveType, uint32_t baseVertexIndex, uint32_t primitiveCount )
        : m_primitiveType( primitiveType )
        , m_baseVertexIndex( baseVertexIndex )
        , m_primitiveCount( primitiveCount )
    {
    }

    ~D3D9DrawUnindexedCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->DrawUnindexed( m_primitiveType, m_baseVertexIndex, m_primitiveType );
    }

private:
    ERendererPrimitiveType m_primitiveType;
    uint32_t m_baseVertexIndex;
    uint32_t m_primitiveCount;
};

class D3D9SetFenceCommand : public D3D9RenderCommand
{
public:
    D3D9SetFenceCommand( RFence* pFence )
        : m_spFence( pFence )
    {
    }

    ~D3D9SetFenceCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->SetFence( m_spFence );
    }

private:
    RFencePtr m_spFence;
};

class D3D9UnbindResourcesCommand : public D3D9RenderCommand
{
public:
    D3D9UnbindResourcesCommand()
    {
    }

    ~D3D9UnbindResourcesCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->UnbindResources();
    }
};

class D3D9ExecuteCommandListCommand : public D3D9RenderCommand
{
public:
    D3D9ExecuteCommandListCommand( RRenderCommandList* pCommandList )
        : m_spCommandList( pCommandList )
    {
    }

    ~D3D9ExecuteCommandListCommand()
    {
    }

    void Execute( D3D9ImmediateCommandProxy* pCommandProxy )
    {
        pCommandProxy->ExecuteCommandList( m_spCommandList );
    }

private:
    RRenderCommandListPtr m_spCommandList;
};

#define L_DEFERRED_COMMAND_PROXY_METHOD( COMMAND, PARAM_LIST, ARGUMENT_LIST ) \
    void D3D9DeferredCommandProxy::COMMAND PARAM_LIST \
    { \
        if( !m_spCommandList ) \
        { \
            m_spCommandList = new D3D9RenderCommandList; \
            HELIUM_ASSERT( m_spCommandList ); \
        } \
        \
        HELIUM_VERIFY( m_spCommandList->NewCommand< D3D9##COMMAND##Command > ARGUMENT_LIST ); \
    }

/// Constructor.
D3D9DeferredCommandProxy::D3D9DeferredCommandProxy()
{
}

/// Destructor.
D3D9DeferredCommandProxy::~D3D9DeferredCommandProxy()
{
}

L_DEFERRED_COMMAND_PROXY_METHOD(
    SetRasterizerState,
    ( RRasterizerState* pState ),
    ( pState ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    SetBlendState,
    ( RBlendState* pState ),
    ( pState ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    SetDepthStencilState,
    ( RDepthStencilState* pState, uint8_t stencilReferenceValue ),
    ( pState, stencilReferenceValue ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    SetSamplerStates,
    ( size_t startIndex, size_t samplerCount, RSamplerState* const* ppStates ),
    ( startIndex, samplerCount, ppStates ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    SetRenderSurfaces,
    ( RSurface* pRenderTargetSurface, RSurface* pDepthStencilSurface ),
    ( pRenderTargetSurface, pDepthStencilSurface ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    SetViewport,
    ( uint32_t x, uint32_t y, uint32_t width, uint32_t height ),
    ( x, y, width, height ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    BeginScene,
    (),
    () )

L_DEFERRED_COMMAND_PROXY_METHOD(
    EndScene,
    (),
    () )

L_DEFERRED_COMMAND_PROXY_METHOD(
    Clear,
    ( uint32_t clearFlags, const Color& rColor, float32_t depth, uint8_t stencil ),
    ( clearFlags, rColor, depth, stencil ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    SetIndexBuffer,
    ( RIndexBuffer* pBuffer ),
    ( pBuffer ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    SetVertexBuffers,
    ( size_t startIndex, size_t bufferCount, RVertexBuffer* const* ppBuffers, uint32_t* pStrides, uint32_t* pOffsets ),
    ( startIndex, bufferCount, ppBuffers, pStrides, pOffsets ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    SetVertexInputLayout,
    ( RVertexInputLayout* pLayout ),
    ( pLayout ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    SetVertexShader,
    ( RVertexShader* pShader ),
    ( pShader ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    SetPixelShader,
    ( RPixelShader* pShader ),
    ( pShader ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    SetVertexConstantBuffers,
    ( size_t startIndex, size_t bufferCount, RConstantBuffer* const* ppBuffers, const size_t* pLimitSizes ),
    ( startIndex, bufferCount, ppBuffers, pLimitSizes ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    SetPixelConstantBuffers,
    ( size_t startIndex, size_t bufferCount, RConstantBuffer* const* ppBuffers, const size_t* pLimitSizes ),
    ( startIndex, bufferCount, ppBuffers, pLimitSizes ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    SetTexture,
    ( size_t samplerIndex, RTexture* pTexture ),
    ( samplerIndex, pTexture ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    DrawIndexed,
    ( ERendererPrimitiveType primitiveType, uint32_t baseVertexIndex, uint32_t minIndex, uint32_t usedVertexCount,
      uint32_t startIndex, uint32_t primitiveCount ),
    ( primitiveType, baseVertexIndex, minIndex, usedVertexCount, startIndex, primitiveCount ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    DrawUnindexed,
    ( ERendererPrimitiveType primitiveType, uint32_t baseVertexIndex, uint32_t primitiveCount ),
    ( primitiveType, baseVertexIndex, primitiveCount ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    SetFence,
    ( RFence* pFence ),
    ( pFence ) )

L_DEFERRED_COMMAND_PROXY_METHOD(
    UnbindResources,
    (),
    () )

L_DEFERRED_COMMAND_PROXY_METHOD(
    ExecuteCommandList,
    ( RRenderCommandList* pCommandList ),
    ( pCommandList ) )

/// @copydoc D3D9DeferredCommandProxy::FinishCommandList()
void D3D9DeferredCommandProxy::FinishCommandList( RRenderCommandListPtr& rspCommandList )
{
    rspCommandList = m_spCommandList;
    if( !rspCommandList )
    {
        rspCommandList = new D3D9RenderCommandList;
        HELIUM_ASSERT( rspCommandList );
    }

    m_spCommandList.Release();
}
