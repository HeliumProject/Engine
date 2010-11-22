//----------------------------------------------------------------------------------------------------------------------
// Texture.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsPch.h"
#include "Graphics/Texture.h"

#include "Rendering/RTexture.h"

namespace Lunar
{
    L_IMPLEMENT_OBJECT( Texture, Graphics, Type::FLAG_ABSTRACT | Type::FLAG_NO_TEMPLATE );

    /// Constructor.
    Texture::Texture()
        : m_compression( COMPRESSION_COLOR_SMOOTH_ALPHA )
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

        Super::PreDestroy();
    }

    /// @copydoc GameObject::Serialize()
    void Texture::Serialize( Serializer& s )
    {
        L_SERIALIZE_SUPER( s );

        s << L_TAGGED( m_compression );
        s << L_TAGGED( m_bSrgb );
        s << L_TAGGED( m_bCreateMipmaps );
        s << L_TAGGED( m_bIgnoreAlpha );
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
}
