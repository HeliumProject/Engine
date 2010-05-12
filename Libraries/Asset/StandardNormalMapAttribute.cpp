#include "StandardNormalMapAttribute.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( StandardNormalMapAttribute );

void StandardNormalMapAttribute::EnumerateClass( Reflect::Compositor<StandardNormalMapAttribute>& comp )
{
  comp.GetComposite().m_UIName = "Normal Map";
  comp.GetComposite().SetProperty( AssetProperties::ShortDescription, "Normal (bump) map settings." );
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "Enhances details on a shader without requiring more polygons on the model." );
  comp.GetComposite().SetProperty( AssetProperties::SmallIcon, "attribute_normalmap_16.png" );

  Reflect::EnumerationField* enumTexFormat  = comp.AddEnumerationField( &StandardNormalMapAttribute::m_TexFormat, "m_TexFormat",        AssetFlags::RealTimeUpdateable );
  Reflect::Field* fieldNormalMapScale       = comp.AddField( &StandardNormalMapAttribute::m_NormalMapScale,       "m_NormalMapScale",   AssetFlags::RealTimeUpdateable );
  fieldNormalMapScale->SetProperty( "UIScript", "UI[.[slider{min=1.0; max=16.0} value{}].]" );
}



///////////////////////////////////////////////////////////////////////////////
// If any of the texture settings on oldAttrib differ from this class's value,
// this function returns true.
// 
bool StandardNormalMapAttribute::ShouldRebuildTexture( const TextureMapAttribute* oldAttrib ) const
{
  if ( __super::ShouldRebuildTexture( oldAttrib ) )
  {
    return true;
  }

  const StandardNormalMapAttribute* oldNormalMap = Reflect::ConstObjectCast< StandardNormalMapAttribute >( oldAttrib );
  if ( !oldNormalMap )
  {
    return true;
  }

  return
    m_TexFormat       != oldNormalMap->m_TexFormat;
}