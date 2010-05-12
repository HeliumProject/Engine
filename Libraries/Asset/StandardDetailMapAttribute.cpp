#include "StandardDetailMapAttribute.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( StandardDetailMapAttribute );

void StandardDetailMapAttribute::EnumerateClass( Reflect::Compositor<StandardDetailMapAttribute>& comp )
{
  comp.GetComposite().m_UIName = "Detail Map";
  comp.GetComposite().SetProperty( AssetProperties::ShortDescription, "Provides finer details on a shader." );
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "Specifies a texture to show when very close to a particular shader." );
  comp.GetComposite().SetProperty( AssetProperties::SmallIcon, "attribute_detailmap_16.png" );

  Reflect::EnumerationField* enumTexFormat = comp.AddEnumerationField( &StandardDetailMapAttribute::m_TexFormat, "m_TexFormat", AssetFlags::RealTimeUpdateable );

  Reflect::Field* fieldResolutionScale = comp.AddField( &StandardDetailMapAttribute::m_ResolutionScale, "m_ResolutionScale", AssetFlags::RealTimeUpdateable );
  fieldResolutionScale->SetProperty( "UIScript", "UI[.[slider{min=1.0; max=16.0} value{}].]" );

  Reflect::Field* fieldDetailStrength = comp.AddField( &StandardDetailMapAttribute::m_DetailStrength, "m_DetailStrength", AssetFlags::RealTimeUpdateable );
  fieldDetailStrength->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=2.0} value{}].]" );
}



///////////////////////////////////////////////////////////////////////////////
// If any of the texture settings on oldAttrib differ from this class's value,
// this function returns true.
// 
bool StandardDetailMapAttribute::ShouldRebuildTexture( const TextureMapAttribute* oldAttrib ) const
{
  if ( __super::ShouldRebuildTexture( oldAttrib ) )
  {
    return true;
  }

  const StandardDetailMapAttribute* oldDetailMap = Reflect::ConstObjectCast< StandardDetailMapAttribute >( oldAttrib );
  if ( !oldDetailMap )
  {
    return true;
  }

  return m_TexFormat != oldDetailMap->m_TexFormat;
}