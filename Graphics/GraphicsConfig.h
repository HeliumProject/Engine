//----------------------------------------------------------------------------------------------------------------------
// GraphicsConfig.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_GRAPHICS_GRAPHICS_CONFIG_H
#define LUNAR_GRAPHICS_GRAPHICS_CONFIG_H

#include "Graphics/Graphics.h"

namespace Lunar
{
    /// Graphics configuration data.
    class LUNAR_GRAPHICS_API GraphicsConfig : public Object
    {
        L_DECLARE_OBJECT( GraphicsConfig, Object );

    public:
        /// Texture filtering mode.
        L_ENUM( ETextureFilter, TEXTURE_FILTER, ( BILINEAR ) ( TRILINEAR ) ( ANISOTROPIC ) );
        /// Shadow mode.
        L_ENUM( EShadowMode, SHADOW_MODE, ( NONE ) ( SIMPLE ) ( PCF_DITHERED ) );

        /// Default display width.
        static const uint32_t DEFAULT_WIDTH = 640;
        /// Default display height.
        static const uint32_t DEFAULT_HEIGHT = 480;

        /// Default shadow buffer size.
        static const uint32_t DEFAULT_SHADOW_BUFFER_SIZE = 1024;

        /// @name Construction/Destruction
        //@{
        GraphicsConfig();
        virtual ~GraphicsConfig();
        //@}

        /// @name Serialization
        //@{
        virtual void Serialize( Serializer& s );
        //@}

        /// @name Data Access
        //@{
        inline uint32_t GetWidth() const;
        inline uint32_t GetHeight() const;

        inline ETextureFilter GetTextureFiltering() const;
        inline uint32_t GetMaxAnisotropy() const;

        inline EShadowMode GetShadowMode() const;
        inline uint32_t GetShadowBufferSize() const;

        inline bool GetFullscreen() const;
        inline bool GetVsync() const;
        //@}

    private:
        /// Display width.
        uint32_t m_width;
        /// Display height.
        uint32_t m_height;

        /// Texture filtering mode.
        ETextureFilter m_textureFiltering;
        /// Maximum anisotropy value for anisotropic texture filtering.
        uint32_t m_maxAnisotropy;

        /// Shadowing mode.
        EShadowMode m_shadowMode;
        /// Shadow buffer size (width/height, in texels).
        uint32_t m_shadowBufferSize;

        /// True to run in fullscreen mode, false to run in windowed mode.
        bool m_bFullscreen;
        /// True to enable vsync.
        bool m_bVsync;
    };
}

#include "Graphics/GraphicsConfig.inl"

#endif  // LUNAR_GRAPHICS_GRAPHICS_CONFIG_H
