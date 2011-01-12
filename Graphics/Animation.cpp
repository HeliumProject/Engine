//----------------------------------------------------------------------------------------------------------------------
// Animation.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsPch.h"
#include "Graphics/Animation.h"

#if L_USE_GRANNY_ANIMATION
#include "GrannyAnimationInterface.h"
#include "GrannyAnimationInterface.cpp.inl"
#endif

using namespace Lunar;

L_IMPLEMENT_OBJECT( Animation, Graphics, GameObjectType::FLAG_NO_TEMPLATE );

/// Constructor.
Animation::Animation()
{
}

/// Destructor.
Animation::~Animation()
{
}

/// @copydoc GameObject::Serialize()
void Animation::Serialize( Serializer& s )
{
    L_SERIALIZE_BASE( s );

#if L_USE_GRANNY_ANIMATION
    m_grannyData.Serialize( s );
#endif
}

/// @copydoc Resource::SerializePersistentResourceData()
void Animation::SerializePersistentResourceData( Serializer& s )
{
#if L_USE_GRANNY_ANIMATION
    m_grannyData.SerializePersistentResourceData( s );
#else
    HELIUM_UNREF( s );
#endif
}

/// @copydoc Resource::GetCacheName()
Name Animation::GetCacheName() const
{
    static Name cacheName( TXT( "Animation" ) );

    return cacheName;
}
