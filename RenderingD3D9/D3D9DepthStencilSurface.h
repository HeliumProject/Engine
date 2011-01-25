#pragma once

#include "RenderingD3D9/D3D9Surface.h"
#include "RenderingD3D9/D3D9DeviceResetListener.h"

namespace Lunar
{
    /// Wrapper for a restorable Direct3D 9 depth-stencil surface.
    ///
    /// Direct3D 9 depth-stencil surfaces can be lost if the device is lost and need to be restored when the device is
    /// reset.
    ///
    /// Note that when using the Direct3D 9Ex device, this class is not used, as Direct3D 9Ex does not lose resources
    /// when the device is lost.
    class D3D9DepthStencilSurface : public D3D9Surface, public D3D9DeviceResetListener
    {
    public:
        /// @name Construction/Destruction
        //@{
        D3D9DepthStencilSurface( IDirect3DSurface9* pD3DSurface, bool bSrgb );
        //@}

        /// @name Device Reset Event Handlers
        //@{
        void OnPreReset();
        void OnPostReset( D3D9Renderer* pRenderer );
        //@}

    private:
        /// Surface width, in pixels.
        uint32_t m_width;
        /// Surface height, in pixels.
        uint32_t m_height;

        /// One less than the multisample count if 2 or greater, zero if not using multisampling.
        uint32_t m_multisampleCountMinusOne : 4;
        /// Non-zero if using a stencil buffer, zero if not.
        uint32_t m_bStencil : 1;

        /// @name Construction/Destruction
        //@{
        ~D3D9DepthStencilSurface();
        //@}
    };
}
