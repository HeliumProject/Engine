//----------------------------------------------------------------------------------------------------------------------
// D3D9DepthStencilState.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_D3D9_RENDERING_D3D9_DEPTH_STENCIL_STATE_H
#define LUNAR_D3D9_RENDERING_D3D9_DEPTH_STENCIL_STATE_H

#include "D3D9Rendering/D3D9Rendering.h"
#include "Rendering/RDepthStencilState.h"

namespace Lunar
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

#endif  // LUNAR_D3D9_RENDERING_D3D9_DEPTH_STENCIL_STATE_H
