#pragma once

#include "Rendering/RRenderResource.h"

#include "Rendering/RendererTypes.h"

namespace Helium
{
    /// Depth-stencil state interface.
    class HELIUM_RENDERING_API RDepthStencilState : public RRenderResource
    {
    public:
        /// Depth-stencil state description.
        struct HELIUM_RENDERING_API Description
        {
            /// Depth comparison function.
            ERendererCompareFunction depthFunction;

            /// Stencil operation to perform when stencil testing fails.
            ERendererStencilOperation stencilFailOperation;
            /// Stencil operation to perform when stencil testing passes and depth testing fails.
            ERendererStencilOperation stencilDepthFailOperation;
            /// Stencil operation to perform when both stencil testing and depth testing pass.
            ERendererStencilOperation stencilDepthPassOperation;
            /// Stencil comparison function.
            ERendererCompareFunction stencilFunction;

            /// Stencil read mask.
            uint8_t stencilReadMask;
            /// Stencil write mask.
            uint8_t stencilWriteMask;

            /// True to enable depth testing.
            bool bDepthTestEnable;
            /// True to enable depth writing.
            bool bDepthWriteEnable;

            /// True to enable stencil testing.
            bool bStencilTestEnable;

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
        virtual ~RDepthStencilState() = 0;
        //@}
    };
}

#include "Rendering/RDepthStencilState.inl"
