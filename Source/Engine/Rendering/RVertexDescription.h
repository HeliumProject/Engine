#pragma once

#include "Rendering/RRenderResource.h"

#include "Rendering/RendererTypes.h"

namespace Helium
{
    /// Vertex layout description interface.
    class HELIUM_RENDERING_API RVertexDescription : public RRenderResource
    {
    public:
        /// Vertex element description.
        struct HELIUM_RENDERING_API Element
        {
            /// Data type.
            ERendererVertexDataType type;
            /// Input usage semantic.
            ERendererVertexSemantic semantic;
            /// Usage semantic index.
            uint8_t semanticIndex;
            /// Input vertex buffer index.
            uint8_t bufferIndex;

            /// @name Construction/Destruction
            //@{
            inline Element();
            //@}
        };

    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RVertexDescription() = 0;
        //@}
    };
}

#include "Rendering/RVertexDescription.inl"
