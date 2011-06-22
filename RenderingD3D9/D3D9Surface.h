//----------------------------------------------------------------------------------------------------------------------
// D3D9Surface.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_D3D9_D3D9_SURFACE_H
#define LUNAR_RENDERING_D3D9_D3D9_SURFACE_H

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RSurface.h"

namespace Helium
{
    /// Wrapper for a Direct3D 9 surface.
    class D3D9Surface : public RSurface
    {
    public:
        /// @name Construction/Destruction
        //@{
        D3D9Surface( IDirect3DSurface9* pD3DSurface, bool bSrgb );
        //@}

        /// @name Data Access
        //@{
        inline IDirect3DSurface9* GetD3DSurface() const;
        inline bool IsSrgb() const;

        void SetD3DSurface( IDirect3DSurface9* pSurface );
        //@}

    protected:
        /// Reference to the Direct3D 9 surface.
        IDirect3DSurface9* m_pSurface;
        /// True if gamma correction to sRGB should be applied when writing to this surface.
        bool m_bSrgb;

        /// @name Construction/Destruction
        //@{
        virtual ~D3D9Surface();
        //@}
    };
}

#include "RenderingD3D9/D3D9Surface.inl"

#endif  // LUNAR_RENDERING_D3D9_D3D9_SURFACE_H
