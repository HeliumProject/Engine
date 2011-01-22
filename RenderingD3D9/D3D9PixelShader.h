//----------------------------------------------------------------------------------------------------------------------
// D3D9PixelShader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_D3D9_D3D9_PIXEL_SHADER_H
#define LUNAR_RENDERING_D3D9_D3D9_PIXEL_SHADER_H

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RPixelShader.h"

namespace Lunar
{
    /// Direct3D 9 pixel shader implementation.
    class D3D9PixelShader : public RPixelShader
    {
    public:
        /// @name Construction/Destruction
        //@{
        D3D9PixelShader( void* pShaderData, bool bStaging );
        //@}

        /// @name Loading
        //@{
        void* Lock();
        bool Unlock();
        //@}

        /// @name Data Access
        //@{
        inline IDirect3DPixelShader9* GetD3DShader() const;
        //@}

    private:
        /// Direct3D pixel shader instance, or a memory buffer allocated as a staging area if not yet loaded.
        void* m_pShaderData;
        /// True if the shader data pointer is pointing to a staging buffer for loading, false if it is pointing to a
        /// Direct3D vertex shader instance.
        bool m_bStaging;

        /// @name Construction/Destruction
        //@{
        ~D3D9PixelShader();
        //@}
    };
}

#include "RenderingD3D9/D3D9PixelShader.inl"

#endif  // LUNAR_RENDERING_D3D9_D3D9_PIXEL_SHADER_H
