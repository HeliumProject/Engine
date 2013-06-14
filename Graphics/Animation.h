#pragma once

#include "Graphics/Graphics.h"
#include "Engine/Resource.h"

#include "GraphicsTypes/GraphicsTypes.h"

#if HELIUM_USE_GRANNY_ANIMATION
#include "GrannyAnimationInterface.h"
#endif

namespace Helium
{
    /// Animation resource data.
    class HELIUM_GRAPHICS_API Animation : public Resource
    {
        HELIUM_DECLARE_ASSET( Animation, Resource );

    public:
        /// @name Construction/Destruction
        //@{
        Animation();
        virtual ~Animation();
        //@}

        /// @name Resource Caching Support
        //@{
        virtual Name GetCacheName() const;
        //@}

        /// @name Data Access
        //@{
#if HELIUM_USE_GRANNY_ANIMATION
        inline const Granny::AnimationData& GetGrannyData() const;
#endif
        //@}

    private:
#if HELIUM_USE_GRANNY_ANIMATION
        /// Granny-specific animation data.
        Granny::AnimationData m_grannyData;
#endif
    };
}

#include "Graphics/Animation.inl"
