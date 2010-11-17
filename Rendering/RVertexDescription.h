//----------------------------------------------------------------------------------------------------------------------
// RVertexDescription.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_R_VERTEX_DESCRIPTION_H
#define LUNAR_RENDERING_R_VERTEX_DESCRIPTION_H

#include "Rendering/RRenderResource.h"

#include "Rendering/RendererTypes.h"

namespace Lunar
{
    /// Vertex layout description interface.
    class LUNAR_RENDERING_API RVertexDescription : public RRenderResource
    {
    public:
        /// Vertex element description.
        struct LUNAR_RENDERING_API Element
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

#endif  // LUNAR_RENDERING_R_VERTEX_DESCRIPTION_H
