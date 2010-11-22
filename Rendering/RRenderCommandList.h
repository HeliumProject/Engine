//----------------------------------------------------------------------------------------------------------------------
// RRenderCommandList.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_R_RENDER_COMMAND_LIST_H
#define LUNAR_RENDERING_R_RENDER_COMMAND_LIST_H

#include "Rendering/RRenderResource.h"

namespace Lunar
{
    /// Render command list interface.
    class LUNAR_RENDERING_API RRenderCommandList : public RRenderResource
    {
    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RRenderCommandList() = 0;
        //@}
    };
}

#endif  // LUNAR_RENDERING_R_RENDER_COMMAND_LIST_H
