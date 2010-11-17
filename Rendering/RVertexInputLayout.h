//----------------------------------------------------------------------------------------------------------------------
// RVertexInputLayout.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_R_VERTEX_INPUT_LAYOUT_H
#define LUNAR_RENDERING_R_VERTEX_INPUT_LAYOUT_H

#include "Rendering/RRenderResource.h"

namespace Lunar
{
    /// Interface for the input layout of a shader with regards to a specific vertex format.
    class LUNAR_RENDERING_API RVertexInputLayout : public RRenderResource
    {
    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RVertexInputLayout() = 0;
        //@}
    };
}

#endif  // LUNAR_RENDERING_R_VERTEX_INPUT_LAYOUT_H
