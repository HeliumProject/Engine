#pragma once

#include "Rendering/RRenderResource.h"

namespace Helium
{
    /// Render command list interface.
    class HELIUM_RENDERING_API RRenderCommandList : public RRenderResource
    {
    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RRenderCommandList() = 0;
        //@}
    };
}
