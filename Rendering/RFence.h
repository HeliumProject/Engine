#pragma once

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
