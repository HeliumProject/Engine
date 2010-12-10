#pragma once

#include "D3D9Rendering/D3D9Rendering.h"
#include "Rendering/RRenderContext.h"

namespace Lunar
{
    L_DECLARE_RPTR( D3D9Surface );

    /// Interface to the a Direct3D 9 render sub-context (one managed through additional swap chains of the main D3D
    /// device).
    class D3D9SubContext : public RRenderContext
    {
    public:
        /// @name Construction/Destruction
        //@{
        D3D9SubContext( IDirect3DSwapChain9* pSwapChain );
        //@}

        /// @name Render Control
        //@{
        RSurface* GetBackBufferSurface();
        void Swap();
        //@}

    private:
        /// Direct3D 9 swap chain instance.
        IDirect3DSwapChain9* m_pSwapChain;
        /// Active backbuffer surface.
        D3D9SurfacePtr m_spBackBufferSurface;

        /// @name Construction/Destruction
        //@{
        ~D3D9SubContext();
        //@}
    };
}
