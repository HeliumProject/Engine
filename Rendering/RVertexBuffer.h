#pragma once

#include "Rendering/RRenderResource.h"

#include "Rendering/RendererTypes.h"

namespace Helium
{
    /// Vertex buffer interface.
    class HELIUM_RENDERING_API RVertexBuffer : public RRenderResource
    {
    public:
        /// @name Data Access
        //@{
        virtual void* Map( ERendererBufferMapHint hint = RENDERER_BUFFER_MAP_HINT_NONE ) = 0;
        virtual void Unmap() = 0;
        //@}

    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RVertexBuffer() = 0;
        //@}
    };
}
