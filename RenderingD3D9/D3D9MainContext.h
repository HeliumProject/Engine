//----------------------------------------------------------------------------------------------------------------------
// D3D9MainContext.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_D3D9_D3D9_MAIN_CONTEXT_H
#define LUNAR_RENDERING_D3D9_D3D9_MAIN_CONTEXT_H

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RRenderContext.h"

namespace Helium
{
    L_DECLARE_RPTR( D3D9Surface );

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

#endif  // LUNAR_RENDERING_D3D9_D3D9_MAIN_CONTEXT_H
