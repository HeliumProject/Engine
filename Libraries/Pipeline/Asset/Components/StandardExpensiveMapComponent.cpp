#include "StandardExpensiveMapComponent.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( StandardExpensiveMapComponent );

void StandardExpensiveMapComponent::EnumerateClass( Reflect::Compositor<StandardExpensiveMapComponent>& comp )
{
  comp.GetComposite().m_UIName = "Expensive Map";
  comp.GetComposite().SetProperty( AssetProperties::ShortDescription, "Settings for Gloss, Parallax, and Incandescent Maps." );
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "As the name indicates, this map contains more data and is more expensive in terms of calculations and memory usage." );
  comp.GetComposite().SetProperty( AssetProperties::SmallIcon, "attribute_expensivemap_16.png" );

  Reflect::EnumerationField* enumTexFormat = comp.AddEnumerationField( &StandardExpensiveMapComponent::m_TexFormat, "m_TexFormat", AssetFlags::RealTimeUpdateable );

  Reflect::Field* fieldGlossMapEnabled = comp.AddField( &StandardExpensiveMapComponent::m_GlossMapEnabled, "m_GlossMapEnabled", AssetFlags::RealTimeUpdateable );

  Reflect::Field* fieldGlossScale = comp.AddField( &StandardExpensiveMapComponent::m_GlossScale, "m_GlossScale", AssetFlags::RealTimeUpdateable );
  fieldGlossScale->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=8.0} value{}].]" );

  Reflect::Field* fieldGlossDirty = comp.AddField( &StandardExpensiveMapComponent::m_GlossDirty, "m_GlossDirty", AssetFlags::RealTimeUpdateable );
  fieldGlossDirty->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=1.0} value{}].]" );

  Reflect::Field* fieldGlossTint = comp.AddField( &StandardExpensiveMapComponent::m_GlossTint, "m_GlossTint", AssetFlags::RealTimeUpdateable );

  Reflect::EnumerationField* enumCubeSpecType = comp.AddEnumerationField( &StandardExpensiveMapComponent::m_CubeSpecType, "m_CubeSpecType", AssetFlags::RealTimeUpdateable );

  Reflect::Field* fieldCubeBias = comp.AddField( &StandardExpensiveMapComponent::m_CubeBias, "m_CubeBias", AssetFlags::RealTimeUpdateable );
  fieldCubeBias->SetProperty( "UIScript", "UI[.[slider{min=-4.0; max=0.0} value{}].]" );

  Reflect::Field* fieldRealTimeSpecPower = comp.AddField( &StandardExpensiveMapComponent::m_RealTimeSpecPower, "m_RealTimeSpecPower", AssetFlags::RealTimeUpdateable );
  fieldRealTimeSpecPower->SetProperty( "UIScript", "UI[.[slider{min=1.0; max=127.0} value{}].]" );

  Reflect::Field* fieldRealTimeSpecIntensity = comp.AddField( &StandardExpensiveMapComponent::m_RealTimeSpecIntensity, "m_RealTimeSpecIntensity", AssetFlags::RealTimeUpdateable );
  fieldRealTimeSpecIntensity->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=4.0} value{}].]" );

  Reflect::Field* fieldRimStrength = comp.AddField( &StandardExpensiveMapComponent::m_RimStrength, "m_RimStrength", AssetFlags::RealTimeUpdateable );
  fieldRimStrength->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=64.0} value{}].]" );

  Reflect::Field* fieldRimTightness = comp.AddField( &StandardExpensiveMapComponent::m_RimTightness, "m_RimTightness", AssetFlags::RealTimeUpdateable );
  fieldRimTightness->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=16.0} value{}].]" );

  Reflect::Field* fieldParallaxMapEnabled = comp.AddField( &StandardExpensiveMapComponent::m_ParallaxMapEnabled, "m_ParallaxMapEnabled", AssetFlags::RealTimeUpdateable );

  Reflect::Field* fieldParallaxScale = comp.AddField( &StandardExpensiveMapComponent::m_ParallaxScale, "m_ParallaxScale", AssetFlags::RealTimeUpdateable );
  fieldParallaxScale->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=0.15} value{}].]" );

  Reflect::Field* fieldParallaxBias = comp.AddField( &StandardExpensiveMapComponent::m_ParallaxBias, "m_ParallaxBias", AssetFlags::RealTimeUpdateable );
  fieldParallaxBias->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=1.0} value{}].]" );

  Reflect::Field* fieldIncanMapEnabled = comp.AddField( &StandardExpensiveMapComponent::m_IncanMapEnabled, "m_IncanMapEnabled", AssetFlags::RealTimeUpdateable );

  Reflect::Field* fieldDetailMapMaskEnabled = comp.AddField( &StandardExpensiveMapComponent::m_DetailMapMaskEnabled, "m_DetailMapMaskEnabled", AssetFlags::RealTimeUpdateable );
}


bool StandardExpensiveMapComponent::ProcessComponent(Reflect::ElementPtr element, const std::string& fieldName)
{
  if ( fieldName == "m_CubeTint" )
  {
    Math::HDRColor3 oldCubeTint;
    Reflect::Serializer::GetValue( Reflect::AssertCast<Reflect::Serializer>(element), oldCubeTint );
    m_GlossTint.r = oldCubeTint.r;
    m_GlossTint.g = oldCubeTint.g;
    m_GlossTint.b = oldCubeTint.b;
    m_GlossScale = oldCubeTint.s;
    return true;
  }

  if ( fieldName == "m_CubeDirty" )
  {
    Reflect::Serializer::GetValue( Reflect::AssertCast<Reflect::Serializer>(element), m_GlossDirty );
    return true;
  }

  return __super::ProcessComponent( element, fieldName );
}

///////////////////////////////////////////////////////////////////////////////
// If any of the texture settings on oldAttrib differ from this class's value,
// this function returns true.
//
bool StandardExpensiveMapComponent::ShouldRebuildTexture( const TextureMapComponent* oldAttrib ) const
{
  if ( __super::ShouldRebuildTexture( oldAttrib ) )
  {
    return true;
  }

  const StandardExpensiveMapComponent* oldExpensiveMap = Reflect::ConstObjectCast< StandardExpensiveMapComponent >( oldAttrib );
  if ( !oldExpensiveMap )
  {
    return true;
  }

  return (m_TexFormat  != oldExpensiveMap->m_TexFormat);
}