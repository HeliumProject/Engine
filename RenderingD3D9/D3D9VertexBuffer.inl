//----------------------------------------------------------------------------------------------------------------------
// D3D9VertexBuffer.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the Direct3D vertex buffer.
    ///
    /// @return  Direct3D vertex buffer instance.
    IDirect3DVertexBuffer9* D3D9VertexBuffer::GetD3DBuffer() const
    {
        return m_pBuffer;
    }
}
