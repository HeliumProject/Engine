//----------------------------------------------------------------------------------------------------------------------
// D3D9Renderer.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the main Direct3D 9 interface.
    ///
    /// @return  Direct3D 9 interface.
    ///
    /// @see GetD3DDevice()
    IDirect3D9* D3D9Renderer::GetD3D() const
    {
        return m_pD3D;
    }

    /// Get the main Direct3D 9 device.
    ///
    /// @return  Direct3D 9 device.
    ///
    /// @see GetD3D()
    IDirect3DDevice9* D3D9Renderer::GetD3DDevice() const
    {
        return m_pD3DDevice;
    }

    /// Get the depth texture format to use for the current driver implementation.
    ///
    /// This is the format that will be used when creating textures using the RENDERER_PIXEL_FORMAT_DEPTH format.
    ///
    /// @return  Depth texture format, or D3DFMT_UNKNOWN if the platform doesn't support depth textures.
    D3DFORMAT D3D9Renderer::GetDepthTextureFormat() const
    {
        return m_depthTextureFormat;
    }

    /// Get the GUID associated with private data stored by the engine in Direct3D resources.
    ///
    /// @return  GUID for Direct3D resource private data.
    const GUID& D3D9Renderer::GetPrivateDataGuid()
    {
        return sm_privateDataGuid;
    }
}
