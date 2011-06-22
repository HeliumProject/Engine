//----------------------------------------------------------------------------------------------------------------------
// RConstantBuffer.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_RENDERING_R_CONSTANT_BUFFER_H
#define HELIUM_RENDERING_R_CONSTANT_BUFFER_H

#include "Rendering/RRenderResource.h"

#include "Rendering/RendererTypes.h"

namespace Helium
{
    /// Shader constant buffer interface.
    class HELIUM_RENDERING_API RConstantBuffer : public RRenderResource
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
        virtual ~RConstantBuffer() = 0;
        //@}
    };
}

#endif  // HELIUM_RENDERING_R_CONSTANT_BUFFER_H
