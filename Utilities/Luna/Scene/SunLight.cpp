#include "Precompile.h"
#include "SunLight.h"

#include "InstanceType.h"
#include "PrimitiveSphere.h"
#include "PrimitivePointer.h"
#include "Scene.h"
#include "SceneManager.h"

#include "Core/Enumerator.h"
#include "UIToolKit/ImageManager.h"


// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(SunLight);

void SunLight::InitializeType()
{
  Reflect::RegisterClass< SunLight >( "Luna::SunLight" );

  Enumerator::InitializePanel( "SunLight", CreatePanelSignature::Delegate( &SunLight::CreatePanel ) );
}

void SunLight::CleanupType()
{
  Reflect::UnregisterClass< SunLight >();
}

SunLight::SunLight(Luna::Scene* scene)
: Luna::DirectionalLight (scene, new Content::SunLight() )
{

}

SunLight::SunLight(Luna::Scene* scene, Content::SunLight* light)
: Luna::DirectionalLight ( scene, light )
{

}

i32 SunLight::GetImageIndex() const
{
  return UIToolKit::GlobalImageManager().GetImageIndex( "light_16.png" );
}

std::string SunLight::GetApplicationTypeName() const
{
  return "SunLight";
}


///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by SunLight.
// 
bool SunLight::ValidatePanel(const std::string& name)
{
  if (name == "SunLight")
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

void SunLight::CreatePanel( CreatePanelArgs& args )
{
  
  args.m_Enumerator->PushPanel("Sun Light", true);
  {
    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Sun Intensity");

      args.m_Enumerator->AddValue<SunLight, float>( args.m_Selection, &SunLight::GetSunIntensity, &SunLight::SetSunIntensity );
      Inspect::Slider* slider = args.m_Enumerator->AddSlider<SunLight, float>( args.m_Selection, &SunLight::GetSunIntensity, &SunLight::SetSunIntensity );
      slider->SetRangeMin( 0.0f );
      slider->SetRangeMax( 10.0f );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Sky Intensity");

      args.m_Enumerator->AddValue<SunLight, float>( args.m_Selection, &SunLight::GetSkyIntensity, &SunLight::SetSkyIntensity );
      Inspect::Slider* slider = args.m_Enumerator->AddSlider<SunLight, float>( args.m_Selection, &SunLight::GetSkyIntensity, &SunLight::SetSkyIntensity );
      slider->SetRangeMin( 0.0f );
      slider->SetRangeMax( 10.0f );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Haze");

      args.m_Enumerator->AddValue<SunLight, float>( args.m_Selection, &SunLight::GetHaze, &SunLight::SetHaze );
      Inspect::Slider* slider = args.m_Enumerator->AddSlider<SunLight, float>( args.m_Selection, &SunLight::GetHaze, &SunLight::SetHaze );
      slider->SetRangeMin( Content::SunLight::MinHaze );
      slider->SetRangeMax( Content::SunLight::MaxHaze );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Red/Blue Shift");

      args.m_Enumerator->AddValue<SunLight, float>( args.m_Selection, &SunLight::GetRedBlueShift, &SunLight::SetRedBlueShift );
      Inspect::Slider* slider = args.m_Enumerator->AddSlider<SunLight, float>( args.m_Selection, &SunLight::GetRedBlueShift, &SunLight::SetRedBlueShift );
      slider->SetRangeMin( Content::SunLight::MinRedBlueShift );
      slider->SetRangeMax( Content::SunLight::MaxRedBlueShift );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Saturation");

      args.m_Enumerator->AddValue<SunLight, float>( args.m_Selection, &SunLight::GetSaturation, &SunLight::SetSaturation );
      Inspect::Slider* slider = args.m_Enumerator->AddSlider<SunLight, float>( args.m_Selection, &SunLight::GetSaturation, &SunLight::SetSaturation );
      slider->SetRangeMin( Content::SunLight::MinSaturation );
      slider->SetRangeMax( Content::SunLight::MaxSaturation );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Horizon Height");

      args.m_Enumerator->AddValue<SunLight, float>( args.m_Selection, &SunLight::GetHorizonHeight, &SunLight::SetHorizonHeight );
      Inspect::Slider* slider = args.m_Enumerator->AddSlider<SunLight, float>( args.m_Selection, &SunLight::GetHorizonHeight, &SunLight::SetHorizonHeight );
      slider->SetRangeMin( Content::SunLight::MinHorizonHeight );
      slider->SetRangeMax( Content::SunLight::MaxHorizonHeight );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Horizon Blur");

      args.m_Enumerator->AddValue<SunLight, float>( args.m_Selection, &SunLight::GetHorizonBlur, &SunLight::SetHorizonBlur );
      Inspect::Slider* slider = args.m_Enumerator->AddSlider<SunLight, float>( args.m_Selection, &SunLight::GetHorizonBlur, &SunLight::SetHorizonBlur );
      slider->SetRangeMin( Content::SunLight::MinHorizonBlur );
      slider->SetRangeMax( Content::SunLight::MaxHorizonBlur );
    }
    args.m_Enumerator->Pop();

 
    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Ground Color");
      args.m_Enumerator->AddColorPicker<SunLight, Color3>( args.m_Selection, &SunLight::GetGroundColor, &SunLight::SetGroundColor );
    }
    args.m_Enumerator->Pop();

    

  }
  args.m_Enumerator->Pop();
  
}

float SunLight::GetRedBlueShift() const
{
  return GetPackage< Content::SunLight >()->m_RedBlueShift;
}

void SunLight::SetRedBlueShift( float redBlueShift )
{
  GetPackage< Content::SunLight >()->m_RedBlueShift = redBlueShift; 
}

float SunLight::GetSaturation() const
{
  return GetPackage< Content::SunLight >()->m_Saturation;
}

void SunLight::SetSaturation( float saturation )
{
  GetPackage< Content::SunLight >()->m_Saturation = saturation; 
}

float SunLight::GetHorizonHeight() const
{
  return GetPackage< Content::SunLight >()->m_HorizonHeight;
}

void SunLight::SetHorizonHeight( float horizonHeight )
{
  GetPackage< Content::SunLight >()->m_HorizonHeight = horizonHeight;  
}

float SunLight::GetHorizonBlur() const
{
  return GetPackage< Content::SunLight >()->m_HorizonBlur;
}

void SunLight::SetHorizonBlur( float horizonBlur )
{
  GetPackage< Content::SunLight >()->m_HorizonBlur = horizonBlur;
}

Color3 SunLight::GetGroundColor() const
{
  return GetPackage< Content::SunLight >()->m_GroundColor;
}

void SunLight::SetGroundColor( Color3 color )
{
  GetPackage< Content::SunLight >()->m_GroundColor = color;  
}


float SunLight::GetSunIntensity() const
{
  return GetPackage< Content::SunLight >()->m_SunIntensity;
}

void SunLight::SetSunIntensity( float multiplier )
{
  GetPackage< Content::SunLight >()->m_SunIntensity = multiplier;
}

float SunLight::GetSkyIntensity() const
{
  return GetPackage< Content::SunLight >()->m_SkyIntensity;
}

void SunLight::SetSkyIntensity( float multiplier )
{
  GetPackage< Content::SunLight >()->m_SkyIntensity = multiplier;
}

float SunLight::GetHaze() const
{
  return GetPackage< Content::SunLight >()->m_Haze;
}

void SunLight::SetHaze( float haze )
{
  GetPackage< Content::SunLight >()->m_Haze = haze;
}


