namespace Helium
{
    /// Get the Direct3D vertex shader.
    ///
    /// @return  Direct3D vertex shader instance.
    IDirect3DVertexShader9* D3D9VertexShader::GetD3DShader() const
    {
        return ( m_bStaging ? NULL : static_cast< IDirect3DVertexShader9* >( m_pShaderData ) );
    }
}
