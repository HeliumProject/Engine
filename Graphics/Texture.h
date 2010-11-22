//----------------------------------------------------------------------------------------------------------------------
// Texture.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_GRAPHICS_TEXTURE_H
#define LUNAR_GRAPHICS_TEXTURE_H

#include "Graphics/Graphics.h"
#include "Engine/Resource.h"

#include "Engine/Serializer.h"
#include "Rendering/RRenderResource.h"

namespace Lunar
{
    class RTexture2d;

    L_DECLARE_RPTR( RTexture );

    /// Base type for texture resources.
    class LUNAR_GRAPHICS_API Texture : public Resource
    {
        L_DECLARE_OBJECT( Texture, Resource );

    public:
        /// Target texture compression schemes.  Note that compression schemes that yield lower memory usage may be used
        /// when appropriate if the visual result is not compromised (i.e. DXT1 may be used to compress a texture with
        /// COLOR_SMOOTH_ALPHA selected if the image has no alpha channel).
        L_ENUM(
            ECompression,
            COMPRESSION,
            /// No compression (32-bit RGBA).
            ( NONE )
            /// Color with optional premultiplied alpha masking (DXT1/DXT1a).
            ( COLOR )
            /// Color with sharp, low-precision alpha channel (DXT3).
            ( COLOR_SHARP_ALPHA )
            /// Color with smooth, compressed alpha channel (DXT5).
            ( COLOR_SMOOTH_ALPHA )
            /// Compressed normal map (swizzled DXT5 with special handling during mip level generation).
            ( NORMAL_MAP )
            /// Compressed normal map, higher compression (DXT1 with special handling during mip level generation).
            ( NORMAL_MAP_COMPACT ) );

        /// @name Construction/Destruction
        //@{
        Texture();
        virtual ~Texture();
        //@}

        /// @name GameObject Interface
        //@{
        virtual void PreDestroy();
        //@}

        /// @name Serialization
        //@{
        virtual void Serialize( Serializer& s );
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

#endif  // LUNAR_GRAPHICS_TEXTURE_H
