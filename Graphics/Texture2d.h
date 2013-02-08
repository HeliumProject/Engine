//----------------------------------------------------------------------------------------------------------------------
// Texture2d.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_GRAPHICS_TEXTURE_2D_H
#define HELIUM_GRAPHICS_TEXTURE_2D_H

#include "Graphics/Texture.h"

namespace Helium
{
    /// 2D texture resource.
    class HELIUM_GRAPHICS_API Texture2d : public Texture
    {
        HELIUM_DECLARE_OBJECT( Texture2d, Texture );

    public:
        /// @name Construction/Destruction
        //@{
        Texture2d();
        virtual ~Texture2d();
        //@}

        struct HELIUM_GRAPHICS_API PersistentResourceData : public Object
        {
            REFLECT_DECLARE_OBJECT(Texture2d::PersistentResourceData, Reflect::Object);

            PersistentResourceData();
            static void PopulateComposite( Reflect::Composite& comp );

            uint32_t m_baseLevelWidth;
            uint32_t m_baseLevelHeight;
            uint32_t m_mipCount;
            int32_t m_pixelFormatIndex;
        };

        /// Persistent texture resource data.
        PersistentResourceData m_persistentResourceData;

        /// @name Serialization
        //@{
        virtual bool NeedsPrecacheResourceData() const;
        virtual bool BeginPrecacheResourceData();
        virtual bool TryFinishPrecacheResourceData();
        //@}

        /// @name Resource Serialization
        //@{
        virtual bool LoadPersistentResourceObject( Reflect::ObjectPtr& _object );
        //@}

        /// @name Data Access
        //@{
        RTexture2d* GetRenderResource2d() const;
        //@}

    private:
        /// Async load IDs for cached texture data.
        DynamicArray< size_t > m_renderResourceLoadIds;
    };
}

#endif  // HELIUM_GRAPHICS_TEXTURE_2D_H
