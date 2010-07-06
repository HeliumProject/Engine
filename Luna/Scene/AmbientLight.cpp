#include "Precompile.h"
#include "AmbientLight.h"

#include "Scene.h"
#include "SceneManager.h"
#include "InstanceType.h"

#include "Core/Enumerator.h"
#include "Application/UI/ArtProvider.h"

#include "PrimitivePointer.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(Luna::AmbientLight);

void AmbientLight::InitializeType()
{
  Reflect::RegisterClass< Luna::AmbientLight >( TXT( "Luna::AmbientLight" ) );

  Enumerator::InitializePanel( TXT( "AmbientLight" ), CreatePanelSignature::Delegate( &AmbientLight::CreatePanel ) );
}

void AmbientLight::CleanupType()
{
  Reflect::UnregisterClass< Luna::AmbientLight >();
}

AmbientLight::AmbientLight(Luna::Scene* scene)
: Luna::Light (scene, new Content::AmbientLight() )
{

}

AmbientLight::AmbientLight(Luna::Scene* scene, Content::AmbientLight* light)
: Luna::Light ( scene, light )
{

}

i32 AmbientLight::GetImageIndex() const
{
  return Nocturnal::GlobalFileIconsTable().GetIconID( TXT( "light" ) );
}

tstring AmbientLight::GetApplicationTypeName() const
{
  return TXT( "AmbientLight" );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Luna::Light.
//
bool AmbientLight::ValidatePanel(const tstring& name)
{
  if (name == TXT( "AmbientLight" ) )
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

void AmbientLight::CreatePanel( CreatePanelArgs& args )
{
  args.m_Enumerator->PushPanel( TXT( "Ambient Light" ), true);
  {
    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel( TXT( "Do Ambient Occlusion" ) );
      args.m_Enumerator->AddCheckBox<Luna::AmbientLight, bool>( args.m_Selection, &AmbientLight::GetDoOcclusion, &AmbientLight::SetDoOcclusion );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel( TXT( "Sample Environment" ) );
      args.m_Enumerator->AddCheckBox<Luna::AmbientLight, bool>( args.m_Selection, &AmbientLight::GetSampleEnvironment, &AmbientLight::SetSampleEnvironment );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel( TXT( "Dark Color" ) );
      args.m_Enumerator->AddColorPicker<Luna::AmbientLight, Color3>( args.m_Selection, &AmbientLight::GetOcclusionColor, &AmbientLight::SetOcclusionColor );

      Inspect::Slider* slider = args.m_Enumerator->AddSlider<Luna::AmbientLight, float>( args.m_Selection, &AmbientLight::GetOcclusionIntensity, &AmbientLight::SetOcclusionIntensity );
      slider->SetRangeMin( 0.0f );

      args.m_Enumerator->AddValue<Luna::AmbientLight, float>( args.m_Selection, &AmbientLight::GetOcclusionIntensity, &AmbientLight::SetOcclusionIntensity );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel( TXT( "Samples" ) );
      args.m_Enumerator->AddValue<Luna::AmbientLight, u32>( args.m_Selection, &AmbientLight::GetSamples, &AmbientLight::SetSamples );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel( TXT( "Sample Distance" ) );
      args.m_Enumerator->AddValue<Luna::AmbientLight, float>( args.m_Selection, &AmbientLight::GetSampleDistance, &AmbientLight::SetSampleDistance );
    }
    args.m_Enumerator->Pop();

  }
  args.m_Enumerator->Pop();
}

Color3 AmbientLight::GetOcclusionColor() const
{
  return GetPackage< Content::AmbientLight >()->m_OcclusionColor;
}

void AmbientLight::SetOcclusionColor( Color3 color )
{
  GetPackage< Content::AmbientLight >()->m_OcclusionColor = color;

  m_Changed.Raise( LightChangeArgs( this ) );
}

float AmbientLight::GetOcclusionIntensity() const
{
  return GetPackage< Content::AmbientLight >()->m_OcclusionColor.s;
}

void AmbientLight::SetOcclusionIntensity( float intensity )
{
  GetPackage< Content::AmbientLight >()->m_OcclusionColor.s = intensity;
}

f32 AmbientLight::GetSampleDistance() const
{
  return GetPackage< Content::AmbientLight >()->m_SampleDistance;   
}

void AmbientLight::SetSampleDistance( f32 distance )
{
  GetPackage< Content::AmbientLight >()->m_SampleDistance = distance;
}

u32 AmbientLight::GetSamples() const
{
  return GetPackage< Content::AmbientLight >()->m_Samples;   
}

void AmbientLight::SetSamples( u32 samples )
{
  GetPackage< Content::AmbientLight >()->m_Samples = samples;
}

bool   AmbientLight::GetDoOcclusion() const
{
  return GetPackage< Content::AmbientLight >()->m_DoAmbientOcclusion;   

}
void   AmbientLight::SetDoOcclusion( bool occlusion )
{
  GetPackage< Content::AmbientLight >()->m_DoAmbientOcclusion = occlusion;
}
f32    AmbientLight::GetSpread() const
{
  return GetPackage< Content::AmbientLight >()->m_Spread;
}
void   AmbientLight::SetSpread( float spread )
{
  GetPackage< Content::AmbientLight >()->m_Spread = spread;
}
bool   AmbientLight::GetSampleEnvironment() const
{
  return GetPackage< Content::AmbientLight >()->m_SampleEnvironment;
}
void   AmbientLight::SetSampleEnvironment( bool sample )
{
  GetPackage< Content::AmbientLight >()->m_SampleEnvironment = sample;
}