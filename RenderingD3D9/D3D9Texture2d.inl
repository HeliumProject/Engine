namespace Helium
{
    /// Get the Direct3D texture.
    ///
    /// @return  Direct3D texture instance.
    IDirect3DTexture9* D3D9Texture2d::GetD3DTexture() const
    {
        return m_pTexture;
    }

    /// Get whether this texture is in sRGB color space.
    ///
    /// @return  True if texture samples are stored in sRGB color space, false if they are in linear space.
    bool D3D9Texture2d::IsSrgb() const
    {
        return m_bSrgb;
    }
}
