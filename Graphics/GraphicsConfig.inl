//----------------------------------------------------------------------------------------------------------------------
// GraphicsConfig.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Get the display width.
    ///
    /// @return  Display width.
    ///
    /// @see GetHeight()
    uint32_t GraphicsConfig::GetWidth() const
    {
        return m_width;
    }

    /// Get the display height.
    ///
    /// @return  Display height.
    ///
    /// @see GetWidth()
    uint32_t GraphicsConfig::GetHeight() const
    {
        return m_height;
    }

    /// Get the standard texture filtering mode to use.
    ///
    /// @return  Standard texture filtering mode.
    GraphicsConfig::ETextureFilter GraphicsConfig::GetTextureFiltering() const
    {
        return m_textureFiltering;
    }

    /// Get the max anisotropy value.
    ///
    /// @return  Max anisotropy.
    uint32_t GraphicsConfig::GetMaxAnisotropy() const
    {
        return m_maxAnisotropy;
    }

    /// Get the requested shadow mode.
    ///
    /// @return  Shadow mode.
    GraphicsConfig::EShadowMode GraphicsConfig::GetShadowMode() const
    {
        return m_shadowMode;
    }

    /// Get the size of the shadow buffer.
    ///
    /// @return  Shadow buffer width/height, in texels.
    uint32_t GraphicsConfig::GetShadowBufferSize() const
    {
        return m_shadowBufferSize;
    }

    /// Get whether fullscreen mode is enabled.
    ///
    /// @return  True if fullscreen mode is enabled, false if not.
    bool GraphicsConfig::GetFullscreen() const
    {
        return m_bFullscreen;
    }

    /// Get whether vsync is enabled.
    ///
    /// @return  True if vsync is enabled, false if not.
    bool GraphicsConfig::GetVsync() const
    {
        return m_bVsync;
    }
}
