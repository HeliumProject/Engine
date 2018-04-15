#include "Precompile.h"
#include "Graphics/GraphicsConfig.h"

#include "Reflect/TranslatorDeduction.h"

HELIUM_DEFINE_ENUM( Helium::GraphicsConfig::ETextureFilter );
HELIUM_DEFINE_ENUM( Helium::GraphicsConfig::EShadowMode );
HELIUM_DEFINE_CLASS( Helium::GraphicsConfig );

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
    comp.AddField( &GraphicsConfig::m_width, "m_Width" );
    comp.AddField( &GraphicsConfig::m_height, "m_Height" );
    comp.AddField( &GraphicsConfig::m_bFullscreen, "m_bFullscreen" );
    comp.AddField( &GraphicsConfig::m_bVsync, "m_bVsync" );
    comp.AddField( &GraphicsConfig::m_textureFiltering, "m_TextureFiltering" );
    comp.AddField( &GraphicsConfig::m_maxAnisotropy, "m_MaxAnisotropy" );
    comp.AddField( &GraphicsConfig::m_shadowMode, "m_ShadowMode" );
    comp.AddField( &GraphicsConfig::m_shadowBufferSize, "m_ShadowBufferSize" );
}
