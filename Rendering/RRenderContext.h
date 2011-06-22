//----------------------------------------------------------------------------------------------------------------------
// RRenderContext.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_R_RENDER_CONTEXT_H
#define LUNAR_RENDERING_R_RENDER_CONTEXT_H

#include "Rendering/RRenderResource.h"

namespace Helium
{
    class RSurface;

    /// Rendering context for managing rendering to a given view.
    class LUNAR_RENDERING_API RRenderContext : public RRenderResource
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

#endif  // LUNAR_RENDERING_R_RENDER_CONTEXT_H
