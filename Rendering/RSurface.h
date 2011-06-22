//----------------------------------------------------------------------------------------------------------------------
// RSurface.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_RENDERING_R_SURFACE_H
#define HELIUM_RENDERING_R_SURFACE_H

#include "Rendering/RRenderResource.h"

namespace Helium
{
    /// Interface for a renderable surface.
    class HELIUM_RENDERING_API RSurface : public RRenderResource
    {
    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RSurface() = 0;
        //@}
    };
}

#endif  // HELIUM_RENDERING_R_SURFACE_H
