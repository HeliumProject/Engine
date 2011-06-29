//----------------------------------------------------------------------------------------------------------------------
// RVertexInputLayout.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_RENDERING_R_VERTEX_INPUT_LAYOUT_H
#define HELIUM_RENDERING_R_VERTEX_INPUT_LAYOUT_H

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

#endif  // HELIUM_RENDERING_R_VERTEX_INPUT_LAYOUT_H
