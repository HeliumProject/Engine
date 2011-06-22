//----------------------------------------------------------------------------------------------------------------------
// RFence.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#ifndef HELIUM_RENDERING_R_FENCE_H
#define HELIUM_RENDERING_R_FENCE_H

#include "Rendering/RRenderResource.h"

namespace Helium
{
    /// GPU command buffer fence interface.
    class HELIUM_RENDERING_API RFence : public RRenderResource
    {
    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RFence() = 0;
        //@}
    };
}

#endif  // HELIUM_RENDERING_R_FENCE_H
