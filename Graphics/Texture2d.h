//----------------------------------------------------------------------------------------------------------------------
// Texture2d.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_GRAPHICS_TEXTURE_2D_H
#define LUNAR_GRAPHICS_TEXTURE_2D_H

#include "Graphics/Texture.h"

namespace Lunar
{
    /// 2D texture resource.
    class LUNAR_GRAPHICS_API Texture2d : public Texture
    {
        L_DECLARE_OBJECT( Texture2d, Texture );

    public:
        /// @name Construction/Destruction
        //@{
        Texture2d();
        virtual ~Texture2d();
        //@}

        /// @name Serialization
        //@{
        virtual bool NeedsPrecacheResourceData() const;
        virtual bool BeginPrecacheResourceData();
        virtual bool TryFinishPrecacheResourceData();
        //@}

        /// @name Resource Serialization
        //@{
        virtual void SerializePersistentResourceData( Serializer& s );
        //@}

        /// @name Data Access
        //@{
        RTexture2d* GetRenderResource2d() const;
        //@}

    private:
        /// Async load IDs for cached texture data.
        DynArray< size_t > m_renderResourceLoadIds;
    };
}

#endif  // LUNAR_GRAPHICS_TEXTURE_2D_H
