#pragma once

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RRenderContext.h"

namespace Helium
{
    HELIUM_DECLARE_RPTR( D3D9Surface );

    /// Interface to the main Direct3D 9 render context (that managed directly by the IDirect3DDevice9 instance).
    class D3D9MainContext : public RRenderContext
    {
    public:
        /// @name Construction/Destruction
        //@{
        D3D9MainContext( IDirect3DDevice9* pD3DDevice );
        //@}

        /// @name Render Control
        //@{
        RSurface* GetBackBufferSurface();
        void Swap();

        void ReleaseBackBufferSurface();
        //@}

    private:
        /// Direct3D 9 device instance.
        IDirect3DDevice9* m_pDevice;
        /// Active backbuffer surface.
        D3D9SurfacePtr m_spBackBufferSurface;

        /// @name Construction/Destruction
        //@{
        ~D3D9MainContext();
        //@}
    };
}
