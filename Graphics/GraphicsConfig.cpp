//----------------------------------------------------------------------------------------------------------------------
// GraphicsConfig.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsPch.h"
#include "Graphics/GraphicsConfig.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium;

HELIUM_DEFINE_ENUMERATION( GraphicsConfig::ETextureFilter, HELIUM_GRAPHICS_API );
HELIUM_DEFINE_ENUMERATION( GraphicsConfig::EShadowMode, HELIUM_GRAPHICS_API );
HELIUM_IMPLEMENT_OBJECT( GraphicsConfig, Graphics, 0 );

/// Constructor.
GraphicsConfig::GraphicsConfig()
: m_width( DEFAULT_WIDTH )
, m_height( DEFAULT_HEIGHT )
, m_textureFiltering( ETextureFilter::TRILINEAR )
, m_maxAnisotropy( 0 )
, m_shadowMode( EShadowMode::PCF_DITHERED )
, m_shadowBufferSize( DEFAULT_SHADOW_BUFFER_SIZE )
, m_bFullscreen( false )
, m_bVsync( true )
{
}

/// Destructor.
GraphicsConfig::~GraphicsConfig()
{
}

void GraphicsConfig::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &GraphicsConfig::m_width, TXT( "m_Width" ) );
    comp.AddField( &GraphicsConfig::m_height, TXT( "m_Height" ) );
    comp.AddField( &GraphicsConfig::m_bFullscreen, TXT( "m_bFullscreen" ) );
    comp.AddField( &GraphicsConfig::m_bVsync, TXT( "m_bVsync" ) );
    comp.AddEnumerationField( &GraphicsConfig::m_textureFiltering, TXT( "m_TextureFiltering" ) );
    comp.AddField( &GraphicsConfig::m_maxAnisotropy, TXT( "m_MaxAnisotropy" ) );
    comp.AddEnumerationField( &GraphicsConfig::m_shadowMode, TXT( "m_ShadowMode" ) );
    comp.AddField( &GraphicsConfig::m_shadowBufferSize, TXT( "m_ShadowBufferSize" ) );
}



// / @copydoc GameObject::Serialize()
// void GraphicsConfig::Serialize( Serializer& s )
// {
//     HELIUM_SERIALIZE_BASE( s );
// 
//     s << HELIUM_TAGGED( m_width );
//     s << HELIUM_TAGGED( m_height );
// 
//     s << HELIUM_TAGGED( m_bFullscreen );
// 
//     s << HELIUM_TAGGED( m_bVsync );
// 
//     s << HELIUM_TAGGED( m_textureFiltering );
//     s << HELIUM_TAGGED( m_maxAnisotropy );
// 
//     s << HELIUM_TAGGED( m_shadowMode );
//     s << HELIUM_TAGGED( m_shadowBufferSize );
// }
