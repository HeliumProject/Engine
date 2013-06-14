namespace Helium
{
    /// Get the Direct3D index buffer.
    ///
    /// @return  Direct3D index buffer instance.
    IDirect3DIndexBuffer9* D3D9IndexBuffer::GetD3DBuffer() const
    {
        return m_pBuffer;
    }
}
