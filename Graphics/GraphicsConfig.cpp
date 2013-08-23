#include "GraphicsPch.h"
#include "Graphics/GraphicsConfig.h"

#include "Reflect/TranslatorDeduction.h"

HELIUM_DEFINE_ENUM( Helium::GraphicsConfig::ETextureFilter );
HELIUM_DEFINE_ENUM( Helium::GraphicsConfig::EShadowMode );
HELIUM_IMPLEMENT_ASSET( Helium::GraphicsConfig, Graphics, 0 );

using namespace Helium;

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

void GraphicsConfig::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &GraphicsConfig::m_width, TXT( "m_Width" ) );
    comp.AddField( &GraphicsConfig::m_height, TXT( "m_Height" ) );
    comp.AddField( &GraphicsConfig::m_bFullscreen, TXT( "m_bFullscreen" ) );
    comp.AddField( &GraphicsConfig::m_bVsync, TXT( "m_bVsync" ) );
    comp.AddField( &GraphicsConfig::m_textureFiltering, TXT( "m_TextureFiltering" ) );
    comp.AddField( &GraphicsConfig::m_maxAnisotropy, TXT( "m_MaxAnisotropy" ) );
    comp.AddField( &GraphicsConfig::m_shadowMode, TXT( "m_ShadowMode" ) );
    comp.AddField( &GraphicsConfig::m_shadowBufferSize, TXT( "m_ShadowBufferSize" ) );
}
