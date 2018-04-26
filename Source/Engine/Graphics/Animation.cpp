#include "Precompile.h"
#include "Graphics/Animation.h"

#if HELIUM_USE_GRANNY_ANIMATION
#include "GrannyAnimationInterface.h"
#include "GrannyAnimationInterface.cpp.inl"
#endif

HELIUM_IMPLEMENT_ASSET( Helium::Animation, Graphics, AssetType::FLAG_NO_TEMPLATE );

using namespace Helium;

/// Constructor.
Animation::Animation()
{
}

/// Destructor.
Animation::~Animation()
{
}

/// @copydoc Resource::GetCacheName()
Name Animation::GetCacheName() const
{
    static Name cacheName( "Animation" );

    return cacheName;
}
