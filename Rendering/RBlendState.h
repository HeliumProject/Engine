#pragma once

#include "Rendering/RRenderResource.h"

#include "Rendering/RendererTypes.h"

namespace Helium
{
    /// Blending state interface.
    class HELIUM_RENDERING_API RBlendState : public RRenderResource
    {
    public:
        /// Blending state description.
        struct HELIUM_RENDERING_API Description
        {
            /// Source color blend factor.
            ERendererBlendFactor sourceFactor;
            /// Destination color blend factor.
            ERendererBlendFactor destinationFactor;
            /// Blend function.
            ERendererBlendFunction function;

            /// Combination of EColorWriteMaskFlag values specifying the color write mask (applied regardless of whether
            /// blending is enabled).
            uint8_t colorWriteMask;

            /// True to enable blending, false to disable.
            bool bBlendEnable;

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
        virtual ~RBlendState() = 0;
        //@}
    };
}

#include "Rendering/RBlendState.inl"
