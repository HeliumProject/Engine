//----------------------------------------------------------------------------------------------------------------------
// RRenderCommandProxy.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_RENDERING_R_RENDER_COMMAND_PROXY_H
#define HELIUM_RENDERING_R_RENDER_COMMAND_PROXY_H

#include "Rendering/RRenderResource.h"

#include "Rendering/Color.h"
#include "Rendering/RendererTypes.h"

namespace Helium
{
    class RRasterizerState;
    class RBlendState;
    class RDepthStencilState;

    class RSurface;
    class RIndexBuffer;
    class RVertexInputLayout;

    class RVertexShader;
    class RPixelShader;

    class RTexture;

    class RFence;

    L_DECLARE_RPTR( RSamplerState );
    L_DECLARE_RPTR( RVertexBuffer );
    L_DECLARE_RPTR( RConstantBuffer );

    L_DECLARE_RPTR( RRenderCommandList );

    /// Proxy for issuing renderer commands.
    class HELIUM_RENDERING_API RRenderCommandProxy : public RRenderResource
    {
    public:
        /// @name State Management
        //@{
        virtual void SetRasterizerState( RRasterizerState* pState ) = 0;
        virtual void SetBlendState( RBlendState* pState ) = 0;
        virtual void SetDepthStencilState( RDepthStencilState* pState, uint8_t stencilReferenceValue ) = 0;
        virtual void SetSamplerStates( size_t startIndex, size_t samplerCount, RSamplerState* const* ppStates ) = 0;
        inline void SetSamplerStates( size_t startIndex, size_t samplerCount, RSamplerStatePtr const* pspStates );
        //@}

        /// @name Render Target Management
        //@{
        virtual void SetRenderSurfaces( RSurface* pRenderTargetSurface, RSurface* pDepthStencilSurface ) = 0;
        virtual void SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) = 0;
        //@}

        /// @name Command Generation
        //@{
        virtual void BeginScene() = 0;
        virtual void EndScene() = 0;

        virtual void Clear(
            uint32_t clearFlags, const Color& rColor = Color( 0 ), float32_t depth = 1.0f, uint8_t stencil = 0 ) = 0;

        virtual void SetIndexBuffer( RIndexBuffer* pBuffer ) = 0;
        virtual void SetVertexBuffers(
            size_t startIndex, size_t bufferCount, RVertexBuffer* const* ppBuffers, uint32_t* pStrides,
            uint32_t* pOffsets ) = 0;
        inline void SetVertexBuffers(
            size_t startIndex, size_t bufferCount, RVertexBufferPtr const* pspBuffers, uint32_t* pStrides,
            uint32_t* pOffsets );
        virtual void SetVertexInputLayout( RVertexInputLayout* pLayout ) = 0;

        virtual void SetVertexShader( RVertexShader* pShader ) = 0;
        virtual void SetPixelShader( RPixelShader* pShader ) = 0;

        virtual void SetVertexConstantBuffers(
            size_t startIndex, size_t bufferCount, RConstantBuffer* const* ppBuffers,
            const size_t* pLimitSizes = NULL ) = 0;
        inline void SetVertexConstantBuffers(
            size_t startIndex, size_t bufferCount, RConstantBufferPtr const* pspBuffers,
            const size_t* pLimitSizes = NULL );
        virtual void SetPixelConstantBuffers(
            size_t startIndex, size_t bufferCount, RConstantBuffer* const* ppBuffers,
            const size_t* pLimitSizes = NULL ) = 0;
        inline void SetPixelConstantBuffers(
            size_t startIndex, size_t bufferCount, RConstantBufferPtr const* pspBuffers,
            const size_t* pLimitSizes = NULL );

        virtual void SetTexture( size_t samplerIndex, RTexture* pTexture ) = 0;

        virtual void DrawIndexed(
            ERendererPrimitiveType primitiveType, uint32_t baseVertexIndex, uint32_t minIndex, uint32_t usedVertexCount,
            uint32_t startIndex, uint32_t primitiveCount ) = 0;
        virtual void DrawUnindexed(
            ERendererPrimitiveType primitiveType, uint32_t baseVertexIndex, uint32_t primitiveCount ) = 0;
        //@}

        /// @name Fence Commands
        //@{
        virtual void SetFence( RFence* pFence ) = 0;
        //@}

        /// @name Miscellaneous Resource Management
        //@{
        virtual void UnbindResources() = 0;
        //@}

        /// @name Command List Support
        //@{
        virtual void ExecuteCommandList( RRenderCommandList* pCommandList ) = 0;

        virtual void FinishCommandList( RRenderCommandListPtr& rspCommandList ) = 0;
        //@}

    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RRenderCommandProxy() = 0;
        //@}
    };
}

#include "Rendering/RRenderCommandProxy.inl"

#endif  // HELIUM_RENDERING_R_RENDER_COMMAND_PROXY_H
