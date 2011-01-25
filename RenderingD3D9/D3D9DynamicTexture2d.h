#pragma once

#include "RenderingD3D9/D3D9Texture2d.h"
#include "RenderingD3D9/D3D9DeviceResetListener.h"

namespace Lunar
{
    L_DECLARE_RPTR( D3D9Surface );

    /// Dynamic Direct3D 9 2D texture.
    ///
    /// Dynamic textures are stored in the D3DPOOL_DEFAULT pool, which requires them to be recreated on device reset.
    ///
    /// Note that when using the Direct3D 9Ex device, this class is not used, as Direct3D 9Ex does not lose resources
    /// when the device is lost.
    class D3D9DynamicTexture2d : public D3D9Texture2d, public D3D9DeviceResetListener
    {
    public:
        /// Maximum number of mip levels supported for a given texture.
        static const size_t LEVEL_COUNT_MAX = 32;

        /// @name Construction/Destruction
        //@{
        D3D9DynamicTexture2d( IDirect3DTexture9* pD3DTexture, bool bSrgb );
        //@}

        /// @name Device Reset Event Handlers
        //@{
        void OnPreReset();
        void OnPostReset( D3D9Renderer* pRenderer );
        //@}

    private:
        /// Cached width of the base mip level.
        uint32_t m_width;
        /// Cached height of the base mip level.
        uint32_t m_height;
        /// Direct3D pixel format.
        D3DFORMAT m_format;
        /// Cached number of mip levels, minus one.
        uint32_t m_mipLevelCountMinusOne : 5;
        /// Usage (see ERendererBufferUsage).
        uint32_t m_usage : 2;

        /// Cached D3D9Surface objects for each mip surface.
        D3D9SurfacePtr m_surfaces[ LEVEL_COUNT_MAX ];

        /// @name Construction/Destruction
        //@{
        ~D3D9DynamicTexture2d();
        //@}
    };
}
