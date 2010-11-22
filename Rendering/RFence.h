//----------------------------------------------------------------------------------------------------------------------
// RFence.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#ifndef LUNAR_RENDERING_R_FENCE_H
#define LUNAR_RENDERING_R_FENCE_H

#include "Rendering/RRenderResource.h"

namespace Lunar
{
    /// GPU command buffer fence interface.
    class LUNAR_RENDERING_API RFence : public RRenderResource
    {
    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RFence() = 0;
        //@}
    };
}

#endif  // LUNAR_RENDERING_R_FENCE_H
