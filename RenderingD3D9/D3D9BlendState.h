//----------------------------------------------------------------------------------------------------------------------
// D3D9BlendState.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_D3D9_D3D9_BLEND_STATE_H
#define LUNAR_RENDERING_D3D9_D3D9_BLEND_STATE_H

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RBlendState.h"

namespace Helium
{
    /// Direct3D 9 blend state object.
    class D3D9BlendState : public RBlendState
    {
    public:
        /// Source blend factor.
        D3DBLEND m_sourceFactor;
        /// Destination blend factor.
        D3DBLEND m_destinationFactor;
        /// Blend function.
        D3DBLENDOP m_function;
        /// Color write mask.
        UINT m_colorWriteMask;
        /// True to enable alpha blending.
        BOOL m_bBlendEnable;

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
        ~D3D9BlendState();
        //@}
    };
}

#endif  // LUNAR_RENDERING_D3D9_D3D9_BLEND_STATE_H
