#pragma once

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RDepthStencilState.h"

namespace Helium
{
    /// Direct3D 9 blend state object.
    class D3D9DepthStencilState : public RDepthStencilState
    {
    public:
        /// Depth comparison function.
        D3DCMPFUNC m_depthFunction;

        /// Stencil operation to perform when stencil testing fails.
        D3DSTENCILOP m_stencilFailOperation;
        /// Stencil operation to perform when stencil testing passes and depth testing fails.
        D3DSTENCILOP m_stencilDepthFailOperation;
        /// Stencil operation to perform when both stencil testing and depth testing pass.
        D3DSTENCILOP m_stencilDepthPassOperation;
        /// Stencil comparison function.
        D3DCMPFUNC m_stencilFunction;

        /// Stencil read mask.
        DWORD m_stencilReadMask;
        /// Stencil write mask.
        DWORD m_stencilWriteMask;

        /// True to enable depth testing.
        BOOL m_bDepthTestEnable;
        /// True to enable depth writing.
        BOOL m_bDepthWriteEnable;

        /// True to enable stencil testing.
        BOOL m_bStencilTestEnable;

        /// @name Initialization
        //@{
        bool Initialize( const Description& rDescription );
        //@}

        /// @name State Information
        //@{
        void GetDescription( Description& rDescription ) const;
        //@}

    private:
        /// @name Construction/Destruction
        //@{
        ~D3D9DepthStencilState();
        //@}
    };
}
