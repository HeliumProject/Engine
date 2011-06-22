#pragma once

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RRenderContext.h"
#include "RenderingD3D9/D3D9DeviceResetListener.h"

namespace Helium
{
    L_DECLARE_RPTR( D3D9Surface );

    /// Interface to the a Direct3D 9 render sub-context (one managed through additional swap chains of the main D3D
    /// device).
    class D3D9SubContext : public RRenderContext, public D3D9DeviceResetListener
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

        /// @name Device Reset Event Handlers
        //@{
        void OnPreReset();
        void OnPostReset( D3D9Renderer* pRenderer );
        //@}

    private:
        /// Direct3D 9 swap chain instance.
        IDirect3DSwapChain9* m_pSwapChain;
        /// Active backbuffer surface.
        D3D9SurfacePtr m_spBackBufferSurface;

        /// Cached presentation parameters.
        D3DPRESENT_PARAMETERS m_cachedPresentParameters;

        /// @name Construction/Destruction
        //@{
        ~D3D9SubContext();
        //@}
    };
}
