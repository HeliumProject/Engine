//----------------------------------------------------------------------------------------------------------------------
// D3D9VertexDescription.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_D3D9_D3D9_VERTEX_DESCRIPTION_H
#define LUNAR_RENDERING_D3D9_D3D9_VERTEX_DESCRIPTION_H

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RVertexDescription.h"

namespace Helium
{
    /// Direct3D 9 vertex description implementation.
    class D3D9VertexDescription : public RVertexDescription
    {
    public:
        /// @name Construction/Destruction
        //@{
        D3D9VertexDescription( IDirect3DVertexDeclaration9* pD3DDeclaration );
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
        ~D3D9VertexDescription();
        //@}
    };
}

#include "RenderingD3D9/D3D9VertexDescription.inl"

#endif  // LUNAR_RENDERING_D3D9_D3D9_VERTEX_DESCRIPTION_H
