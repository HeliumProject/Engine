//----------------------------------------------------------------------------------------------------------------------
// Texture.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsPch.h"
#include "Graphics/Texture.h"

#include "Rendering/RTexture.h"

using namespace Helium;

HELIUM_DEFINE_ENUMERATION( Texture::ECompression, HELIUM_GRAPHICS_API );
HELIUM_IMPLEMENT_OBJECT( Texture, Graphics, GameObjectType::FLAG_ABSTRACT | GameObjectType::FLAG_NO_TEMPLATE );

/// Constructor.
Texture::Texture()
: m_compression( ECompression::COLOR_SMOOTH_ALPHA )
, m_bSrgb( true )
, m_bCreateMipmaps( true )
, m_bIgnoreAlpha( false )
{
}

/// Destructor.
Texture::~Texture()
{
}

/// @copydoc GameObject::PreDestroy()
void Texture::PreDestroy()
{
    m_spTexture.Release();

    Base::PreDestroy();
}

/// @copydoc GameObject::Serialize()
void Texture::Serialize( Serializer& s )
{
    HELIUM_SERIALIZE_BASE( s );

    s << HELIUM_TAGGED( m_compression );
    s << HELIUM_TAGGED( m_bSrgb );
    s << HELIUM_TAGGED( m_bCreateMipmaps );
    s << HELIUM_TAGGED( m_bIgnoreAlpha );
}

/// Get the render resource as a RTexture2d if this is a 2D texture.
///
/// @return  Pointer to the RTexture2d object for this texture if it is a 2D texture, null if it is not or if there
///          is no render resource allocated.
RTexture2d* Texture::GetRenderResource2d() const
{
    return NULL;
}

/// @copydoc Resource::GetCacheName()
Name Texture::GetCacheName() const
{
    static Name cacheName( TXT( "Texture" ) );

    return cacheName;
}
