#pragma once

#include "Rendering/RTexture.h"

#include "Rendering/RendererTypes.h"

namespace Helium
{
    class RSurface;

    /// Low-level 2D texture interface.
    class HELIUM_RENDERING_API RTexture2d : public RTexture
    {
    public:
        /// 2D texture initialization data for a single mip level.  This is used when initializing a texture upon
        /// creation using Renderer::CreateTexture2d().
        struct CreateData
        {
            /// Pointer to the texture mip level data.
            const void* pData;
            /// Number of bytes per row of data.  For uncompressed texture formats, this is the number of bytes per row
            /// of pixels.  For block-compressed texture formats (i.e. DXT formats), this is the number of bytes per row
            /// of blocks.
            size_t pitch;
        };

        /// @name Type Information
        //@{
        EType GetType() const;
        //@}

        /// @name Data Access
        //@{
        virtual void* Map(
            uint32_t mipLevel, size_t& rPitch, ERendererBufferMapHint hint = RENDERER_BUFFER_MAP_HINT_NONE ) = 0;
        virtual void Unmap( uint32_t mipLevel ) = 0;
        virtual bool CanMapWholeResource() const = 0;

        virtual uint32_t GetWidth( uint32_t mipLevel = 0 ) const = 0;
        virtual uint32_t GetHeight( uint32_t mipLevel = 0 ) const = 0;

        virtual ERendererPixelFormat GetPixelFormat() const = 0;

        virtual RSurface* GetSurface( uint32_t mipLevel ) = 0;
        //@}

    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RTexture2d() = 0;
        //@}
    };
}
