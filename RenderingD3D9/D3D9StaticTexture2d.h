//----------------------------------------------------------------------------------------------------------------------
// D3D9StaticTexture2d.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_RENDERING_D3D9_D3D9_STATIC_TEXTURE_2D_H
#define HELIUM_RENDERING_D3D9_D3D9_STATIC_TEXTURE_2D_H

#include "RenderingD3D9/D3D9Texture2d.h"

namespace Helium
{
    /// Static Direct3D 9 2D texture.
    ///
    /// Direct3D 9 Ex (on Windows Vista and later) only allows static textures to be allocated in the default pool, in
    /// which textures cannot be locked (unless they are dynamic), instead of the managed pool.  As a result, we need to
    /// use a temporary dynamic texture as a staging area for mapping the texture for updating.
    ///
    /// Note that we only ever copy from the staging area to the texture itself, so a given texture mip level must
    /// always be fully updated when locked.
    class D3D9StaticTexture2d : public D3D9Texture2d
    {
    public:
        /// @name Construction/Destruction
        //@{
        D3D9StaticTexture2d( IDirect3DTexture9* pD3DTexture, bool bSrgb );
        //@}

        /// @name Data Access
        //@{
        void* Map( uint32_t mipLevel, size_t& rPitch, ERendererBufferMapHint hint );
        void Unmap( uint32_t mipLevel );
        //@}

    protected:
        /// Mapped texture staging area.
        IDirect3DTexture9* m_pMappedTexture;
        /// Base mip level in the mapped texture.
        uint8_t m_baseMipLevel;
        /// Current number of active Map() calls than have not yet been unmapped.
        uint8_t m_mapCount;
        /// True if the mapped texture is a pooled mapped texture.
        bool m_bIsMappedTexturePooled;

        /// @name Construction/Destruction
        //@{
        ~D3D9StaticTexture2d();
        //@}

        /// @name Map Count Decrementing
        //@{
        void DecrementMapCount();
        //@}
    };
}

#endif  // HELIUM_RENDERING_D3D9_D3D9_STATIC_TEXTURE_2D_H
