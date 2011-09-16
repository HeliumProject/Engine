//----------------------------------------------------------------------------------------------------------------------
// Animation.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsPch.h"
#include "Graphics/Animation.h"

#if HELIUM_USE_GRANNY_ANIMATION
#include "GrannyAnimationInterface.h"
#include "GrannyAnimationInterface.cpp.inl"
#endif

using namespace Helium;

HELIUM_IMPLEMENT_OBJECT( Animation, Graphics, GameObjectType::FLAG_NO_TEMPLATE );

/// Constructor.
Animation::Animation()
{
}

/// Destructor.
Animation::~Animation()
{
}

//PMDTODO: Implement this
///// @copydoc GameObject::Serialize()
//void Animation::Serialize( Serializer& s )
//{
//    HELIUM_SERIALIZE_BASE( s );
//
//#if HELIUM_USE_GRANNY_ANIMATION
//    m_grannyData.Serialize( s );
//#endif
//}
//
///// @copydoc Resource::SerializePersistentResourceData()
//void Animation::SerializePersistentResourceData( Serializer& s )
//{
//#if HELIUM_USE_GRANNY_ANIMATION
//    m_grannyData.SerializePersistentResourceData( s );
//#else
//    HELIUM_UNREF( s );
//#endif
//}

/// @copydoc Resource::GetCacheName()
Name Animation::GetCacheName() const
{
    static Name cacheName( TXT( "Animation" ) );

    return cacheName;
}
