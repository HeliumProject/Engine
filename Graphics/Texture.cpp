#include "GraphicsPch.h"
#include "Graphics/Texture.h"

#include "Rendering/RTexture.h"

#include "Reflect/DataDeduction.h"

REFLECT_DEFINE_ENUMERATION( Helium::Texture::ECompression );
HELIUM_IMPLEMENT_ASSET( Helium::Texture, Graphics, AssetType::FLAG_ABSTRACT | AssetType::FLAG_NO_TEMPLATE );

using namespace Helium;

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

/// @copydoc Asset::PreDestroy()
void Texture::PreDestroy()
{
    m_spTexture.Release();

    Base::PreDestroy();
}


//PMDTODO: Implement this
///// @copydoc Asset::Serialize()
//void Texture::Serialize( Serializer& s )
//{
//    HELIUM_SERIALIZE_BASE( s );
//
//    s << HELIUM_TAGGED( m_compression );
//    s << HELIUM_TAGGED( m_bSrgb );
//    s << HELIUM_TAGGED( m_bCreateMipmaps );
//    s << HELIUM_TAGGED( m_bIgnoreAlpha );
//}

void Texture::PopulateStructure( Reflect::Structure& comp )
{
    comp.AddField( &Texture::m_compression,  TXT( "m_compression" ) );
    comp.AddField( &Texture::m_bSrgb,          TXT( "m_bSrgb" ) );
    comp.AddField( &Texture::m_bCreateMipmaps, TXT( "m_bCreateMipmaps" ) );
    comp.AddField( &Texture::m_bIgnoreAlpha,   TXT( "m_bIgnoreAlpha" ) );
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
