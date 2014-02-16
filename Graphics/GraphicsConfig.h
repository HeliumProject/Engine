#pragma once

#include "Engine/Asset.h"
#include "Graphics/Graphics.h"

namespace Helium
{
    /// Graphics configuration data.
	class HELIUM_GRAPHICS_API GraphicsConfig : public Reflect::Object
    {
        HELIUM_DECLARE_CLASS( GraphicsConfig, Reflect::Object );

    public:
        /// Texture filtering mode.
        struct ETextureFilter : Reflect::Enum
        {
            enum Enum
            {
                BILINEAR,
                TRILINEAR,
                ANISOTROPIC,
            };

            HELIUM_DECLARE_ENUM( ETextureFilter );

            static void PopulateMetaType( Helium::Reflect::MetaEnum& info )
            {
                info.AddElement( BILINEAR,      TXT( "BILINEAR" ) );
                info.AddElement( TRILINEAR,     TXT( "TRILINEAR" ) );
                info.AddElement( ANISOTROPIC,   TXT( "ANISOTROPIC" ) );
            }
        };

        /// Shadow mode.
        struct EShadowMode : Reflect::Enum
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

            HELIUM_DECLARE_ENUM( EShadowMode );

            static void PopulateMetaType( Helium::Reflect::MetaEnum& info )
            {
                info.AddElement( NONE,          TXT( "NONE" ) );
                info.AddElement( SIMPLE,        TXT( "SIMPLE" ) );
                info.AddElement( PCF_DITHERED,  TXT( "PCF_DITHERED" ) );
            }
        };

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

        static void PopulateMetaType( Reflect::MetaStruct& comp );

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
