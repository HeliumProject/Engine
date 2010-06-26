#include "StandardNormalMapComponent.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( StandardNormalMapComponent );

void StandardNormalMapComponent::EnumerateClass( Reflect::Compositor<StandardNormalMapComponent>& comp )
{
  comp.GetComposite().m_UIName = TXT( "Normal Map" );
  comp.GetComposite().SetProperty( AssetProperties::ShortDescription, TXT( "Normal (bump) map settings." ) );
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, TXT( "Enhances details on a shader without requiring more polygons on the model." ) );
  comp.GetComposite().SetProperty( AssetProperties::SmallIcon, TXT( "attribute_normalmap_16.png" ) );

  Reflect::EnumerationField* enumTexFormat  = comp.AddEnumerationField( &StandardNormalMapComponent::m_TexFormat, "m_TexFormat",        AssetFlags::RealTimeUpdateable );
  Reflect::Field* fieldNormalMapScale       = comp.AddField( &StandardNormalMapComponent::m_NormalMapScale,       "m_NormalMapScale",   AssetFlags::RealTimeUpdateable );
  fieldNormalMapScale->SetProperty( TXT( "UIScript" ), TXT( "UI[.[slider{min=1.0; max=16.0} value{}].]" ) );
}



///////////////////////////////////////////////////////////////////////////////
// If any of the texture settings on oldAttrib differ from this class's value,
// this function returns true.
// 
bool StandardNormalMapComponent::ShouldRebuildTexture( const TextureMapComponent* oldAttrib ) const
{
  if ( __super::ShouldRebuildTexture( oldAttrib ) )
  {
    return true;
  }

  const StandardNormalMapComponent* oldNormalMap = Reflect::ConstObjectCast< StandardNormalMapComponent >( oldAttrib );
  if ( !oldNormalMap )
  {
    return true;
  }

  return
    m_TexFormat       != oldNormalMap->m_TexFormat;
}