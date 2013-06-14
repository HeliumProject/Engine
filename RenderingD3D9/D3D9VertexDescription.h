#pragma once

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
