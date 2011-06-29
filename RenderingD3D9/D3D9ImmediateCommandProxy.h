//----------------------------------------------------------------------------------------------------------------------
// D3D9ImmediateCommandProxy.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_RENDERING_D3D9_D3D9_IMMEDIATE_COMMAND_PROXY_H
#define HELIUM_RENDERING_D3D9_D3D9_IMMEDIATE_COMMAND_PROXY_H

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RRenderCommandProxy.h"

namespace Helium
{
    L_DECLARE_RPTR( RTexture );

    L_DECLARE_RPTR( D3D9RasterizerState );
    L_DECLARE_RPTR( D3D9BlendState );
    L_DECLARE_RPTR( D3D9DepthStencilState );
    L_DECLARE_RPTR( D3D9SamplerState );

    L_DECLARE_RPTR( D3D9ConstantBuffer );

    /// Render command proxy for immediate issuing of rendering commands to the GPU command buffer.
    class D3D9ImmediateCommandProxy : public RRenderCommandProxy
    {
    public:
        /// Maximum number of sampler stages.
        static const size_t SAMPLER_STAGE_COUNT = 16;

        /// Maximum number of vertex stream sources.
        static const size_t STREAM_SOURCE_COUNT = 16;

        /// Maximum number of constant buffers for a given shader type (vertex or pixel).
        static const size_t CONSTANT_BUFFER_SLOT_COUNT = 14;

        /// Maximum number of vertex shader constant registers that can be addressed by the engine (minimum supported by
        /// shader model 3).
        static const size_t VERTEX_SHADER_CONSTANT_REGISTER_COUNT = 256;
        /// Maximum number of pixel shader constant registers that can be addressed by the engine (minimum supported by
        /// shader model 3).
        static const size_t PIXEL_SHADER_CONSTANT_REGISTER_COUNT = 224;

        /// @name Construction/Destruction
        //@{
        D3D9ImmediateCommandProxy( IDirect3DDevice9* pD3DDevice );
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
        void DrawUnindexed( ERendererPrimitiveType primitiveType, uint32_t baseVertexIndex, uint32_t primitiveCount );
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
        /// Shader constant management.
        template< typename Pusher, size_t RegisterCount >
        class ConstantManager
        {
        public:
            /// @name Construction/Destruction
            //@{
            ConstantManager();
            ~ConstantManager();
            //@}

            /// @name Constant Buffer Access
            //@{
            void SetBuffer( size_t index, D3D9ConstantBuffer* pBuffer, size_t limitSize );
            D3D9ConstantBuffer* GetBuffer( size_t index ) const;
            //@}

            /// @name Constant Register Updating
            //@{
            void Push( IDirect3DDevice9* pDevice );
            //@}

        private:
            /// Active constant buffers.
            D3D9ConstantBufferPtr m_buffers[ CONSTANT_BUFFER_SLOT_COUNT ];
            /// Most recent constant buffer tags.
            uint32_t m_bufferTags[ CONSTANT_BUFFER_SLOT_COUNT ];
            /// Flags specifying which constant registers are dirty.
            uint32_t m_dirtyRegisters[ ( RegisterCount + sizeof( uint32_t ) * 8 - 1 ) / ( sizeof( uint32_t ) * 8 ) ];
            /// Constant buffer update range limits.
            uint16_t m_bufferLimitSizes[ CONSTANT_BUFFER_SLOT_COUNT ];
            /// Constant value pusher.
            Pusher m_pusher;
        };

        /// Vertex shader constant pusher.
        class VertexShaderConstantPusher
        {
        public:
            /// @name Overloaded Operators
            //@{
            void operator()( IDirect3DDevice9* pDevice, UINT startIndex, const float32_t* pData, UINT count );
            //@}
        };

        /// Pixel shader constant pusher.
        class PixelShaderConstantPusher
        {
        public:
            /// @name Overloaded Operators
            //@{
            void operator()( IDirect3DDevice9* pDevice, UINT startIndex, const float32_t* pData, UINT count );
            //@}
        };

        /// Vertex shader constant manager.
        typedef ConstantManager< VertexShaderConstantPusher, VERTEX_SHADER_CONSTANT_REGISTER_COUNT >
            VertexShaderConstantManager;
        /// Pixel shader constant manager.
        typedef ConstantManager< PixelShaderConstantPusher, PIXEL_SHADER_CONSTANT_REGISTER_COUNT >
            PixelShaderConstantManager;

        /// Direct3D 9 device instance.
        IDirect3DDevice9* m_pDevice;

        /// Currently bound rasterizer state.
        D3D9RasterizerStatePtr m_spRasterizerState;
        /// Currently bound blend state.
        D3D9BlendStatePtr m_spBlendState;
        /// Currently bound depth-stencil state.
        D3D9DepthStencilStatePtr m_spDepthStencilState;
        /// Currently bound sampler states.
        D3D9SamplerStatePtr m_samplerStates[ SAMPLER_STAGE_COUNT ];

        /// Vertex shader constant manager.
        VertexShaderConstantManager m_vertexConstantManager;
        /// Pixel shader constant manager.
        PixelShaderConstantManager m_pixelConstantManager;

        /// Bound textures.
        RTexturePtr m_textures[ SAMPLER_STAGE_COUNT ];
        /// Bit flags specifying which bound textures are in sRGB space.
        uint32_t m_srgbTextureFlags;

        /// @name Construction/Destruction
        //@{
        ~D3D9ImmediateCommandProxy();
        //@}
    };
}

#endif  // HELIUM_RENDERING_D3D9_D3D9_IMMEDIATE_COMMAND_PROXY_H
