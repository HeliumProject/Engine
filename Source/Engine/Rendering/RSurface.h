#pragma once

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
