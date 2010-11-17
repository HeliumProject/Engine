//----------------------------------------------------------------------------------------------------------------------
// D3D9VertexInputLayout.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_D3D9_RENDERING_D3D9_VERTEX_INPUT_LAYOUT_H
#define LUNAR_D3D9_RENDERING_D3D9_VERTEX_INPUT_LAYOUT_H

#include "D3D9Rendering/D3D9Rendering.h"
#include "Rendering/RVertexInputLayout.h"

namespace Lunar
{
    /// Direct3D 9 vertex input layout implementation.
    class D3D9VertexInputLayout : public RVertexInputLayout
    {
    public:
        /// @name Construction/Destruction
        //@{
        D3D9VertexInputLayout( IDirect3DVertexDeclaration9* pD3DDeclaration );
        //@}

        /// @name Data Access
        //@{
        inline IDirect3DVertexDeclaration9* GetD3DDeclaration() const;
        //@}

    private:
        /// Vertex declaration.
        IDirect3DVertexDeclaration9* m_pDeclaration;

        /// @name Construction/Destruction
        //@{
        ~D3D9VertexInputLayout();
        //@}
    };
}

#include "D3D9Rendering/D3D9VertexInputLayout.inl"

#endif  // LUNAR_D3D9_RENDERING_D3D9_VERTEX_INPUT_LAYOUT_H
