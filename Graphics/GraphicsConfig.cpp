//----------------------------------------------------------------------------------------------------------------------
// GraphicsConfig.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsPch.h"
#include "Graphics/GraphicsConfig.h"

using namespace Lunar;

L_IMPLEMENT_OBJECT( GraphicsConfig, Graphics, 0 );

/// Constructor.
GraphicsConfig::GraphicsConfig()
: m_width( DEFAULT_WIDTH )
, m_height( DEFAULT_HEIGHT )
, m_textureFiltering( TEXTURE_FILTER_TRILINEAR )
, m_maxAnisotropy( 0 )
, m_shadowMode( SHADOW_MODE_PCF_DITHERED )
, m_shadowBufferSize( DEFAULT_SHADOW_BUFFER_SIZE )
, m_bFullscreen( false )
, m_bVsync( true )
{
}

/// Destructor.
GraphicsConfig::~GraphicsConfig()
{
}

/// @copydoc GameObject::Serialize()
void GraphicsConfig::Serialize( Serializer& s )
{
    L_SERIALIZE_SUPER( s );

    s << L_TAGGED( m_width );
    s << L_TAGGED( m_height );

    s << L_TAGGED( m_bFullscreen );

    s << L_TAGGED( m_bVsync );

    s << L_TAGGED( m_textureFiltering );
    s << L_TAGGED( m_maxAnisotropy );

    s << L_TAGGED( m_shadowMode );
    s << L_TAGGED( m_shadowBufferSize );
}
