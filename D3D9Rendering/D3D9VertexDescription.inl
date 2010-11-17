//----------------------------------------------------------------------------------------------------------------------
// D3D9VertexDescription.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the Direct3D vertex declaration for this vertex description.
    ///
    /// @return  Direct3D vertex declaration.
    IDirect3DVertexDeclaration9* D3D9VertexDescription::GetD3DDeclaration() const
    {
        return m_pDeclaration;
    }
}
