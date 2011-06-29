//----------------------------------------------------------------------------------------------------------------------
// RRenderCommandList.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_RENDERING_R_RENDER_COMMAND_LIST_H
#define HELIUM_RENDERING_R_RENDER_COMMAND_LIST_H

#include "Rendering/RRenderResource.h"

namespace Helium
{
    /// Render command list interface.
    class HELIUM_RENDERING_API RRenderCommandList : public RRenderResource
    {
    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RRenderCommandList() = 0;
        //@}
    };
}

#endif  // HELIUM_RENDERING_R_RENDER_COMMAND_LIST_H
