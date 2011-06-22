//----------------------------------------------------------------------------------------------------------------------
// Animation.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_GRAPHICS_ANIMATION_H
#define LUNAR_GRAPHICS_ANIMATION_H

#include "Graphics/Graphics.h"
#include "Engine/Resource.h"

#include "GraphicsTypes/GraphicsTypes.h"

#if L_USE_GRANNY_ANIMATION
#include "GrannyAnimationInterface.h"
#endif

namespace Helium
{
    /// Animation resource data.
    class LUNAR_GRAPHICS_API Animation : public Resource
    {
        L_DECLARE_OBJECT( Animation, Resource );

    public:
        /// @name Construction/Destruction
        //@{
        Animation();
        virtual ~Animation();
        //@}

        /// @name Serialization
        //@{
        virtual void Serialize( Serializer& s );
        //@}

        /// @name Resource Serialization
        //@{
        virtual void SerializePersistentResourceData( Serializer& s );
        //@}

        /// @name Resource Caching Support
        //@{
        virtual Name GetCacheName() const;
        //@}

        /// @name Data Access
        //@{
#if L_USE_GRANNY_ANIMATION
        inline const Granny::AnimationData& GetGrannyData() const;
#endif
        //@}

    private:
#if L_USE_GRANNY_ANIMATION
        /// Granny-specific animation data.
        Granny::AnimationData m_grannyData;
#endif
    };
}

#include "Graphics/Animation.inl"

#endif  // LUNAR_GRAPHICS_ANIMATION_H
