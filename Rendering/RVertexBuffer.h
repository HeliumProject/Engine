//----------------------------------------------------------------------------------------------------------------------
// RVertexBuffer.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_R_VERTEX_BUFFER_H
#define LUNAR_RENDERING_R_VERTEX_BUFFER_H

#include "Rendering/RRenderResource.h"

#include "Rendering/RendererTypes.h"

namespace Lunar
{
    /// Vertex buffer interface.
    class LUNAR_RENDERING_API RVertexBuffer : public RRenderResource
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

#endif  // LUNAR_RENDERING_R_VERTEX_BUFFER_H
