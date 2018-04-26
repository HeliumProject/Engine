#pragma once

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RTexture2d.h"

namespace Helium
{
    /// Direct3D 9 2D texture implementation.
    class D3D9Texture2d : public RTexture2d
    {
    public:
        /// @name Construction/Destruction
        //@{
        D3D9Texture2d( IDirect3DTexture9* pD3DTexture, bool bSrgb );
        //@}

        /// @name Base Texture Information
        //@{
        uint32_t GetMipCount() const;
        //@}

        /// @name Data Access
        //@{
        virtual void* Map( uint32_t mipLevel, size_t& rPitch, ERendererBufferMapHint hint );
        virtual void Unmap( uint32_t mipLevel );
        bool CanMapWholeResource() const;

        uint32_t GetWidth( uint32_t mipLevel ) const;
        uint32_t GetHeight( uint32_t mipLevel ) const;
        ERendererPixelFormat GetPixelFormat() const;

        RSurface* GetSurface( uint32_t mipLevel );

        inline IDirect3DTexture9* GetD3DTexture() const;
        inline bool IsSrgb() const;
        //@}

    protected:
        /// Direct3D texture instance.
        IDirect3DTexture9* m_pTexture;
        /// True if texture color values are mapped in sRGB space, false if they are in linear space.
        bool m_bSrgb;

        /// @name Construction/Destruction
        //@{
        virtual ~D3D9Texture2d();
        //@}
    };
}

#include "RenderingD3D9/D3D9Texture2d.inl"
