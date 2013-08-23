#pragma once

#include "Graphics/Graphics.h"
#include "Engine/Resource.h"
#include "Rendering/RRenderResource.h"

namespace Helium
{
    class RTexture2d;

    HELIUM_DECLARE_RPTR( RTexture );

    class Texture;
    typedef Helium::StrongPtr< Texture > TexturePtr;
    typedef Helium::StrongPtr< const Texture > ConstTexturePtr;

    /// Base type for texture resources.
    class HELIUM_GRAPHICS_API Texture : public Resource
    {
        HELIUM_DECLARE_ASSET( Texture, Resource );
        static void PopulateMetaType( Reflect::MetaStruct& comp );

    public:
        struct ECompression : Reflect::Enum
        {
            /// Target texture compression schemes.  Note that compression schemes that yield lower memory usage may be used
            /// when appropriate if the visual result is not compromised (i.e. DXT1 may be used to compress a texture with
            /// COLOR_SMOOTH_ALPHA selected if the image has no alpha channel).
            enum Enum
            {
                /// No compression (32-bit RGBA).
                NONE,
                /// Color with optional premultiplied alpha masking (DXT1/DXT1a).
                COLOR,
                /// Color with sharp, low-precision alpha channel (DXT3).
                COLOR_SHARP_ALPHA,
                /// Color with smooth, compressed alpha channel (DXT5).
                COLOR_SMOOTH_ALPHA,
                /// Compressed normal map (swizzled DXT5 with special handling during mip level generation).
                NORMAL_MAP,
                /// Compressed normal map, higher compression (DXT1 with special handling during mip level generation).
                NORMAL_MAP_COMPACT,

                MAX,
            };

            HELIUM_DECLARE_ENUM( ECompression );

            static void PopulateMetaType( Helium::Reflect::MetaEnum& info )
            {
                info.AddElement( NONE,                  TXT( "NONE" ) );
                info.AddElement( COLOR,                 TXT( "COLOR" ) );
                info.AddElement( COLOR_SHARP_ALPHA,     TXT( "COLOR_SHARP_ALPHA" ) );
                info.AddElement( COLOR_SMOOTH_ALPHA,    TXT( "COLOR_SMOOTH_ALPHA" ) );
                info.AddElement( NORMAL_MAP,            TXT( "NORMAL_MAP" ) );
                info.AddElement( NORMAL_MAP_COMPACT,    TXT( "NORMAL_MAP_COMPACT" ) );
            }
        };

        /// @name Construction/Destruction
        //@{
        Texture();
        virtual ~Texture();
        //@}

        /// @name Asset Interface
        //@{
        virtual void PreDestroy();
        //@}

        /// @name Data Access
        //@{
        inline RTexture* GetRenderResource() const;
        virtual RTexture2d* GetRenderResource2d() const;

        inline ECompression GetCompression() const;
        inline bool GetSrgb() const;
        inline bool GetCreateMipmaps() const;
        inline bool GetIgnoreAlpha() const;
        //@}

        /// @name Resource Caching Support
        //@{
        virtual Name GetCacheName() const;
        //@}

        /// @name Static Utility Functions
        //@{
        inline static bool IsNormalMapCompression( ECompression compression );
        //@}

    protected:
        /// Texture render resource.
        RTexturePtr m_spTexture;
        /// Compression scheme.
        ECompression m_compression;
        /// True if the texture is in sRGB color space (ignored when using COMPRESSION_NORMAL_MAP as the compression
        /// scheme).
        bool m_bSrgb;
        /// True to generate mipmaps, false to only use a single mipmap.
        bool m_bCreateMipmaps;
        /// True to ignore any alpha channel data, false to keep it.
        bool m_bIgnoreAlpha;
    };
}

#include "Graphics/Texture.inl"
