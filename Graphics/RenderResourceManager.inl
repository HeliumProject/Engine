//----------------------------------------------------------------------------------------------------------------------
// RenderResourceManager.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the effective shadow mode currently in use.
    ///
    /// @return  Effective shadow mode.
    GraphicsConfig::EShadowMode RenderResourceManager::GetShadowMode() const
    {
        return m_shadowMode;
    }

    /// Get the usable size of the shadow depth texture.
    ///
    /// @return  Size of the shadow depth texture to be actually used during rendering.  This is cached from the
    ///          graphics configuration settings for easy access.
    ///
    /// @see GetShadowDepthTexture()
    uint32_t RenderResourceManager::GetShadowDepthTextureUsableSize() const
    {
        return m_shadowDepthTextureUsableSize;
    }
}
