#include "Precompile.h"
#include "DirectionalLight.h"

#include "Scene.h"
#include "SceneManager.h"
#include "InstanceType.h"

#include "Core/Enumerator.h"
#include "UIToolKit/ImageManager.h"

#include "PrimitivePointer.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(Luna::DirectionalLight);

void DirectionalLight::InitializeType()
{
  Reflect::RegisterClass< Luna::DirectionalLight >( "Luna::DirectionalLight" );

  Enumerator::InitializePanel( "DirectionalLight", CreatePanelSignature::Delegate( &DirectionalLight::CreatePanel ) );
}

void DirectionalLight::CleanupType()
{
  Reflect::UnregisterClass< Luna::DirectionalLight >();
}

DirectionalLight::DirectionalLight(Luna::Scene* scene)
: Luna::Light (scene, new Content::DirectionalLight() )
{

}

DirectionalLight::DirectionalLight(Luna::Scene* scene, Content::DirectionalLight* light)
: Luna::Light ( scene, light )
{

}

i32 DirectionalLight::GetImageIndex() const
{
  return UIToolKit::GlobalImageManager().GetImageIndex( "light_16.png" );
}

std::string DirectionalLight::GetApplicationTypeName() const
{
  return "DirectionalLight";
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by DirectionalLight.
//
bool DirectionalLight::ValidatePanel(const std::string& name)
{
  if (name == "DirectionalLight")
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

void DirectionalLight::CreatePanel( CreatePanelArgs& args )
{
  args.m_Enumerator->PushPanel("Directional Light", true);
  {
    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Global Sun");
      args.m_Enumerator->AddCheckBox<DirectionalLight, bool>( args.m_Selection, &DirectionalLight::GetGlobalSun, &DirectionalLight::SetGlobalSun );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Shadow Softness");

      args.m_Enumerator->AddValue<DirectionalLight, float>( args.m_Selection, &DirectionalLight::GetShadowSoftness, &DirectionalLight::SetShadowSoftness );
      Inspect::Slider* slider = args.m_Enumerator->AddSlider<DirectionalLight, float>( args.m_Selection, &DirectionalLight::GetShadowSoftness, &DirectionalLight::SetShadowSoftness );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Soft Shadow Samples");

      args.m_Enumerator->AddValue<DirectionalLight, int>( args.m_Selection, &DirectionalLight::GetSoftShadowSamples, &DirectionalLight::SetSoftShadowSamples );
      Inspect::Slider* slider = args.m_Enumerator->AddSlider<DirectionalLight, int>( args.m_Selection, &DirectionalLight::GetSoftShadowSamples, &DirectionalLight::SetSoftShadowSamples );
    }
    args.m_Enumerator->Pop();
  }
  args.m_Enumerator->Pop();

}

bool DirectionalLight::GetGlobalSun() const
{
  return GetPackage< Content::DirectionalLight >()->m_GlobalSun;
}

void DirectionalLight::SetGlobalSun( bool globalSun )
{
  GetPackage< Content::DirectionalLight >()->m_GlobalSun = globalSun;
}

float DirectionalLight::GetShadowSoftness() const
{
  return GetPackage< Content::DirectionalLight >()->m_ShadowSoftness;
}

void DirectionalLight::SetShadowSoftness( float multiplier )
{
  GetPackage< Content::DirectionalLight >()->m_ShadowSoftness = multiplier;
}

int DirectionalLight::GetSoftShadowSamples() const
{
  return GetPackage< Content::DirectionalLight >()->m_SoftShadowSamples;
}

void DirectionalLight::SetSoftShadowSamples( int multiplier )
{
  GetPackage< Content::DirectionalLight >()->m_SoftShadowSamples = multiplier;
}
