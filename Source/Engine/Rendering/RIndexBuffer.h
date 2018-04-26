#pragma once

#include "Rendering/RRenderResource.h"

#include "Rendering/RendererTypes.h"

namespace Helium
{
    /// Index buffer interface.
    class HELIUM_RENDERING_API RIndexBuffer : public RRenderResource
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
        virtual ~RIndexBuffer() = 0;
        //@}
    };

    typedef Helium::SmartPtr< RIndexBuffer > RIndexBufferPtr;
}
