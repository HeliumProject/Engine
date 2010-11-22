//----------------------------------------------------------------------------------------------------------------------
// D3D9VertexShader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_D3D9_RENDERING_D3D9_VERTEX_SHADER_H
#define LUNAR_D3D9_RENDERING_D3D9_VERTEX_SHADER_H

#include "D3D9Rendering/D3D9Rendering.h"
#include "Rendering/RVertexShader.h"

namespace Lunar
{
    /// Direct3D 9 vertex shader implementation.
    class D3D9VertexShader : public RVertexShader
    {
    public:
        /// @name Construction/Destruction
        //@{
        D3D9VertexShader( void* pShaderData, bool bStaging );
        //@}

        /// @name Loading
        //@{
        void* Lock();
        bool Unlock();
        //@}

        /// @name Data Access
        //@{
        inline IDirect3DVertexShader9* GetD3DShader() const;
        //@}

    private:
        /// Direct3D vertex shader instance, or a memory buffer allocated as a staging area if not yet loaded.
        void* m_pShaderData;
        /// True if the shader data pointer is pointing to a staging buffer for loading, false if it is pointing to a
        /// Direct3D vertex shader instance.
        bool m_bStaging;

        /// @name Construction/Destruction
        //@{
        ~D3D9VertexShader();
        //@}
    };
}

#include "D3D9Rendering/D3D9VertexShader.inl"

#endif  // LUNAR_D3D9_RENDERING_D3D9_VERTEX_SHADER_H
