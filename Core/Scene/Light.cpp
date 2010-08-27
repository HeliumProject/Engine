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