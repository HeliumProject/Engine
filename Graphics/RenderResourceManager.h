//----------------------------------------------------------------------------------------------------------------------
// RenderResourceManager.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_GRAPHICS_RENDER_RESOURCE_MANAGER_H
#define LUNAR_GRAPHICS_RENDER_RESOURCE_MANAGER_H

#include "Graphics/Graphics.h"

#include "Rendering/RendererTypes.h"
#include "Rendering/RRenderResource.h"
#include "Graphics/GraphicsConfig.h"

namespace Lunar
{
    HELIUM_DECLARE_PTR( Font );
    HELIUM_DECLARE_PTR( ShaderVariant );

    L_DECLARE_RPTR( RRasterizerState );
    L_DECLARE_RPTR( RBlendState );
    L_DECLARE_RPTR( RDepthStencilState );
    L_DECLARE_RPTR( RSamplerState );
    L_DECLARE_RPTR( RTexture2d );
    L_DECLARE_RPTR( RVertexDescription );

    /// Manager for common render resources used by the graphics system.
    class LUNAR_GRAPHICS_API RenderResourceManager : NonCopyable
    {
    public:
        /// Maximum number of texture coordinate sets allowed for meshes.
        static const size_t MESH_TEXTURE_COORDINATE_SET_COUNT_MAX = 2;

        /// Standard rasterizer states.
        enum ERasterizerState
        {
            RASTERIZER_STATE_FIRST   =  0,
            RASTERIZER_STATE_INVALID = -1,

            /// Default rasterizer state (fill triangles, CW front faces, cull back faces, no depth biasing).
            RASTERIZER_STATE_DEFAULT,
            /// Double-sided rendering rasterizer state.
            RASTERIZER_STATE_DOUBLE_SIDED,
            /// Wireframe rasterizer state (with culling).
            RASTERIZER_STATE_WIREFRAME,
            /// Double-sided wireframe rasterizer state.
            RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED,
            /// Shadow depth rasterizer state (double-sided, depth biasing applied to resolve z-fighting, etc.).
            RASTERIZER_STATE_SHADOW_DEPTH,

            RASTERIZER_STATE_MAX,
            RASTERIZER_STATE_LAST = RASTERIZER_STATE_MAX - 1
        };

        /// Standard blend states.
        enum EBlendState
        {
            BLEND_STATE_FIRST   =  0,
            BLEND_STATE_INVALID = -1,

            /// Opaque/masked rendering blend state.
            BLEND_STATE_OPAQUE,
            /// Transparent rendering blend state.
            BLEND_STATE_TRANSPARENT,
            /// Additive rendering blend state.
            BLEND_STATE_ADDITIVE,
            /// Subtractive rendering blend state.
            BLEND_STATE_SUBTRACTIVE,
            /// Modulate rendering blend state.
            BLEND_STATE_MODULATE,
            /// No-color rendering blend state (used for depth-only rendering).
            BLEND_STATE_NO_COLOR,

            BLEND_STATE_MAX,
            BLEND_STATE_LAST = BLEND_STATE_MAX - 1
        };

        /// Standard depth/stencil states.
        enum EDepthStencilState
        {
            DEPTH_STENCIL_STATE_FIRST   =  0,
            DEPTH_STENCIL_STATE_INVALID = -1,

            /// Default depth/stencil state (testing and writing enabled, less-than comparison function, no stencil).
            DEPTH_STENCIL_STATE_DEFAULT,
            /// Depth test only (no depth write, less-than comparison function, no stencil).
            DEPTH_STENCIL_STATE_TEST_ONLY,
            /// No writing or testing of the depth or stencil buffers.
            DEPTH_STENCIL_STATE_NONE,

            DEPTH_STENCIL_STATE_MAX,
            DEPTH_STENCIL_STATE_LAST = DEPTH_STENCIL_STATE_MAX - 1
        };

        /// Texture filtering modes.
        enum ETextureFilter
        {
            TEXTURE_FILTER_FIRST   =  0,
            TEXTURE_FILTER_INVALID = -1,

            /// Standard linear filtering (bilinear/trilinear and anisotropic filtering determined by graphics config).
            TEXTURE_FILTER_LINEAR,
            /// Point filtering.
            TEXTURE_FILTER_POINT,

            TEXTURE_FILTER_MAX,
            TEXTURE_FILTER_LAST = TEXTURE_FILTER_MAX - 1
        };

        /// Debug font sizes.
        enum EDebugFontSize
        {
            DEBUG_FONT_SIZE_FIRST   =  0,
            DEBUG_FONT_SIZE_INVALID = -1,

            /// Small (10 pt) debug font size.
            DEBUG_FONT_SIZE_SMALL,
            /// Medium (12 pt) debug font size.
            DEBUG_FONT_SIZE_MEDIUM,
            /// Large (14 pt) debug font size.
            DEBUG_FONT_SIZE_LARGE,

            DEBUG_FONT_SIZE_MAX,
            DEBUG_FONT_SIZE_LAST = DEBUG_FONT_SIZE_MAX - 1
        };

        /// @name State Initialization
        //@{
        void Initialize();
        void Shutdown();

        void PostConfigUpdate();
        //@}

        /// @name State Access
        //@{
        RRasterizerState* GetRasterizerState( ERasterizerState type ) const;
        RBlendState* GetBlendState( EBlendState type ) const;
        RDepthStencilState* GetDepthStencilState( EDepthStencilState type ) const;
        RSamplerState* GetSamplerState( ETextureFilter filterType, ERendererTextureAddressMode addressMode ) const;
        //@}

        /// @name Vertex Description Access
        //@{
        RVertexDescription* GetSimpleVertexDescription() const;
        RVertexDescription* GetSimpleTexturedVertexDescription() const;
        RVertexDescription* GetStaticMeshVertexDescription( size_t textureCoordinateSetCount ) const;
        RVertexDescription* GetSkinnedMeshVertexDescription() const;
        //@}

        /// @name Resource Access
        //@{
        RTexture2d* GetSceneTexture() const;
        RTexture2d* GetShadowDepthTexture() const;

        ShaderVariant* GetPrePassVertexShader() const;
        ShaderVariant* GetSimpleWorldSpaceVertexShader() const;
        ShaderVariant* GetSimpleWorldSpacePixelShader() const;
        ShaderVariant* GetSimpleScreenSpaceVertexShader() const;
        ShaderVariant* GetSimpleScreenSpacePixelShader() const;

        Font* GetDebugFont( EDebugFontSize size ) const;

        inline GraphicsConfig::EShadowMode GetShadowMode() const;
        inline uint32_t GetShadowDepthTextureUsableSize() const;
        //@}

        /// @name Static Access
        //@{
        static RenderResourceManager& GetStaticInstance();
        static void DestroyStaticInstance();
        //@}

    private:
        /// Standard rasterizer states.
        RRasterizerStatePtr m_rasterizerStates[ RASTERIZER_STATE_MAX ];
        /// Standard blend states.
        RBlendStatePtr m_blendStates[ BLEND_STATE_MAX ];
        /// Standard depth/stencil states.
        RDepthStencilStatePtr m_depthStencilStates[ DEPTH_STENCIL_STATE_MAX ];
        /// Standard sampler states.
        RSamplerStatePtr m_samplerStates[ TEXTURE_FILTER_MAX ][ RENDERER_TEXTURE_ADDRESS_MODE_MAX ];

        /// Simple vertex description.
        RVertexDescriptionPtr m_spSimpleVertexDescription;
        /// Simple textured vertex description.
        RVertexDescriptionPtr m_spSimpleTexturedVertexDescription;
        /// Static mesh vertex descriptions.
        RVertexDescriptionPtr m_staticMeshVertexDescriptions[ MESH_TEXTURE_COORDINATE_SET_COUNT_MAX ];
        /// Skinned mesh vertex description.
        RVertexDescriptionPtr m_spSkinnedMeshVertexDescription;

        /// Scene render texture.
        RTexture2dPtr m_spSceneTexture;
        /// Shadow depth texture.
        RTexture2dPtr m_spShadowDepthTexture;

        /// Depth-only pre-pass vertex shader.
        ShaderVariantPtr m_spPrePassVertexShader;
        /// Simple world-space primitive vertex shader.
        ShaderVariantPtr m_spSimpleWorldSpaceVertexShader;
        /// Simple world-space primitive pixel shader.
        ShaderVariantPtr m_spSimpleWorldSpacePixelShader;
        /// Simple screen-space primitive vertex shader.
        ShaderVariantPtr m_spSimpleScreenSpaceVertexShader;
        /// Simple screen-space primitive pixel shader.
        ShaderVariantPtr m_spSimpleScreenSpacePixelShader;

        /// Debug fonts.
        FontPtr m_debugFonts[ DEBUG_FONT_SIZE_MAX ];

        /// Effective shadow mode in use (accounting for whether a shadow buffer was allocated, etc.).
        GraphicsConfig::EShadowMode m_shadowMode;
        /// Shadow depth texture usable size (cached from graphics config object value).
        uint32_t m_shadowDepthTextureUsableSize;

        /// Singleton instance.
        static RenderResourceManager* sm_pInstance;

        /// @name Construction/Destruction
        //@{
        RenderResourceManager();
        ~RenderResourceManager();
        //@}
    };
}

#include "Graphics/RenderResourceManager.inl"

#endif  // LUNAR_GRAPHICS_RENDER_RESOURCE_MANAGER_H
