//----------------------------------------------------------------------------------------------------------------------
// D3D9RasterizerState.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_D3D9_D3D9_RASTERIZER_STATE_H
#define LUNAR_RENDERING_D3D9_D3D9_RASTERIZER_STATE_H

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RRasterizerState.h"

namespace Lunar
{
    /// Direct3D 9 rasterizer state object.
    class D3D9RasterizerState : public RRasterizerState
    {
    public:
        /// Fill mode.
        D3DFILLMODE m_fillMode;
        /// Cull mode.
        D3DCULL m_cullMode;
        /// Face winding.
        ERendererWinding m_winding;
        /// Depth bias.
        float32_t m_depthBias;
        /// Slope-scaled depth bias.
        float32_t m_slopeScaledDepthBias;

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
        ~D3D9RasterizerState();
        //@}
    };
}

#endif  // LUNAR_RENDERING_D3D9_D3D9_RASTERIZER_STATE_H
