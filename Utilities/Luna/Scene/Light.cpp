#include "Precompile.h"
#include "Light.h"
#include "Application.h"

#include "Scene.h"
#include "SceneManager.h"
#include "InstanceType.h"

#include "Core/Enumerator.h"
#include "SceneEditor.h"
#include "Application/UI/ImageManager.h"

#include "PrimitiveSphere.h"
#include "PrimitivePointer.h"
#include "PrimitiveCube.h"
#include "PrimitiveCircle.h"
#include "PrimitiveCylinder.h"

#include "Pipeline/Content/Nodes/Lights/AreaLights/SphereLight.h"
#include "Pipeline/Content/Nodes/Lights/AreaLights/RectangleLight.h"
#include "Pipeline/Content/Nodes/Lights/AreaLights/CylinderLight.h"
#include "Pipeline/Content/Nodes/Lights/AreaLights/DiscLight.h"

#include "Pipeline/Component/ComponentHandle.h"
#include "LightPanel.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(Luna::Light);

// statics
D3DMATERIAL9 Light::s_Material;

void Light::InitializeType()
{
    Reflect::RegisterClass< Luna::Light >( TXT( "Luna::Light" ) );

    ZeroMemory(&s_Material, sizeof(s_Material));

    Enumerator::InitializePanel( TXT( "Light" ), CreatePanelSignature::Delegate( &Light::CreatePanel ) );
}

void Light::CleanupType()
{
    Reflect::UnregisterClass< Luna::Light >();
}

Light::Light(Luna::Scene* scene, Content::Light* light)
: Luna::Instance ( scene, light )
, m_AreaLightType( Content::AreaLightTypes::None )
, m_SelectionHelper(LightSelectionChoices::NumLightSelectionChoices)
, m_AreaLightPrim( NULL )
{

}

void Light::Initialize()
{
    __super::Initialize();

    Content::AreaLight* areaLight = GetPackage< Content::Light >()->m_AreaLight;
    if( areaLight )
    {
        if( areaLight->HasType( Reflect::GetType< Content::SphereLight >() ) )
        {
            SetAreaLightType( Content::AreaLightTypes::Sphere );
        }
        else if( areaLight->HasType( Reflect::GetType< Content::RectangleLight >() ) )
        {
            SetAreaLightType( Content::AreaLightTypes::Rectangle );
        } 
        else if( areaLight->HasType( Reflect::GetType< Content::CylinderLight >() ) )
        {
            SetAreaLightType( Content::AreaLightTypes::Cylinder );
        } 
    }
}

void Light::Delete()
{
    __super::Delete();

    if (m_AreaLightPrim)
    {
        m_AreaLightPrim->Delete();
    }
}

i32 Light::GetImageIndex() const
{
    return Nocturnal::GlobalImageManager().GetImageIndex( TXT( "light.png" ) );
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
                Luna::InstanceType* type = Reflect::AssertCast<Luna::InstanceType>(m_NodeType);
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
    Luna::HierarchyNode::Render( render );
}

void Light::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const Luna::Light* light = Reflect::ConstAssertCast<Luna::Light>( object );

    const Luna::InstanceType* type = Reflect::ConstAssertCast<Luna::InstanceType>( light->GetNodeType() );

    light->SetMaterial( type->GetMaterial() );

    // draw type pointer
    type->GetPointer()->Draw( args );
}

bool Light::Pick( PickVisitor* pick )
{
    Luna::InstanceType* type = Reflect::AssertCast<Luna::InstanceType>(m_NodeType);

    pick->SetCurrentObject (this, pick->State().m_Matrix.Normalized());

    if (type->GetPointer()->Pick(pick))
    {
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Luna::Light.
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

    args.m_Enumerator->PushPanel( TXT( "Light" ), true);
    {
        args.m_Enumerator->PushContainer();
        {
            args.m_Enumerator->AddLabel( TXT( "Color" ) );
            args.m_Enumerator->AddColorPicker<Light, Color3>( args.m_Selection, &Light::GetColor, &Light::SetColor );

            Inspect::Slider* slider = args.m_Enumerator->AddSlider<Light, float>( args.m_Selection, &Light::GetIntensity, &Light::SetIntensity );
            args.m_Enumerator->AddValue<Light, float>( args.m_Selection, &Light::GetIntensity, &Light::SetIntensity );
        }
        args.m_Enumerator->Pop();

        args.m_Enumerator->PushContainer();
        {
            args.m_Enumerator->AddLabel( TXT( "Photon Emissive" ) );
            args.m_Enumerator->AddCheckBox<Light, bool>( args.m_Selection, &Light::GetEmitPhotons, &Light::SetEmitPhotons );
        }
        args.m_Enumerator->Pop();

        args.m_Enumerator->PushContainer();
        {
            args.m_Enumerator->AddLabel( TXT( "Photon Energy" ) );
            args.m_Enumerator->AddColorPicker<Luna::Light, Color3>( args.m_Selection, &Light::GetPhotonColor, &Light::SetPhotonColor );

            Inspect::Slider* slider = args.m_Enumerator->AddSlider<Luna::Light, float>( args.m_Selection, &Light::GetPhotonIntensity, &Light::SetPhotonIntensity );
            slider->SetRangeMin( 0.0f );

            args.m_Enumerator->AddValue<Luna::Light, float>( args.m_Selection, &Light::GetPhotonIntensity, &Light::SetPhotonIntensity );
        }
        args.m_Enumerator->Pop();

        args.m_Enumerator->PushContainer();
        {
            args.m_Enumerator->AddLabel( TXT( "Number of Photons" ) );
            args.m_Enumerator->AddValue<Luna::Light, u32>( args.m_Selection, &Light::GetNumPhotons, &Light::SetNumPhotons );
        }
        args.m_Enumerator->Pop();


        args.m_Enumerator->PushContainer();
        {
            args.m_Enumerator->AddLabel( TXT( "Attenuate" ) );
            args.m_Enumerator->AddCheckBox<Luna::Light, bool>( args.m_Selection, &Light::GetAttenuate, &Light::SetAttenuate );
        }
        args.m_Enumerator->Pop();

        args.m_Enumerator->PushContainer();
        {
            args.m_Enumerator->AddLabel( TXT( "Cast Shadows" ) );
            args.m_Enumerator->AddCheckBox<Luna::Light, bool>( args.m_Selection, &Light::GetCastsShadows, &Light::SetCastsShadows );
        }
        args.m_Enumerator->Pop();

        args.m_Enumerator->PushContainer();
        {
            args.m_Enumerator->AddLabel( TXT( "Shadow Factor" ) );
            Inspect::Slider* slider = args.m_Enumerator->AddSlider<Luna::Light, f32>( args.m_Selection, &Light::GetShadowFactor, &Light::SetShadowFactor );
            slider->SetRangeMin( 0.0f );
            slider->SetRangeMax( 1.0f );
        }
        args.m_Enumerator->Pop();

        args.m_Enumerator->PushContainer();
        {
            args.m_Enumerator->AddLabel( TXT( "Visibility Ray Test" ) );
            args.m_Enumerator->AddCheckBox<Luna::Light, bool>( args.m_Selection, &Light::GetVisibilityRayTest, &Light::SetVisibilityRayTest );
        }
        args.m_Enumerator->Pop();

        args.m_Enumerator->PushContainer();
        {
            args.m_Enumerator->AddLabel( TXT( "Render Type" ) );
            Inspect::Choice* choice = args.m_Enumerator->AddChoice<Luna::Light, int>(args.m_Selection, &Light::GetRenderType, &Light::SetRenderType);
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
        args.m_Enumerator->Pop();

        args.m_Enumerator->PushContainer();
        {
            args.m_Enumerator->AddLabel( TXT( "Lens Flare Type" ) );
            Inspect::Choice* choice = args.m_Enumerator->AddChoice<Luna::Light, int>(args.m_Selection, &Light::GetLensFlareType, &Light::SetLensFlareType);
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
        args.m_Enumerator->Pop();

        args.m_Enumerator->PushContainer();
        {
            args.m_Enumerator->AddLabel( TXT( "Area Light" ) );

            Inspect::Choice* choice = args.m_Enumerator->AddChoice<Luna::Light, int>(args.m_Selection, &Light::GetAreaLightType, &Light::SetAreaLightType);
            choice->SetDropDown( true );

            Inspect::V_Item items;
            {
                {
                    tostringstream str;
                    str << Content::AreaLightTypes::None;
                    items.push_back( Inspect::Item( TXT( "None" ), str.str() ) );
                }
                {
                    tostringstream str;
                    str << Content::AreaLightTypes::Sphere;
                    items.push_back( Inspect::Item( TXT( "Sphere" ), str.str() ) );
                }
                {
                    tostringstream str;
                    str << Content::AreaLightTypes::Rectangle;
                    items.push_back( Inspect::Item( TXT( "Rectangle" ), str.str() ) );
                }
                {
                    tostringstream str;
                    str << Content::AreaLightTypes::Cylinder;
                    items.push_back( Inspect::Item( TXT( "Cylinder" ), str.str() ) );
                }
                {
                    tostringstream str;
                    str << Content::AreaLightTypes::Disc;
                    items.push_back( Inspect::Item( TXT( "Disc" ), str.str() ) );
                }
            }
            choice->SetItems( items );
        }
        args.m_Enumerator->Pop();

        args.m_Enumerator->PushContainer();
        {
            args.m_Enumerator->AddLabel( TXT( "Physical Light" ) );
            args.m_Enumerator->AddCheckBox<Luna::Light, bool>( args.m_Selection, &Light::GetPhysicalLight, &Light::SetPhysicalLight );
        }
        args.m_Enumerator->Pop();


        args.m_Enumerator->PushPanel( TXT( "RealTime" ) );
        {

            LightPanel* panel = new LightPanel ( args.m_Enumerator, args.m_Selection );

            args.m_Enumerator->Push( panel );
            {
                panel->SetCanvas( args.m_Enumerator->GetContainer()->GetCanvas() );
                panel->Create();
            }
            args.m_Enumerator->Pop();



            args.m_Enumerator->PushContainer();
            {
                args.m_Enumerator->AddLabel( TXT( "Draw Distance" ) );
                args.m_Enumerator->AddValue<Luna::Light, float>( args.m_Selection, &Light::GetDrawDist, &Light::SetDrawDist );
            }
            args.m_Enumerator->Pop();

            args.m_Enumerator->PushContainer();
            {
                args.m_Enumerator->AddLabel( TXT( "Kill If Inactive" ) );
                args.m_Enumerator->AddCheckBox<Luna::Light, bool>( args.m_Selection, &Light::GetKillIfInactive, &Light::SetKillIfInactive );
            }
            args.m_Enumerator->Pop();

            args.m_Enumerator->PushContainer();
            {
                args.m_Enumerator->AddLabel( TXT( "Allow Oversized" ) );
                args.m_Enumerator->AddCheckBox<Luna::Light, bool>( args.m_Selection, &Light::GetAllowOversized, &Light::SetAllowOversized );
            }
            args.m_Enumerator->Pop();

            args.m_Enumerator->PushPanel( TXT( "Animation" ) );
            {
                args.m_Enumerator->PushContainer();
                {
                    args.m_Enumerator->AddLabel( TXT( "Color" ) );
                    args.m_Enumerator->AddKeyControl<Luna::Light>( args.m_Selection, &Reflect::CreateObject< Content::ParametricColorKey >, &Light::GetColorAnimation, &Light::SetColorAnimation );
                }
                args.m_Enumerator->Pop();

                args.m_Enumerator->PushContainer();
                {
                    args.m_Enumerator->AddLabel( TXT( "Flicker - r = amplitude, g = frequency" ) );
                    args.m_Enumerator->AddKeyControl<Luna::Light>( args.m_Selection, &Reflect::CreateObject< Content::ParametricColorKey >, &Light::GetIntensityAnimation, &Light::SetIntensityAnimation );
                }
                args.m_Enumerator->Pop();

                args.m_Enumerator->PushContainer();
                {
                    args.m_Enumerator->AddLabel( TXT( "Animation Duration" ) );
                    args.m_Enumerator->AddValue<Luna::Light, float>( args.m_Selection, &Light::GetAnimationDuration, &Light::SetAnimationDuration );
                }
                args.m_Enumerator->Pop();

                args.m_Enumerator->PushContainer();
                {
                    args.m_Enumerator->AddLabel( TXT( "Use Random Offset" ) );
                    args.m_Enumerator->AddCheckBox<Luna::Light, bool>( args.m_Selection, &Light::GetRandomAnimOffset, &Light::SetRandomAnimOffset );
                }
                args.m_Enumerator->Pop();
            }
            args.m_Enumerator->Pop();

        }
        args.m_Enumerator->Pop();


        args.m_Enumerator->PushPanel( TXT( "Selection Helper" ) );
        {
            /*
            args.m_Enumerator->PushContainer();
            {
            args.m_Enumerator->AddLabel("Light Type");
            args.m_Enumerator->AddCheckBox<Luna::Light, bool>( args.m_Selection, &Light::GetSelectionHelperLightType, &Light::SetSelectionHelperLightType );
            }
            */

            args.m_Enumerator->PushContainer();
            {
                args.m_Enumerator->AddLabel( TXT( "Render Type" ) );
                args.m_Enumerator->AddCheckBox<Luna::Light, bool>( args.m_Selection, &Light::GetSelectionHelperRenderType, &Light::SetSelectionHelperRenderType );
            }
            args.m_Enumerator->Pop();

            args.m_Enumerator->PushContainer();
            {
                args.m_Enumerator->AddLabel( TXT( "Color" ) );
                args.m_Enumerator->AddCheckBox<Luna::Light, bool>( args.m_Selection, &Light::GetSelectionHelperColor, &Light::SetSelectionHelperColor );
            }
            args.m_Enumerator->Pop();

            args.m_Enumerator->PushContainer();
            {
                args.m_Enumerator->AddLabel( TXT( "Intensity" ) );
                args.m_Enumerator->AddCheckBox<Luna::Light, bool>( args.m_Selection, &Light::GetSelectionHelperIntensity, &Light::SetSelectionHelperIntensity );
            }
            args.m_Enumerator->Pop();


            args.m_Enumerator->PushContainer();
            {
                args.m_Enumerator->AddLabel( TXT( "Scale" ) );
                args.m_Enumerator->AddCheckBox<Luna::Light, bool>( args.m_Selection, &Light::GetSelectionHelperScale, &Light::SetSelectionHelperScale );
            }
            args.m_Enumerator->Pop();

            args.m_Enumerator->PushContainer();
            {
                args.m_Enumerator->AddLabel( TXT( "Lens Flare" ) );
                args.m_Enumerator->AddCheckBox<Luna::Light, bool>( args.m_Selection, &Light::GetSelectionHelperLensFlare, &Light::SetSelectionHelperLensFlare );
            }
            args.m_Enumerator->Pop();

            args.m_Enumerator->PushContainer();
            {
                args.m_Enumerator->AddLabel( TXT( "Physical Light" ) );
                args.m_Enumerator->AddCheckBox<Luna::Light, bool>( args.m_Selection, &Light::GetSelectionHelperPhysicalLight, &Light::SetSelectionHelperPhysicalLight );
            }
            args.m_Enumerator->Pop();

            args.m_Enumerator->PushContainer();
            {
                Inspect::Action* button = args.m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( &Light::OnSelectionHelper ) );
                button->SetClientData( new SelectionDataObject( args.m_Selection ) );
                button->SetText( TXT( "Select Lights" ) );
            }
            args.m_Enumerator->Pop();
        }
        args.m_Enumerator->Pop();
    }
    args.m_Enumerator->Pop();
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


int Light::GetAreaLightType() const
{
    return m_AreaLightType;
}
void Light::SetAreaLightType( int type )
{
    if( m_AreaLightType == type )
    {
        return;
    }

    m_AreaLightType = static_cast< Content::AreaLightType >(type);

    switch( type )
    {
    case Content::AreaLightTypes::None:
        {
            GetPackage< Content::Light >()->m_AreaLight = NULL;

            if( m_AreaLightPrim )
            {
                m_AreaLightPrim->Delete();
                m_AreaLightPrim = NULL;
            }

            break;
        }

    case Content::AreaLightTypes::Sphere:
        {
            Content::AreaLightPtr areaLight = GetPackage< Content::Light >()->m_AreaLight;

            if( areaLight.ReferencesObject() )
            {
                if( !areaLight->HasType( Reflect::GetType< Content::SphereLight >() ) )
                {
                    Content::AreaLightPtr newAreaLight = new Content::SphereLight();
                    *newAreaLight = *areaLight;
                    GetPackage< Content::Light >()->m_AreaLight = newAreaLight;
                }
            }  
            else
            {
                Content::AreaLightPtr newAreaLight = new Content::SphereLight();
                GetPackage< Content::Light >()->m_AreaLight = newAreaLight;
            }


            if( m_AreaLightPrim )
            {
                m_AreaLightPrim->Delete();
                delete m_AreaLightPrim;
            }

            m_AreaLightPrim =  new PrimitiveSphere ( m_Scene->GetView()->GetResources() );
            m_AreaLightPrim->SetSolid( true );
            m_AreaLightPrim->SetTransparent( true );
            m_AreaLightPrim->Create();

            PrimitiveSphere* primSphere = Reflect::DangerousCast<PrimitiveSphere>( m_AreaLightPrim );

            primSphere->m_Radius = GetAreaLightRadius();
            primSphere->Update();

            break;
        }
    case Content::AreaLightTypes::Rectangle:
        {
            Content::AreaLightPtr areaLight = GetPackage< Content::Light >()->m_AreaLight;

            if( areaLight.ReferencesObject() )
            {
                if( !areaLight->HasType( Reflect::GetType< Content::RectangleLight >() ) )
                {
                    Content::AreaLightPtr newAreaLight = new Content::RectangleLight();
                    *newAreaLight = *areaLight;
                    GetPackage< Content::Light >()->m_AreaLight = newAreaLight;
                }
            }  
            else
            {
                Content::AreaLightPtr newAreaLight = new Content::RectangleLight();
                GetPackage< Content::Light >()->m_AreaLight = newAreaLight;
            }

            if( m_AreaLightPrim )
            {
                m_AreaLightPrim->Delete();
                delete m_AreaLightPrim;
            }

            Content::RectangleLight* rectLight = Reflect::DangerousCast<Content::RectangleLight>( GetPackage< Content::Light >()->m_AreaLight );

            m_AreaLightPrim =  new PrimitiveCube( m_Scene->GetView()->GetResources() );
            m_AreaLightPrim->SetSolid( true );
            m_AreaLightPrim->SetTransparent( true );

            PrimitiveCube* prim = Reflect::DangerousCast<PrimitiveCube>( m_AreaLightPrim );

            //prim->m_Radius = GetAreaLightRadius();

            Math::Vector3 min( -( rectLight->m_Dimensions.x * 0.5f ), -( rectLight->m_Dimensions.y * 0.5f ), 0.0f );
            Math::Vector3 max( ( rectLight->m_Dimensions.x * 0.5f ), ( rectLight->m_Dimensions.y * 0.5f ), 0.0f );
            prim->SetBounds( min, max );

            prim->Update();

            m_AreaLightPrim->Create();



            break;
        }
    case Content::AreaLightTypes::Cylinder:
        {
            Content::AreaLightPtr areaLight = GetPackage< Content::Light >()->m_AreaLight;

            if( areaLight.ReferencesObject() )
            {
                if( !areaLight->HasType( Reflect::GetType< Content::CylinderLight >() ) )
                {
                    Content::AreaLightPtr newAreaLight = new Content::CylinderLight();
                    *newAreaLight = *areaLight;
                    GetPackage< Content::Light >()->m_AreaLight = newAreaLight;
                }
            }  
            else
            {
                Content::AreaLightPtr newAreaLight = new Content::CylinderLight();
                GetPackage< Content::Light >()->m_AreaLight = newAreaLight;
            }

            if( m_AreaLightPrim )
            {
                m_AreaLightPrim->Delete();
                delete m_AreaLightPrim;
            }

            Content::CylinderLight* cylinderLight = Reflect::DangerousCast<Content::CylinderLight>( GetPackage< Content::Light >()->m_AreaLight );

            m_AreaLightPrim =  new PrimitiveCylinder( m_Scene->GetView()->GetResources() );
            m_AreaLightPrim->SetSolid( true );
            m_AreaLightPrim->SetTransparent( true );

            PrimitiveCylinder* prim = Reflect::DangerousCast<PrimitiveCylinder>( m_AreaLightPrim );
            prim->m_Radius = cylinderLight->m_Radius;
            prim->m_Length = cylinderLight->m_Length * 2.0f;
            prim->m_VerticalOrientation = false;

            prim->Update();

            m_AreaLightPrim->Create();



            break;
        }
    case Content::AreaLightTypes::Disc:
        {
            Content::AreaLightPtr areaLight = GetPackage< Content::Light >()->m_AreaLight;

            if( areaLight.ReferencesObject() )
            {
                if( !areaLight->HasType( Reflect::GetType< Content::DiscLight >() ) )
                {
                    Content::AreaLightPtr newAreaLight = new Content::DiscLight();
                    *newAreaLight = *areaLight;
                    GetPackage< Content::Light >()->m_AreaLight = newAreaLight;
                }
            }  
            else
            {
                Content::AreaLightPtr newAreaLight = new Content::DiscLight();
                GetPackage< Content::Light >()->m_AreaLight = newAreaLight;
            }

            if( m_AreaLightPrim )
            {
                m_AreaLightPrim->Delete();
                delete m_AreaLightPrim;
            }

            Content::DiscLight* cylinderLight = Reflect::DangerousCast<Content::DiscLight>( GetPackage< Content::Light >()->m_AreaLight );

            m_AreaLightPrim =  new PrimitiveCircle( m_Scene->GetView()->GetResources() );
            m_AreaLightPrim->SetSolid( true );
            m_AreaLightPrim->SetTransparent( true );

            PrimitiveCircle* prim = Reflect::DangerousCast<PrimitiveCircle>( m_AreaLightPrim );
            prim->m_Radius = cylinderLight->m_Radius;
            prim->m_HackyRotateFlag = true;
            prim->Update();

            m_AreaLightPrim->Create();



            break;
        }

    }

}
f32 Light::GetAreaLightRadius() const
{
    if( m_AreaLightType == Content::AreaLightTypes::None || m_AreaLightType == Content::AreaLightTypes::Rectangle )
    {
        return 0.0f;
    }

    Content::RadiusLight* areaLight = Reflect::ObjectCast< Content::RadiusLight >( GetPackage< Content::Light >()->m_AreaLight );
    if( areaLight )
    {
        return areaLight->m_Radius;
    }
    return 0.0f;
}

void Light::SetAreaLightRadius( f32 radius )
{
    if( m_AreaLightType == Content::AreaLightTypes::None )
    {
        return;
    }

    Content::RadiusLight* areaLight = Reflect::ObjectCast< Content::RadiusLight >( GetPackage< Content::Light >()->m_AreaLight );
    if( areaLight )
    {
        Reflect::DangerousCast< PrimitiveRadius >( m_AreaLightPrim )->m_Radius = radius;
        m_AreaLightPrim->Update();   
    }
}

Math::Vector2 Light::GetAreaLightDimensions() const
{
    if( m_AreaLightType == Content::AreaLightTypes::None )
    {
        return Math::Vector2::Zero;
    }

    Content::AreaLight* areaLight = GetPackage< Content::Light >()->m_AreaLight;
    if( areaLight )
    {
        if( m_AreaLightType == Content::AreaLightTypes::Rectangle )
        {
            Content::RectangleLight* light = Reflect::ObjectCast< Content::RectangleLight >( areaLight );
            if( light )
            {
                return light->m_Dimensions;
            }
        }
        else if( m_AreaLightType == Content::AreaLightTypes::Cylinder )
        {
            Content::CylinderLight* light = Reflect::ObjectCast< Content::CylinderLight >( areaLight );
            if( light )
            {
                Math::Vector2 dimensions( light->m_Radius, light->m_Length );
                return dimensions;
            }
        }
    }
    return Math::Vector2::Zero;

}
void Light::SetAreaLightDimensions( Math::Vector2 dim )
{
    if( m_AreaLightType == Content::AreaLightTypes::None )
    {
        return;
    }

    Content::AreaLight* areaLight = GetPackage< Content::Light >()->m_AreaLight;
    if( areaLight )
    {
        // RECTANGLE
        if( m_AreaLightType == Content::AreaLightTypes::Rectangle )
        {
            Content::RectangleLight* light = Reflect::ObjectCast< Content::RectangleLight >( areaLight );
            if( light )
            {
                light->m_Dimensions = dim;
                if( m_AreaLightPrim )
                {
                    Math::Vector3 min( -( dim.x * 0.5f ), -( dim.y * 0.5f ), 0.0f );
                    Math::Vector3 max(  ( dim.x * 0.5f ),  ( dim.y * 0.5f ), 0.0f );
                    Reflect::DangerousCast< PrimitiveCube >( m_AreaLightPrim )->SetBounds( min, max );
                    m_AreaLightPrim->Update();
                }
            }
        }
        else if( m_AreaLightType == Content::AreaLightTypes::Cylinder )
        {
            Content::CylinderLight* light = Reflect::ObjectCast< Content::CylinderLight >( areaLight );
            if( light )
            {
                light->m_Radius = dim.x;
                light->m_Length = dim.y;
                if( m_AreaLightPrim )
                {
                    PrimitiveCylinder* prim = Reflect::DangerousCast< PrimitiveCylinder >( m_AreaLightPrim );
                    prim->m_Radius = light->m_Radius;
                    prim->m_Length = light->m_Length * 2.0f;

                    m_AreaLightPrim->Update();
                }
            }
        }
    }
}

f32 Light::GetAreaLightSamplesPerMeter() const
{
    if( m_AreaLightType == Content::AreaLightTypes::None )
    {
        return 0.0f;
    }

    Content::AreaLight* areaLight = GetPackage< Content::Light >()->m_AreaLight;
    if( areaLight )
    {
        return areaLight->m_SamplesPerMeter;
    }
    return 0.0f;
}

void Light::SetAreaLightSamplesPerMeter( f32 samples )
{
    if( m_AreaLightType == Content::AreaLightTypes::None )
    {
        return;
    }

    Content::AreaLight* areaLight = GetPackage< Content::Light >()->m_AreaLight;
    if( areaLight )
    {
        areaLight->m_SamplesPerMeter = samples;
    }
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


const Content::V_ParametricKeyPtr& Light::GetColorAnimation() const
{
    return (Content::V_ParametricKeyPtr& )( GetPackage< Content::Light >()->m_AnimationColor );
}

void Light::SetColorAnimation( const Content::V_ParametricKeyPtr& animation )
{
    GetPackage< Content::Light >()->m_AnimationColor = (Content::V_ParametricColorKeyPtr& )( animation );
    m_Changed.Raise( LightChangeArgs( this ) );
}
const Content::V_ParametricKeyPtr& Light::GetIntensityAnimation() const
{
    return (Content::V_ParametricKeyPtr& )( GetPackage< Content::Light >()->m_AnimationIntensity );
}

void Light::SetIntensityAnimation( const Content::V_ParametricKeyPtr& animation )
{
    GetPackage< Content::Light >()->m_AnimationIntensity = (Content::V_ParametricColorKeyPtr& )( animation );
    m_Changed.Raise( LightChangeArgs( this ) );
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
    NOC_ASSERT( light1 && light2 );

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

void Light::OnSelectionHelper( Inspect::Button* button )
{
    SceneEditor* editor = wxGetApp().GetSceneEditor();
    Luna::Scene* scene = editor->GetSceneManager()->GetCurrentScene();
    if( scene )
    {
        SelectionDataObject* selectionData = static_cast<SelectionDataObject*>( button->GetClientData() );
        if( selectionData )
        {

            OS_SelectableDumbPtr lightSelection;

            V_LightDumbPtr lights;
            scene->GetAll< Luna::Light >( lights );

            OS_SelectableDumbPtr& selection = selectionData->m_Selection;

            OS_SelectableDumbPtr::Iterator selItor = selection.Begin();
            OS_SelectableDumbPtr::Iterator selEnd  = selection.End();

            for( ; selItor != selEnd; ++selItor )
            {
                Luna::Light* selLight = Reflect::ObjectCast< Luna::Light >( *selItor );

                if( selLight )
                {
                    Content::Light* contentLight = selLight->GetPackage< Content::Light >();

                    if( contentLight )
                    {
                        V_LightDumbPtr::iterator itor = lights.begin();
                        V_LightDumbPtr::iterator end  = lights.end();

                        for( ; itor != end; ++itor )
                        {
                            if( CompareLights( contentLight, (*itor)->GetPackage< Content::Light >(), selLight->m_SelectionHelper ) )
                            {
                                lightSelection.Append( *itor );
                            }
                        }
                    }
                }
            }
            editor->PostCommand( new SceneSelectCommand( scene, lightSelection ) );
        }
    }
}
