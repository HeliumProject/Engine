//----------------------------------------------------------------------------------------------------------------------
// RRasterizerState.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_R_RASTERIZER_STATE_H
#define LUNAR_RENDERING_R_RASTERIZER_STATE_H

#include "Rendering/RRenderResource.h"

#include "Rendering/RendererTypes.h"

namespace Helium
{
    /// Rasterizer state interface.
    class LUNAR_RENDERING_API RRasterizerState : public RRenderResource
    {
    public:
        /// Rasterization state description.
        struct LUNAR_RENDERING_API Description
        {
            /// Triangle fill mode.
            ERendererFillMode fillMode;
            /// Triangle culling mode.
            ERendererCullMode cullMode;
            /// Triangle front-face winding mode.
            ERendererWinding winding;
            /// Depth value added to a given pixel.
            int32_t depthBias;
            /// Depth bias scale value based on pixel slope.
            float32_t slopeScaledDepthBias;

            /// @name Construction/Destruction
            //@{
            inline Description();
            //@}
        };

        /// @name State Information
        //@{
        virtual void GetDescription( Description& rDescription ) const = 0;
        //@}

    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RRasterizerState() = 0;
        //@}
    };
}

#include "Rendering/RRasterizerState.inl"

#endif  // LUNAR_RENDERING_R_RASTERIZER_STATE_H
