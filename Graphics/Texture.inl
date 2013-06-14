namespace Helium
{
    /// Get the render resource for this texture.
    ///
    /// @return  Render resource pointer.  It's reference count is not automatically incremented.
    RTexture* Texture::GetRenderResource() const
    {
        return m_spTexture;
    }

    /// Get the compression scheme for this texture.
    ///
    /// @return  Texture compression scheme.
    Texture::ECompression Texture::GetCompression() const
    {
        return m_compression;
    }

    /// Get whether this texture is in sRGB color space.
    ///
    /// @return  True if this texture is in sRGB color space, false if not.
    bool Texture::GetSrgb() const
    {
        return ( m_bSrgb && !IsNormalMapCompression( m_compression ) );
    }

    /// Get whether mipmaps should be created during resource preprocessing.
    ///
    /// @return  True if mipmaps should be generated, false if not.
    bool Texture::GetCreateMipmaps() const
    {
        return m_bCreateMipmaps;
    }

    /// Get whether the alpha channel in the source texture should be ignored.
    ///
    /// @return  True if the alpha channel should be ignored, false if not.
    bool Texture::GetIgnoreAlpha() const
    {
        return m_bIgnoreAlpha;
    }

    /// Get whether the specified compression scheme is a normal map compression scheme.
    ///
    /// @return  True if the compression scheme is a normal map compression scheme, false if not.
    bool Texture::IsNormalMapCompression( ECompression compression )
    {
        return ( compression == ECompression::NORMAL_MAP || compression == ECompression::NORMAL_MAP_COMPACT );
    }
}
