/*#include "Precompile.h"*/
#include "AmbientLight.h"

#include "Core/Scene/Scene.h"
#include "Core/Scene/SceneManager.h"
#include "Core/Scene/InstanceType.h"
#include "Core/Scene/PropertiesGenerator.h"
#include "Core/Scene/PrimitivePointer.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Core;

// RTTI
SCENE_DEFINE_TYPE(Core::AmbientLight);

void AmbientLight::InitializeType()
{
    Reflect::RegisterClassType< Core::AmbientLight >( TXT( "Core::AmbientLight" ) );

    PropertiesGenerator::InitializePanel( TXT( "AmbientLight" ), CreatePanelSignature::Delegate( &AmbientLight::CreatePanel ) );
}

void AmbientLight::CleanupType()
{
    Reflect::UnregisterClassType< Core::AmbientLight >();
}

AmbientLight::AmbientLight(Core::Scene* scene)
: Core::Light (scene, new Content::AmbientLight() )
{

}

AmbientLight::AmbientLight(Core::Scene* scene, Content::AmbientLight* light)
: Core::Light ( scene, light )
{

}

i32 AmbientLight::GetImageIndex() const
{
    return -1; // Helium::GlobalFileIconsTable().GetIconID( TXT( "light" ) );
}

tstring AmbientLight::GetApplicationTypeName() const
{
    return TXT( "AmbientLight" );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Core::Light.
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
    args.m_Generator->PushContainer( TXT( "Ambient Light" ) );
    {
        Inspect::Label* label;
        args.m_Generator->PushContainer();
        {
            label = args.m_Generator->AddLabel( TXT( "Do Ambient Occlusion" ) );
            label->a_HelpText.Set( TXT( "Toggles ambient occlusion with regard to this light." ) );
            Inspect::CheckBox* checkBox = args.m_Generator->AddCheckBox<Core::AmbientLight, bool>( args.m_Selection, &AmbientLight::GetDoOcclusion, &AmbientLight::SetDoOcclusion );
            checkBox->a_HelpText.Set( TXT( "Toggles ambient occlusion with regard to this light." ) );
        }
        args.m_Generator->Pop();

        args.m_Generator->PushContainer();
        {
            label = args.m_Generator->AddLabel( TXT( "Sample Environment" ) );
            label->a_HelpText.Set( TXT( "Sample the environment to determine the color of this light (FIXME: Is this true?)" ) );
            Inspect::CheckBox* checkBox = args.m_Generator->AddCheckBox<Core::AmbientLight, bool>( args.m_Selection, &AmbientLight::GetSampleEnvironment, &AmbientLight::SetSampleEnvironment );
            checkBox->a_HelpText.Set( TXT( "Sample the environment to determine the color of this light (FIXME: Is this true?)" ) );
        }
        args.m_Generator->Pop();

        args.m_Generator->PushContainer();
        {
            label = args.m_Generator->AddLabel( TXT( "Dark Color" ) );
            label->a_HelpText.Set( TXT( "Sets the 'dark color' for this light, which affects the color of occluded areas." ) );
            Inspect::ColorPicker* colorPicker = args.m_Generator->AddColorPicker<Core::AmbientLight, Color3>( args.m_Selection, &AmbientLight::GetOcclusionColor, &AmbientLight::SetOcclusionColor );
            colorPicker->a_HelpText.Set( TXT( "Sets the 'dark color' for this light, which affects the color of occluded areas." ) );

            Inspect::Slider* slider = args.m_Generator->AddSlider<Core::AmbientLight, float>( args.m_Selection, &AmbientLight::GetOcclusionIntensity, &AmbientLight::SetOcclusionIntensity );
            slider->a_Min.Set( 0.0f );
            slider->a_HelpText.Set( TXT( "Sets the intensity of the occlusion." ) );

            Inspect::Value* value = args.m_Generator->AddValue<Core::AmbientLight, float>( args.m_Selection, &AmbientLight::GetOcclusionIntensity, &AmbientLight::SetOcclusionIntensity );
            value->a_HelpText.Set( TXT( "Sets the intensity of the occlusion." ) );
        }
        args.m_Generator->Pop();

        args.m_Generator->PushContainer();
        {
            label = args.m_Generator->AddLabel( TXT( "Samples" ) );
            label->a_HelpText.Set( TXT( "Sets the number of samples to use for this light." ) );
            Inspect::Value* value = args.m_Generator->AddValue<Core::AmbientLight, u32>( args.m_Selection, &AmbientLight::GetSamples, &AmbientLight::SetSamples );
            value->a_HelpText.Set( TXT( "Sets the number of samples to use for this light." ) );
        }
        args.m_Generator->Pop();

        args.m_Generator->PushContainer();
        {
            label = args.m_Generator->AddLabel( TXT( "Sample Distance" ) );
            label->a_HelpText.Set( TXT( "Sets the distance between samples for this light." ) );
            Inspect::Value* value = args.m_Generator->AddValue<Core::AmbientLight, float>( args.m_Selection, &AmbientLight::GetSampleDistance, &AmbientLight::SetSampleDistance );
            value->a_HelpText.Set( TXT( "Sets the distance between samples for this light." ) );
        }
        args.m_Generator->Pop();

    }
    args.m_Generator->Pop();
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