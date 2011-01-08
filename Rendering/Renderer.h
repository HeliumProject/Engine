//----------------------------------------------------------------------------------------------------------------------
// Renderer.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_RENDERER_H
#define LUNAR_RENDERING_RENDERER_H

#include "Rendering/RRasterizerState.h"
#include "Rendering/RBlendState.h"
#include "Rendering/RDepthStencilState.h"
#include "Rendering/RSamplerState.h"
#include "Rendering/RTexture2d.h"
#include "Rendering/RVertexDescription.h"

namespace Lunar
{
    class RRenderCommandProxy;
    class RRenderContext;

    class RSurface;

    class RVertexShader;
    class RPixelShader;

    class RVertexBuffer;
    class RIndexBuffer;
    class RConstantBuffer;

    class RVertexInputLayout;

    class RFence;

    /// Main renderer base class.
    class LUNAR_RENDERING_API Renderer : NonCopyable
    {
    public:
        /// Rendering context initialization parameters.
        struct LUNAR_RENDERING_API ContextInitParameters
        {
            /// Platform-specific handle for the window to associate with the context.
            void* pWindow;

            /// Display buffer width, in pixels.
            uint32_t displayWidth;
            /// Display buffer height, in pixels.
            uint32_t displayHeight;

            /// Multisampling sample count (0 or 1 to disable).
            uint32_t multisampleCount;

            /// True to render fullscreen.
            bool bFullscreen;
            /// True to enable vsync.
            bool bVsync;

            /// @name Construction/Destruction
            //@{
            inline ContextInitParameters();
            //@}
        };

        /// @name Initialization
        //@{
        virtual bool Initialize() = 0;
        virtual void Shutdown() = 0;
        //@}

        /// @name Feature Support
        //@{
        inline uint32_t GetFeatureFlags() const;
        inline bool SupportsAllFeatures( uint32_t featureFlags ) const;
        inline bool SupportsAnyFeature( uint32_t featureFlags ) const;
        //@}

        /// @name Display Initialization
        //@{
        virtual bool CreateMainContext( const ContextInitParameters& rInitParameters ) = 0;
        virtual RRenderContext* GetMainContext() = 0;

        inline uint32_t GetMainContextWidth() const;
        inline uint32_t GetMainContextHeight() const;

        virtual RRenderContext* CreateSubContext( const ContextInitParameters& rInitParameters ) = 0;
        //@}

        /// @name State Object Creation
        //@{
        virtual RRasterizerState* CreateRasterizerState( const RRasterizerState::Description& rDescription ) = 0;
        virtual RBlendState* CreateBlendState( const RBlendState::Description& rDescription ) = 0;
        virtual RDepthStencilState* CreateDepthStencilState( const RDepthStencilState::Description& rDescription ) = 0;
        virtual RSamplerState* CreateSamplerState( const RSamplerState::Description& rDescription ) = 0;
        //@}

        /// @name Resource Allocation
        //@{
        virtual RSurface* CreateDepthStencilSurface(
            uint32_t width, uint32_t height, ERendererSurfaceFormat format, uint32_t multisampleCount ) = 0;

        virtual RVertexShader* CreateVertexShader( size_t size, const void* pData = NULL ) = 0;
        virtual RPixelShader* CreatePixelShader( size_t size, const void* pData = NULL ) = 0;

        virtual RVertexBuffer* CreateVertexBuffer(
            size_t size, ERendererBufferUsage usage, const void* pData = NULL ) = 0;
        virtual RIndexBuffer* CreateIndexBuffer(
            size_t size, ERendererBufferUsage usage, ERendererIndexFormat format, const void* pData = NULL ) = 0;
        virtual RConstantBuffer* CreateConstantBuffer(
            size_t size, ERendererBufferUsage usage, const void* pData = NULL ) = 0;

        virtual RVertexDescription* CreateVertexDescription(
            const RVertexDescription::Element* pElements, size_t elementCount ) = 0;
        virtual RVertexInputLayout* CreateVertexInputLayout(
            RVertexDescription* pDescription, RVertexShader* pShader ) = 0;

        virtual RTexture2d* CreateTexture2d(
            uint32_t width, uint32_t height, uint32_t mipCount, ERendererPixelFormat format, ERendererBufferUsage usage,
            const RTexture2d::CreateData* pData = NULL ) = 0;
        //@}

        /// @name Deferred Query Allocation
        //@{
        virtual RFence* CreateFence() = 0;
        virtual void SyncFence( RFence* pFence ) = 0;
        virtual bool TrySyncFence( RFence* pFence ) = 0;
        //@}

        /// @name Command Interfaces
        //@{
        virtual RRenderCommandProxy* GetImmediateCommandProxy() = 0;
        virtual RRenderCommandProxy* CreateDeferredCommandProxy() = 0;

        virtual void Flush() = 0;
        //@}

        /// @name Static Access
        //@{
        static Renderer* GetStaticInstance();
        static void DestroyStaticInstance();
        //@}

    protected:
        /// Cached display width of the main context, in pixels.
        uint32_t m_mainContextWidth;
        /// Cached display height of the main context, in pixels.
        uint32_t m_mainContextHeight;

        /// Renderer feature flags.
        uint32_t m_featureFlags;

        /// Singleton instance.
        static Renderer* sm_pInstance;

        /// @name Construction/Destruction
        //@{
        Renderer();
        virtual ~Renderer() = 0;
        //@}
    };
}

#include "Rendering/Renderer.inl"

#endif  // LUNAR_RENDERING_RENDERER_H
