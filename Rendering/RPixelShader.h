//----------------------------------------------------------------------------------------------------------------------
// RPixelShader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_R_PIXEL_SHADER_H
#define LUNAR_RENDERING_R_PIXEL_SHADER_H

#include "Rendering/RShader.h"

namespace Helium
{
    /// Pixel shader interface.
    class LUNAR_RENDERING_API RPixelShader : public RShader
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

#endif  // LUNAR_RENDERING_R_PIXEL_SHADER_H
