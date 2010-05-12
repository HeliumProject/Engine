#include "CloudControlAttribute.h"

#include "LevelAsset.h"
#include "SkyAsset.h"

namespace Asset
{
  REFLECT_DEFINE_CLASS( ColorOverbright );

void ColorOverbright::EnumerateClass( Reflect::Compositor<ColorOverbright>& comp )
{
  Reflect::Field* fieldRed = comp.AddField( &ColorOverbright::m_Red, "m_Red" );
  fieldRed->SetProperty( "UIScript", "UI[.[slider{min=1.0; max=4.0} value{}].]" );

  Reflect::Field* fieldGreen = comp.AddField( &ColorOverbright::m_Green, "m_Green" );
  fieldGreen->SetProperty( "UIScript", "UI[.[slider{min=1.0; max=4.0} value{}].]" );

  Reflect::Field* fieldBlue = comp.AddField( &ColorOverbright::m_Blue, "m_Blue" );
  fieldBlue->SetProperty( "UIScript", "UI[.[slider{min=1.0; max=4.0} value{}].]" );
}


  void ColorOverbright::PreSerialize()
  {
    __super::PreSerialize();

    Math::Clamp( m_Red, 1.0f, 4.0f );
    Math::Clamp( m_Green, 1.0f, 4.0f );
    Math::Clamp( m_Blue, 1.0f, 4.0f );
  }
  

  REFLECT_DEFINE_CLASS( CloudControlAttribute );

void CloudControlAttribute::EnumerateClass( Reflect::Compositor<CloudControlAttribute>& comp )
{
  comp.GetComposite().m_UIName = "Cloud Control";

  Reflect::Field* fieldColor = comp.AddField( &CloudControlAttribute::m_Color, "m_Color" );
  Reflect::ElementField* elemOverBright = comp.AddField( &CloudControlAttribute::m_OverBright, "m_OverBright" );
  Reflect::Field* fieldStrength = comp.AddField( &CloudControlAttribute::m_Strength, "m_Strength" );
  fieldStrength->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=4.0} value{}].]" );
  Reflect::Field* fieldLayerBlend = comp.AddField( &CloudControlAttribute::m_LayerBlend, "m_LayerBlend" );
  fieldLayerBlend->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=1.0} value{}].]" );
}


  Attribute::AttributeCategoryType CloudControlAttribute::GetCategoryType() const
  {
    return Attribute::AttributeCategoryTypes::Configuration;
  }
}