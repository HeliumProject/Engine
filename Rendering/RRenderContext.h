#pragma once

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
