//----------------------------------------------------------------------------------------------------------------------
// RShader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_R_SHADER_H
#define LUNAR_RENDERING_R_SHADER_H

#include "Rendering/RRenderResource.h"

namespace Helium
{
    /// Interface to a low-level shader resource.
    class LUNAR_RENDERING_API RShader : public RRenderResource
    {
    public:
        /// Shader type identifiers.
        enum EType
        {
            TYPE_FIRST   =  0,
            TYPE_INVALID = -1,

            /// Vertex shader.
            TYPE_VERTEX,
            /// Pixel shader.
            TYPE_PIXEL,

            TYPE_MAX,
            TYPE_LAST = TYPE_MAX - 1
        };

        /// @name Type Information
        //@{
        virtual EType GetType() const = 0;
        //@}

        /// @name Loading
        //@{
        virtual void* Lock() = 0;
        virtual bool Unlock() = 0;
        //@}

    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RShader() = 0;
        //@}
    };
}

#endif  // LUNAR_RENDERING_R_SHADER_H
