#include "StandardDetailMapComponent.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( StandardDetailMapComponent );

void StandardDetailMapComponent::EnumerateClass( Reflect::Compositor<StandardDetailMapComponent>& comp )
{
  comp.GetComposite().m_UIName = TXT( "Detail Map" );
  comp.GetComposite().SetProperty( AssetProperties::ShortDescription, "Provides finer details on a shader." );
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "Specifies a texture to show when very close to a particular shader." );
  comp.GetComposite().SetProperty( AssetProperties::SmallIcon, "attribute_detailmap.png" );

  Reflect::EnumerationField* enumTexFormat = comp.AddEnumerationField( &StandardDetailMapComponent::m_TexFormat, "m_TexFormat", AssetFlags::RealTimeUpdateable );

  Reflect::Field* fieldResolutionScale = comp.AddField( &StandardDetailMapComponent::m_ResolutionScale, "m_ResolutionScale", AssetFlags::RealTimeUpdateable );
  fieldResolutionScale->SetProperty( TXT( "UIScript" ), TXT( "UI[.[slider{min=1.0; max=16.0} value{}].]" ) );

  Reflect::Field* fieldDetailStrength = comp.AddField( &StandardDetailMapComponent::m_DetailStrength, "m_DetailStrength", AssetFlags::RealTimeUpdateable );
  fieldDetailStrength->SetProperty( TXT( "UIScript" ), TXT( "UI[.[slider{min=0.0; max=2.0} value{}].]" ) );
}



///////////////////////////////////////////////////////////////////////////////
// If any of the texture settings on oldAttrib differ from this class's value,
// this function returns true.
// 
bool StandardDetailMapComponent::ShouldRebuildTexture( const TextureMapComponent* oldAttrib ) const
{
  if ( __super::ShouldRebuildTexture( oldAttrib ) )
  {
    return true;
  }

  const StandardDetailMapComponent* oldDetailMap = Reflect::ConstObjectCast< StandardDetailMapComponent >( oldAttrib );
  if ( !oldDetailMap )
  {
    return true;
  }

  return m_TexFormat != oldDetailMap->m_TexFormat;
}