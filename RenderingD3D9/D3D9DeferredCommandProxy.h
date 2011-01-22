//----------------------------------------------------------------------------------------------------------------------
// D3D9DeferredCommandProxy.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_D3D9_D3D9_DEFERRED_COMMAND_PROXY_H
#define LUNAR_RENDERING_D3D9_D3D9_DEFERRED_COMMAND_PROXY_H

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RRenderCommandProxy.h"

namespace Lunar
{
    L_DECLARE_RPTR( D3D9RenderCommandList );

    /// Render command proxy for building command lists for deferred issuing of rendering commands to the GPU command
    /// buffer.
    class D3D9DeferredCommandProxy : public RRenderCommandProxy
    {
    public:
        /// @name Construction/Destruction
        //@{
        D3D9DeferredCommandProxy();
        //@}

        /// @name State Management
        //@{
        void SetRasterizerState( RRasterizerState* pState );
        void SetBlendState( RBlendState* pState );
        void SetDepthStencilState( RDepthStencilState* pState, uint8_t stencilReferenceValue );
        void SetSamplerStates( size_t startIndex, size_t samplerCount, RSamplerState* const* ppStates );
        //@}

        /// @name Render Target Management
        //@{
        void SetRenderSurfaces( RSurface* pRenderTargetSurface, RSurface* pDepthStencilSurface );
        void SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height );
        //@}

        /// @name Command Generation
        //@{
        void BeginScene();
        void EndScene();

        void Clear( uint32_t clearFlags, const Color& rColor, float32_t depth, uint8_t stencil );

        void SetIndexBuffer( RIndexBuffer* pBuffer );
        void SetVertexBuffers(
            size_t startIndex, size_t bufferCount, RVertexBuffer* const* ppBuffers, uint32_t* pStrides,
            uint32_t* pOffsets );
        void SetVertexInputLayout( RVertexInputLayout* pLayout );

        void SetVertexShader( RVertexShader* pShader );
        void SetPixelShader( RPixelShader* pShader );

        void SetVertexConstantBuffers(
            size_t startIndex, size_t bufferCount, RConstantBuffer* const* ppBuffers,
            const size_t* pLimitSizes = NULL );
        void SetPixelConstantBuffers(
            size_t startIndex, size_t bufferCount, RConstantBuffer* const* ppBuffers,
            const size_t* pLimitSizes = NULL );

        void SetTexture( size_t samplerIndex, RTexture* pTexture );

        void DrawIndexed(
            ERendererPrimitiveType primitiveType, uint32_t baseVertexIndex, uint32_t minIndex, uint32_t usedVertexCount,
            uint32_t startIndex, uint32_t primitiveCount );
        //@}

        /// @name Fence Commands
        //@{
        void SetFence( RFence* pFence );
        //@}

        /// @name Miscellaneous Resource Management
        //@{
        void UnbindResources();
        //@}

        /// @name Command List Support
        //@{
        void ExecuteCommandList( RRenderCommandList* pCommandList );

        void FinishCommandList( RRenderCommandListPtr& rspCommandList );
        //@}

    private:
        /// Command list.
        D3D9RenderCommandListPtr m_spCommandList;

        /// @name Construction/Destruction
        //@{
        ~D3D9DeferredCommandProxy();
        //@}
    };
}

#endif  // LUNAR_RENDERING_D3D9_D3D9_DEFERRED_COMMAND_PROXY_H
