#pragma once

#include "Rendering/RShader.h"

namespace Helium
{
    /// Pixel shader interface.
    class HELIUM_RENDERING_API RPixelShader : public RShader
    {
    public:
        /// @name Type Information
        //@{
        EType GetType() const;
        //@}

    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RPixelShader() = 0;
        //@}
    };
}
