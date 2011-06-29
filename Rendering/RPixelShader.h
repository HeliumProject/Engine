//----------------------------------------------------------------------------------------------------------------------
// RPixelShader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_RENDERING_R_PIXEL_SHADER_H
#define HELIUM_RENDERING_R_PIXEL_SHADER_H

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

#endif  // HELIUM_RENDERING_R_PIXEL_SHADER_H
