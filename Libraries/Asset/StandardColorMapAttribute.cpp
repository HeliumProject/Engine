#include "StandardColorMapAttribute.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( StandardColorMapAttribute );

void StandardColorMapAttribute::EnumerateClass( Reflect::Compositor<StandardColorMapAttribute>& comp )
{
  comp.GetComposite().m_UIName = "Color Map";
  comp.GetComposite().SetProperty( AssetProperties::ShortDescription, "Also known as a Base Map." );
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "Specifies the base map texture for a shader.  All shaders are required to have some sort of color map texture." );
  comp.GetComposite().SetProperty( AssetProperties::SmallIcon, "attribute_colormap_16.png" );

  Reflect::EnumerationField* enumTexFormat = comp.AddEnumerationField( &StandardColorMapAttribute::m_TexFormat, "m_TexFormat", AssetFlags::RealTimeUpdateable );
  Reflect::Field* fieldAmbOccScale = comp.AddField( &StandardColorMapAttribute::m_AmbOccScale, "m_AmbOccScale", AssetFlags::RealTimeUpdateable );
  Reflect::Field* fieldBaseMapTint = comp.AddField( &StandardColorMapAttribute::m_BaseMapTint, "m_BaseMapTint", AssetFlags::RealTimeUpdateable );

  Reflect::Field* fieldDisableEnvTint = comp.AddField( &StandardColorMapAttribute::m_DisableBaseTint, "m_DisableBaseTint", AssetFlags::RealTimeUpdateable );

  // Legacy
  Reflect::EnumerationField* enumAlphaMode = comp.AddEnumerationField( &StandardColorMapAttribute::m_AlphaMode, "m_AlphaMode", Reflect::FieldFlags::Hide );
}


#pragma TODO( "Remove legacy shader support (m_AlphaMode should not be a member of StandardColorMapAttribute)" )


///////////////////////////////////////////////////////////////////////////////
// If any of the texture settings on oldAttrib differ from this class's value,
// this function returns true.
// 
bool StandardColorMapAttribute::ShouldRebuildTexture( const TextureMapAttribute* oldAttrib ) const
{
  if ( __super::ShouldRebuildTexture( oldAttrib ) )
  {
    return true;
  }

  const StandardColorMapAttribute* oldColorMap = Reflect::ConstObjectCast< StandardColorMapAttribute >( oldAttrib );
  if ( oldColorMap == NULL )
  {
    return true;
  }

  return 
    m_TexFormat != oldColorMap->m_TexFormat ||
    m_AmbOccScale != oldColorMap->m_AmbOccScale;
}