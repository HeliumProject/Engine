//----------------------------------------------------------------------------------------------------------------------
// RSamplerState.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_R_SAMPLER_STATE_H
#define LUNAR_RENDERING_R_SAMPLER_STATE_H

#include "Rendering/RRenderResource.h"

#include "Rendering/RendererTypes.h"

namespace Lunar
{
    /// Sampler state interface.
    class LUNAR_RENDERING_API RSamplerState : public RRenderResource
    {
    public:
        /// Sampler state description.
        struct LUNAR_RENDERING_API Description
        {
            /// Filtering method to use when sampling a texture.
            ERendererTextureFilter filter;

            /// Method for resolving texture u-coordinates outside the 0 to 1 range.
            ERendererTextureAddressMode addressModeU;
            /// Method for resolving texture v-coordinates outside the 0 to 1 range.
            ERendererTextureAddressMode addressModeV;
            /// Method for resolving texture w-coordinates outside the 0 to 1 range.
            ERendererTextureAddressMode addressModeW;

            /// Offset from the calculated mip level.
            uint32_t mipLodBias;

            /// Maximum anisotropy value for anistropic texture filtering.
            uint32_t maxAnisotropy;

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
        virtual ~RSamplerState() = 0;
        //@}
    };
}

#include "Rendering/RSamplerState.inl"

#endif  // LUNAR_RENDERING_R_SAMPLER_STATE_H
