//----------------------------------------------------------------------------------------------------------------------
// RSurface.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_R_SURFACE_H
#define LUNAR_RENDERING_R_SURFACE_H

#include "Rendering/RRenderResource.h"

namespace Lunar
{
    /// Interface for a renderable surface.
    class LUNAR_RENDERING_API RSurface : public RRenderResource
    {
    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RSurface() = 0;
        //@}
    };
}

#endif  // LUNAR_RENDERING_R_SURFACE_H
