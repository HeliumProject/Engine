#pragma once

#include "Rendering/RRenderResource.h"

namespace Helium
{
    /// Interface for the input layout of a shader with regards to a specific vertex format.
    class HELIUM_RENDERING_API RVertexInputLayout : public RRenderResource
    {
    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RVertexInputLayout() = 0;
        //@}
    };
}
