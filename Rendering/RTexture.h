//----------------------------------------------------------------------------------------------------------------------
// RTexture.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_R_TEXTURE_H
#define LUNAR_RENDERING_R_TEXTURE_H

#include "Rendering/RRenderResource.h"

namespace Helium
{
    /// Base interface to texture resources.
    class LUNAR_RENDERING_API RTexture : public RRenderResource
    {
    public:
        /// Texture type identifiers.
        enum EType
        {
            TYPE_FIRST   =  0,
            TYPE_INVALID = -1,

            /// 2D texture.
            TYPE_2D,

            TYPE_MAX,
            TYPE_LAST = TYPE_MAX - 1
        };

        /// @name Type Information
        //@{
        virtual EType GetType() const = 0;
        //@}

        /// @name Base Texture Information
        //@{
        virtual uint32_t GetMipCount() const = 0;
        //@}

    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RTexture() = 0;
        //@}
    };
}

#endif  // LUNAR_RENDERING_R_TEXTURE_H
