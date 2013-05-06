//----------------------------------------------------------------------------------------------------------------------
// GraphicsConfig.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_GRAPHICS_GRAPHICS_CONFIG_H
#define HELIUM_GRAPHICS_GRAPHICS_CONFIG_H

#include "Graphics/Graphics.h"

namespace Helium
{
    /// Graphics configuration data.
    class HELIUM_GRAPHICS_API GraphicsConfig : public Asset
    {
        HELIUM_DECLARE_ASSET( GraphicsConfig, Asset );

    public:
        /// Texture filtering mode.
        struct ETextureFilter : Reflect::EnumerationBase
        {
            enum Enum
            {
                BILINEAR,
                TRILINEAR,
                ANISOTROPIC,
            };

            REFLECT_DECLARE_ENUMERATION( ETextureFilter );

            static void EnumerateEnum( Helium::Reflect::Enumeration& info )
            {
                info.AddElement( BILINEAR,      TXT( "BILINEAR" ) );
                info.AddElement( TRILINEAR,     TXT( "TRILINEAR" ) );
                info.AddElement( ANISOTROPIC,   TXT( "ANISOTROPIC" ) );
            }
        };

        /// Shadow mode.
        struct EShadowMode : Reflect::EnumerationBase
        {
        public:
            enum Enum
            {
                INVALID = -1,
                NONE,
                SIMPLE,
                PCF_DITHERED,
                MAX,
            };

            REFLECT_DECLARE_ENUMERATION( EShadowMode );

            static void EnumerateEnum( Helium::Reflect::Enumeration& info )
            {
                info.AddElement( NONE,          TXT( "NONE" ) );
                info.AddElement( SIMPLE,        TXT( "SIMPLE" ) );
                info.AddElement( PCF_DITHERED,  TXT( "PCF_DITHERED" ) );
            }
        };

        /*
        /// Texture filtering mode.
        HELIUM_ENUM( ETextureFilter, TEXTURE_FILTER, ( BILINEAR ) ( TRILINEAR ) ( ANISOTROPIC ) );
        /// Shadow mode.
        HELIUM_ENUM( EShadowMode, SHADOW_MODE, ( NONE ) ( SIMPLE ) ( PCF_DITHERED ) );
        */

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

        static void PopulateStructure( Reflect::Structure& comp );
        /// @name Serialization
        //@{
        //virtual void Serialize( Serializer& s );
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

    public:
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

#endif  // HELIUM_GRAPHICS_GRAPHICS_CONFIG_H
