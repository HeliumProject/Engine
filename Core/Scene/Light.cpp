/*#include "Precompile.h"*/
#include "Light.h"

#include "Foundation/Component/ComponentHandle.h"

#include "Core/Scene/Scene.h"
#include "Core/Scene/SceneManager.h"
#include "Core/Scene/InstanceType.h"
#include "Core/Scene/PropertiesGenerator.h"
#include "Core/Scene/PrimitiveSphere.h"
#include "Core/Scene/PrimitivePointer.h"
#include "Core/Scene/PrimitiveCube.h"
#include "Core/Scene/PrimitiveCircle.h"
#include "Core/Scene/PrimitiveCylinder.h"
#include "Core/Scene/LightPanel.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Core;

// RTTI
SCENE_DEFINE_TYPE(Core::Light);

// statics
D3DMATERIAL9 Light::s_Material;

void Light::InitializeType()
{
    Reflect::RegisterClassType< Core::Light >( TXT( "Core::Light" ) );

    ZeroMemory(&s_Material, sizeof(s_Material));

    PropertiesGenerator::InitializePanel( TXT( "Light" ), CreatePanelSignature::Delegate( &Light::CreatePanel ) );
}

void Light::CleanupType()
{
    Reflect::UnregisterClassType< Core::Light >();
}

Light::Light(Core::Scene* scene, Content::Light* light)
: Core::Instance ( scene, light )
, m_SelectionHelper(LightSelectionChoices::NumLightSelectionChoices)
{

}

i32 Light::GetImageIndex() const
{
    return -1; // Helium::GlobalFileIconsTable().GetIconID( TXT( "light" ) );
}

tstring Light::GetApplicationTypeName() const
{
    return TXT( "Light" );
}

void Light::Evaluate(GraphDirection direction)
{
    __super::Evaluate(direction);

    switch (direction)
    {
    case GraphDirections::Downstream:
        {
            // start the box from scratch
            m_ObjectBounds.Reset();

            if ( m_NodeType )
            {
                Core::InstanceType* type = Reflect::AssertCast<Core::InstanceType>(m_NodeType);
                Math::AlignedBox box (type->GetPointer()->GetBounds());

                Math::Scale scale;
                Math::Matrix3 rotate;
                Math::Vector3 translate;
                m_InverseGlobalTransform.Decompose (scale, rotate, translate);

                // this will compensate for the normalized render of the pointer
                box.Transform (Math::Matrix4 (scale));
                m_ObjectBounds.Merge( box );
            }
            break;
        }
    }
}

void Light::Render( RenderVisitor* render )
{
    // pointer is drawn normalized
    {
        RenderEntry* entry = render->Allocate(this);
        entry->m_Location = render->State().m_Matrix.Normalized();
        entry->m_Center = m_ObjectBounds.Center();
        entry->m_Draw = &Light::DrawPointer;
    }

    // don't call __super here, it will draw big ass axes
    Core::HierarchyNode::Render( render );
}

void Light::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const Core::Light* light = Reflect::ConstAssertCast<Core::Light>( object );

    const Core::InstanceType* type = Reflect::ConstAssertCast<Core::InstanceType>( light->GetNodeType() );

    light->SetMaterial( type->GetMaterial() );

    // draw type pointer
    type->GetPointer()->Draw( args );
}

bool Light::Pick( PickVisitor* pick )
{
    Core::InstanceType* type = Reflect::AssertCast<Core::InstanceType>(m_NodeType);

    pick->SetCurrentObject (this, pick->State().m_Matrix.Normalized());

    if (type->GetPointer()->Pick(pick))
    {
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Core::Light.
//
bool Light::ValidatePanel(const tstring& name)
{
    if (name == TXT( "Light" ) )
    {
        return true;
    }

    return __super::ValidatePanel( name );
}

void Light::CreatePanel( CreatePanelArgs& args )
{

    args.m_Generator->PushPanel( TXT( "Light" ), true);
    {
        args.m_Generator->PushContainer();
        {
            args.m_Generator->AddLabel( TXT( "Color" ) );
            args.m_Generator->AddColorPicker<Light, Color3>( args.m_Selection, &Light::GetColor, &Light::SetColor );

            Inspect::Slider* slider = args.m_Generator->AddSlider<Light, float>( args.m_Selection, &Light::GetIntensity, &Light::SetIntensity );
            args.m_Generator->AddValue<Light, float>( args.m_Selection, &Light::GetIntensity, &Light::SetIntensity );
        }
        args.m_Generator->Pop();

        args.m_Generator->PushContainer();
        {
            args.m_Generator->AddLabel( TXT( "Photon Emissive" ) );
            args.m_Generator->AddCheckBox<Light, bool>( args.m_Selection, &Light::GetEmitPhotons, &Light::SetEmitPhotons );
        }
        args.m_Generator->Pop();

        args.m_Generator->PushContainer();
        {
            args.m_Generator->AddLabel( TXT( "Photon Energy" ) );
            args.m_Generator->AddColorPicker<Core::Light, Color3>( args.m_Selection, &Light::GetPhotonColor, &Light::SetPhotonColor );

            Inspect::Slider* slider = args.m_Generator->AddSlider<Core::Light, float>( args.m_Selection, &Light::GetPhotonIntensity, &Light::SetPhotonIntensity );
            slider->SetRangeMin( 0.0f );

            args.m_Generator->AddValue<Core::Light, float>( args.m_Selection, &Light::GetPhotonIntensity, &Light::SetPhotonIntensity );
        }
        args.m_Generator->Pop();

        args.m_Generator->PushContainer();
        {
            args.m_Generator->AddLabel( TXT( "Number of Photons" ) );
            args.m_Generator->AddValue<Core::Light, u32>( args.m_Selection, &Light::GetNumPhotons, &Light::SetNumPhotons );
        }
        args.m_Generator->Pop();


        args.m_Generator->PushContainer();
        {
            args.m_Generator->AddLabel( TXT( "Attenuate" ) );
            args.m_Generator->AddCheckBox<Core::Light, bool>( args.m_Selection, &Light::GetAttenuate, &Light::SetAttenuate );
        }
        args.m_Generator->Pop();

        args.m_Generator->PushContainer();
        {
            args.m_Generator->AddLabel( TXT( "Cast Shadows" ) );
            args.m_Generator->AddCheckBox<Core::Light, bool>( args.m_Selection, &Light::GetCastsShadows, &Light::SetCastsShadows );
        }
        args.m_Generator->Pop();

        args.m_Generator->PushContainer();
        {
            args.m_Generator->AddLabel( TXT( "Shadow Factor" ) );
            Inspect::Slider* slider = args.m_Generator->AddSlider<Core::Light, f32>( args.m_Selection, &Light::GetShadowFactor, &Light::SetShadowFactor );
            slider->SetRangeMin( 0.0f );
            slider->SetRangeMax( 1.0f );
        }
        args.m_Generator->Pop();

        args.m_Generator->PushContainer();
        {
            args.m_Generator->AddLabel( TXT( "Visibility Ray Test" ) );
            args.m_Generator->AddCheckBox<Core::Light, bool>( args.m_Selection, &Light::GetVisibilityRayTest, &Light::SetVisibilityRayTest );
        }
        args.m_Generator->Pop();

        args.m_Generator->PushContainer();
        {
            args.m_Generator->AddLabel( TXT( "Render Type" ) );
            Inspect::Choice* choice = args.m_Generator->AddChoice<Core::Light, int>(args.m_Selection, &Light::GetRenderType, &Light::SetRenderType);
            choice->SetDropDown( true );

            Inspect::V_Item items;
            {
                {
                    tostringstream str;
                    str << Content::LightRenderTypes::Baked;
                    items.push_back( Inspect::Item( TXT( "Baked" ), str.str() ) );
                }

                {
                    tostringstream str;
                    str << Content::LightRenderTypes::RealTime;
                    items.push_back( Inspect::Item( TXT( "RealTime" ), str.str() ) );
                }

                {
                    tostringstream str;
                    str << Content::LightRenderTypes::LightmapOnly;
                    items.push_back( Inspect::Item( TXT( "LightmapOnly" ), str.str() ) );
                }
            }
            choice->SetItems( items );
        }
        args.m_Generator->Pop();

        args.m_Generator->PushContainer();
        {
            args.m_Generator->AddLabel( TXT( "Lens Flare Type" ) );
            Inspect::Choice* choice = args.m_Generator->AddChoice<Core::Light, int>(args.m_Selection, &Light::GetLensFlareType, &Light::SetLensFlareType);
            choice->SetDropDown( true );

            Inspect::V_Item items;
            {
                {
                    tostringstream str;
                    str << Content::LensFlareTypes::Disabled;
                    items.push_back( Inspect::Item( TXT( "Disabled" ), str.str() ) );
                }

                {
                    tostringstream str;
                    str << Content::LensFlareTypes::Vortex;
                    items.push_back( Inspect::Item( TXT( "Vortex" ), str.str() ) );
                }

                {
                    tostringstream str;
                    str << Content::LensFlareTypes::VortexNoTrail;
                    items.push_back( Inspect::Item( TXT( "Vortex No Trail" ), str.str() ) );
                }
            }
            choice->SetItems( items );
        }
        args.m_Generator->Pop();

        args.m_Generator->PushContainer();
        {
            args.m_Generator->AddLabel( TXT( "Physical Light" ) );
            args.m_Generator->AddCheckBox<Core::Light, bool>( args.m_Selection, &Light::GetPhysicalLight, &Light::SetPhysicalLight );
        }
        args.m_Generator->Pop();


        args.m_Generator->PushPanel( TXT( "RealTime" ) );
        {

            LightPanel* panel = new LightPanel ( args.m_Generator, args.m_Selection );

            args.m_Generator->Push( panel );
            {
                panel->SetCanvas( args.m_Generator->GetContainer()->GetCanvas() );
                panel->Create();
            }
            args.m_Generator->Pop();



            args.m_Generator->PushContainer();
            {
                args.m_Generator->AddLabel( TXT( "Draw Distance" ) );
                args.m_Generator->AddValue<Core::Light, float>( args.m_Selection, &Light::GetDrawDist, &Light::SetDrawDist );
            }
            args.m_Generator->Pop();

            args.m_Generator->PushContainer();
            {
                args.m_Generator->AddLabel( TXT( "Kill If Inactive" ) );
                args.m_Generator->AddCheckBox<Core::Light, bool>( args.m_Selection, &Light::GetKillIfInactive, &Light::SetKillIfInactive );
            }
            args.m_Generator->Pop();

            args.m_Generator->PushContainer();
            {
                args.m_Generator->AddLabel( TXT( "Allow Oversized" ) );
                args.m_Generator->AddCheckBox<Core::Light, bool>( args.m_Selection, &Light::GetAllowOversized, &Light::SetAllowOversized );
            }
            args.m_Generator->Pop();

            args.m_Generator->PushPanel( TXT( "Animation" ) );
            {
                args.m_Generator->PushContainer();
                {
                    args.m_Generator->AddLabel( TXT( "Color" ) );
                    args.m_Generator->AddKeyControl<Core::Light>( args.m_Selection, &Reflect::CreateObject< Content::ParametricColorKey >, &Light::GetColorAnimation, &Light::SetColorAnimation );
                }
                args.m_Generator->Pop();

                args.m_Generator->PushContainer();
                {
                    args.m_Generator->AddLabel( TXT( "Flicker - r = amplitude, g = frequency" ) );
                    args.m_Generator->AddKeyControl<Core::Light>( args.m_Selection, &Reflect::CreateObject< Content::ParametricColorKey >, &Light::GetIntensityAnimation, &Light::SetIntensityAnimation );
                }
                args.m_Generator->Pop();

                args.m_Generator->PushContainer();
                {
                    args.m_Generator->AddLabel( TXT( "Animation Duration" ) );
                    args.m_Generator->AddValue<Core::Light, float>( args.m_Selection, &Light::GetAnimationDuration, &Light::SetAnimationDuration );
                }
                args.m_Generator->Pop();

                args.m_Generator->PushContainer();
                {
                    args.m_Generator->AddLabel( TXT( "Use Random Offset" ) );
                    args.m_Generator->AddCheckBox<Core::Light, bool>( args.m_Selection, &Light::GetRandomAnimOffset, &Light::SetRandomAnimOffset );
                }
                args.m_Generator->Pop();
            }
            args.m_Generator->Pop();

        }
        args.m_Generator->Pop();


        args.m_Generator->PushPanel( TXT( "Selection Helper" ) );
        {
            /*
            args.m_Generator->PushContainer();
            {
            args.m_Generator->AddLabel("Light Type");
            args.m_Generator->AddCheckBox<Core::Light, bool>( args.m_Selection, &Light::GetSelectionHelperLightType, &Light::SetSelectionHelperLightType );
            }
            */

            args.m_Generator->PushContainer();
            {
                args.m_Generator->AddLabel( TXT( "Render Type" ) );
                args.m_Generator->AddCheckBox<Core::Light, bool>( args.m_Selection, &Light::GetSelectionHelperRenderType, &Light::SetSelectionHelperRenderType );
            }
            args.m_Generator->Pop();

            args.m_Generator->PushContainer();
            {
                args.m_Generator->AddLabel( TXT( "Color" ) );
                args.m_Generator->AddCheckBox<Core::Light, bool>( args.m_Selection, &Light::GetSelectionHelperColor, &Light::SetSelectionHelperColor );
            }
            args.m_Generator->Pop();

            args.m_Generator->PushContainer();
            {
                args.m_Generator->AddLabel( TXT( "Intensity" ) );
                args.m_Generator->AddCheckBox<Core::Light, bool>( args.m_Selection, &Light::GetSelectionHelperIntensity, &Light::SetSelectionHelperIntensity );
            }
            args.m_Generator->Pop();


            args.m_Generator->PushContainer();
            {
                args.m_Generator->AddLabel( TXT( "Scale" ) );
                args.m_Generator->AddCheckBox<Core::Light, bool>( args.m_Selection, &Light::GetSelectionHelperScale, &Light::SetSelectionHelperScale );
            }
            args.m_Generator->Pop();

            args.m_Generator->PushContainer();
            {
                args.m_Generator->AddLabel( TXT( "Lens Flare" ) );
                args.m_Generator->AddCheckBox<Core::Light, bool>( args.m_Selection, &Light::GetSelectionHelperLensFlare, &Light::SetSelectionHelperLensFlare );
            }
            args.m_Generator->Pop();

            args.m_Generator->PushContainer();
            {
                args.m_Generator->AddLabel( TXT( "Physical Light" ) );
                args.m_Generator->AddCheckBox<Core::Light, bool>( args.m_Selection, &Light::GetSelectionHelperPhysicalLight, &Light::SetSelectionHelperPhysicalLight );
            }
            args.m_Generator->Pop();
        }
        args.m_Generator->Pop();
    }
    args.m_Generator->Pop();
}

Color3 Light::GetColor() const
{
    return GetPackage< Content::Light >()->m_Color;
}

void Light::SetColor( Color3 color )
{
    GetPackage< Content::Light >()->m_Color = color;

    m_Changed.Raise( LightChangeArgs( this ) );
}

float Light::GetIntensity() const
{
    return GetPackage< Content::Light >()->m_Color.s;
}
void Light::SetIntensity( float intensity )
{
    GetPackage< Content::Light >()->m_Color.s = intensity;

    m_Changed.Raise( LightChangeArgs( this ) );
}

i32 Light::GetRenderType() const
{
    return GetPackage< Content::Light >()->m_RenderType;
}

void Light::SetRenderType( i32 renderType )
{
    Content::LightRenderType oldRenderType = GetPackage< Content::Light >()->m_RenderType;
    GetPackage< Content::Light >()->m_RenderType = (Content::LightRenderType)renderType;

    // if we're adding a real-time component, create a new real-time light in the remote view
    // if we're removing a real-time component, remove the real-time light from the remote view
    if ( oldRenderType != Content::LightRenderTypes::RealTime && renderType == Content::LightRenderTypes::RealTime )
    {
        // light was baked, now it's realtime
        m_RealtimeExistence.Raise( RealtimeLightExistenceArgs( this, true ) );
    }
    else if ( oldRenderType == Content::LightRenderTypes::RealTime && renderType != Content::LightRenderTypes::RealTime )
    {
        // light was realtime, not anymore
        m_RealtimeExistence.Raise( RealtimeLightExistenceArgs( this, false ) );
    }
}

i32 Light::GetLensFlareType() const
{
    return GetPackage< Content::Light >()->m_FlareType;
}

void Light::SetLensFlareType( i32 type )
{
    GetPackage< Content::Light >()->m_FlareType = (Content::LensFlareType)type;

    m_Changed.Raise( LightChangeArgs( this ) );
}

float Light::GetDrawDist() const
{
    return GetPackage< Content::Light >()->m_DrawDist;
}

void Light::SetDrawDist( float distance )
{
    GetPackage< Content::Light >()->m_DrawDist = distance;

    m_Changed.Raise( LightChangeArgs( this ) );
}

bool Light::GetKillIfInactive() const
{
    return GetPackage< Content::Light >()->m_KillIfInactive;
}
void Light::SetKillIfInactive( bool b )
{
    GetPackage< Content::Light >()->m_KillIfInactive = b;

    m_Changed.Raise( LightChangeArgs( this ) );
}

bool Light::GetVisibilityRayTest() const
{
    return GetPackage< Content::Light >()->m_DoVisibilityRayTest;
}
void Light::SetVisibilityRayTest( bool b )
{
    GetPackage< Content::Light >()->m_DoVisibilityRayTest = b;

    m_Changed.Raise( LightChangeArgs( this ) );
}

bool Light::GetCastsShadows() const
{
    return GetPackage< Content::Light >()->m_CastShadows;
}
void Light::SetCastsShadows( bool b )
{
    GetPackage< Content::Light >()->m_CastShadows = b;

    m_Changed.Raise( LightChangeArgs( this ) );
}

bool Light::GetAttenuate() const
{
    return GetPackage< Content::Light >()->m_Attenuate;
}
void Light::SetAttenuate( bool b )
{
    GetPackage< Content::Light >()->m_Attenuate = b;
}

f32 Light::GetShadowFactor() const
{
    return GetPackage< Content::Light >()->m_Factor;
}
void Light::SetShadowFactor( f32 f )
{
    GetPackage< Content::Light >()->m_Factor = f;
}

bool Light::GetAllowOversized() const
{
    return GetPackage< Content::Light >()->m_AllowOversized;
}
void Light::SetAllowOversized( bool b )
{
    GetPackage< Content::Light >()->m_AllowOversized = b;

    m_Changed.Raise( LightChangeArgs( this ) );
}

bool Light::GetEmitPhotons() const
{
    return GetPackage< Content::Light >()->m_EmitPhotons;
}
void Light::SetEmitPhotons( bool b )
{
    GetPackage< Content::Light >()->m_EmitPhotons = b;
}

u32 Light::GetNumPhotons() const
{
    return GetPackage< Content::Light >()->m_NumPhotons;
}

void Light::SetNumPhotons( u32 numPhotons )
{
    GetPackage< Content::Light >()->m_NumPhotons = numPhotons;
}


Color3 Light::GetPhotonColor() const
{
    return GetPackage< Content::Light >()->m_PhotonEnergy;
}

void Light::SetPhotonColor( Color3 color )
{
    GetPackage< Content::Light >()->m_PhotonEnergy = color;

    m_Changed.Raise( LightChangeArgs( this ) );
}

float Light::GetPhotonIntensity() const
{
    return GetPackage< Content::Light >()->m_PhotonEnergy.s;
}

void Light::SetPhotonIntensity( float intensity )
{
    GetPackage< Content::Light >()->m_PhotonEnergy.s = intensity;
}

f32 Light::GetAnimationDuration() const
{
    return GetPackage< Content::Light >()->m_AnimationDuration;
}

void Light::SetAnimationDuration( f32 duration )
{
    GetPackage< Content::Light >()->m_AnimationDuration = duration;
    m_Changed.Raise( LightChangeArgs( this ) );
}


bool Light::GetRandomAnimOffset() const
{
    return GetPackage< Content::Light >()->m_RandomAnimOffset;
}

void Light::SetRandomAnimOffset( bool b )
{
    GetPackage< Content::Light >()->m_RandomAnimOffset = b;
    m_Changed.Raise( LightChangeArgs( this ) );
}


bool Light::GetPhysicalLight() const
{
    return GetPackage< Content::Light >()->m_PhysicalLight;
}

void Light::SetPhysicalLight( bool b )
{
    GetPackage< Content::Light >()->m_PhysicalLight = b;
}

bool Light::GetSelectionHelperRenderType() const
{
    return m_SelectionHelper[LightSelectionChoices::RenderType];
}

void Light::SetSelectionHelperRenderType( bool b )
{
    m_SelectionHelper[LightSelectionChoices::RenderType] = b;
}

bool Light::GetSelectionHelperColor() const
{
    return m_SelectionHelper[LightSelectionChoices::Color];
}

void Light::SetSelectionHelperColor( bool b )
{
    m_SelectionHelper[LightSelectionChoices::Color] = b;
}

bool Light::GetSelectionHelperIntensity() const
{
    return m_SelectionHelper[LightSelectionChoices::Intensity];
}

void Light::SetSelectionHelperIntensity( bool b )
{
    m_SelectionHelper[LightSelectionChoices::Intensity] = b;
}

bool Light::GetSelectionHelperScale() const
{
    return m_SelectionHelper[LightSelectionChoices::Scale];
}

void Light::SetSelectionHelperScale( bool b )
{
    m_SelectionHelper[LightSelectionChoices::Scale] = b;
}

bool Light::GetSelectionHelperLensFlare() const
{
    return m_SelectionHelper[LightSelectionChoices::LensFlare];
}

void Light::SetSelectionHelperLensFlare( bool b )
{
    m_SelectionHelper[LightSelectionChoices::LensFlare] = b;
}

bool Light::GetSelectionHelperPhysicalLight() const
{
    return m_SelectionHelper[LightSelectionChoices::PhysicalLight];
}

void Light::SetSelectionHelperPhysicalLight( bool b )
{
    m_SelectionHelper[LightSelectionChoices::PhysicalLight] = b;
}

static bool CompareLights( Content::Light* light1, Content::Light* light2, BitArray& selectionHelperMask  )
{
    HELIUM_ASSERT( light1 && light2 );

    if( selectionHelperMask[ LightSelectionChoices::RenderType ] )
    {
        if( light1->m_RenderType != light2->m_RenderType )
            return false;
    }

    if( selectionHelperMask[ LightSelectionChoices::Color ] )
    {
        if( light1->m_Color.r != light2->m_Color.r && light1->m_Color.g != light2->m_Color.g && light1->m_Color.b != light2->m_Color.b )
            return false;
    }

    if( selectionHelperMask[ LightSelectionChoices::Intensity ] )
    {
        if( light1->m_Color.s != light2->m_Color.s )
            return false;
    }

    if( selectionHelperMask[ LightSelectionChoices::Scale ] )
    {
        if( !light1->m_Scale.Equal( light2->m_Scale, 0.0001f ) )
            return false;
    }

    if( selectionHelperMask[ LightSelectionChoices::LensFlare ] )
    {
        if( !light1->m_FlareType == light2->m_FlareType )
            return false;
    }

    if( selectionHelperMask[ LightSelectionChoices::PhysicalLight ] )
    {
        if( !light1->m_PhysicalLight == light2->m_PhysicalLight )
            return false;
    }

    return true;


}

