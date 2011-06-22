//----------------------------------------------------------------------------------------------------------------------
// RRenderContext.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_RENDERING_R_RENDER_CONTEXT_H
#define HELIUM_RENDERING_R_RENDER_CONTEXT_H

#include "Rendering/RRenderResource.h"

namespace Helium
{
    class RSurface;

    /// Rendering context for managing rendering to a given view.
    class HELIUM_RENDERING_API RRenderContext : public RRenderResource
    {
    public:
        /// @name Render Control
        //@{
        virtual RSurface* GetBackBufferSurface() = 0;
        virtual void Swap() = 0;
        //@}

    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RRenderContext() = 0;
        //@}
    };
}

#endif  // HELIUM_RENDERING_R_RENDER_CONTEXT_H
