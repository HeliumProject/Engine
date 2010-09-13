/*#include "Precompile.h"*/
#include "DirectionalLight.h"

#include "Core/Scene/Scene.h"
#include "Core/Scene/SceneManager.h"
#include "Core/Scene/InstanceType.h"
#include "Core/Scene/PropertiesGenerator.h"
#include "Core/Scene/PrimitivePointer.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Core;

// RTTI
SCENE_DEFINE_TYPE(Core::DirectionalLight);

void DirectionalLight::InitializeType()
{
    Reflect::RegisterClassType< Core::DirectionalLight >( TXT( "Core::DirectionalLight" ) );

    PropertiesGenerator::InitializePanel( TXT( "DirectionalLight" ), CreatePanelSignature::Delegate( &DirectionalLight::CreatePanel ) );
}

void DirectionalLight::CleanupType()
{
    Reflect::UnregisterClassType< Core::DirectionalLight >();
}

DirectionalLight::DirectionalLight(Core::Scene* scene)
: Core::Light (scene, new Content::DirectionalLight() )
{

}

DirectionalLight::DirectionalLight(Core::Scene* scene, Content::DirectionalLight* light)
: Core::Light ( scene, light )
{

}

i32 DirectionalLight::GetImageIndex() const
{
    return -1; // Helium::GlobalFileIconsTable().GetIconID( TXT( "light" ) );
}

tstring DirectionalLight::GetApplicationTypeName() const
{
    return TXT( "DirectionalLight" );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by DirectionalLight.
//
bool DirectionalLight::ValidatePanel(const tstring& name)
{
    if (name == TXT( "DirectionalLight" ) )
    {
        return true;
    }

    return __super::ValidatePanel( name );
}

void DirectionalLight::CreatePanel( CreatePanelArgs& args )
{
    args.m_Generator->PushContainer( TXT( "Directional Light" ) );
    {
        args.m_Generator->PushContainer();
        {
            static const tstring helpText = TXT( "If this is enabled, the scene will have a global directional light (akin to the sun)." );
            args.m_Generator->AddLabel( TXT( "Global Sun" ) )->a_HelpText.Set( helpText );
            args.m_Generator->AddCheckBox<DirectionalLight, bool>( args.m_Selection, &DirectionalLight::GetGlobalSun, &DirectionalLight::SetGlobalSun )->a_HelpText.Set( helpText );
        }
        args.m_Generator->Pop();

        args.m_Generator->PushContainer();
        {
            static const tstring helpText = TXT( "This controls how soft the shadows are as cast by the currently selected directional light." );
            args.m_Generator->AddLabel( TXT( "Shadow Softness" ) )->a_HelpText.Set( helpText );
            args.m_Generator->AddValue<DirectionalLight, float>( args.m_Selection, &DirectionalLight::GetShadowSoftness, &DirectionalLight::SetShadowSoftness )->a_HelpText.Set( helpText );
            Inspect::Slider* slider = args.m_Generator->AddSlider<DirectionalLight, float>( args.m_Selection, &DirectionalLight::GetShadowSoftness, &DirectionalLight::SetShadowSoftness );
            slider->a_HelpText.Set( helpText );
        }
        args.m_Generator->Pop();

        args.m_Generator->PushContainer();
        {
            static const tstring helpText = TXT( "This controls how many sample points are used to control soft shadows." );
            args.m_Generator->AddLabel( TXT( "Soft Shadow Samples" ) )->a_HelpText.Set( helpText );
            args.m_Generator->AddValue<DirectionalLight, int>( args.m_Selection, &DirectionalLight::GetSoftShadowSamples, &DirectionalLight::SetSoftShadowSamples )->a_HelpText.Set( helpText );
            args.m_Generator->AddSlider<DirectionalLight, int>( args.m_Selection, &DirectionalLight::GetSoftShadowSamples, &DirectionalLight::SetSoftShadowSamples )->a_HelpText.Set( helpText );
        }
        args.m_Generator->Pop();
    }
    args.m_Generator->Pop();

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
