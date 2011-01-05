/*#include "Precompile.h"*/
#include "Light.h"

#include "Foundation/Component/ComponentHandle.h"

#include "Pipeline/SceneGraph/Scene.h"
#include "Pipeline/SceneGraph/SceneManager.h"
#include "Pipeline/SceneGraph/InstanceType.h"
#include "Pipeline/SceneGraph/PropertiesGenerator.h"
#include "Pipeline/SceneGraph/PrimitiveSphere.h"
#include "Pipeline/SceneGraph/PrimitivePointer.h"
#include "Pipeline/SceneGraph/PrimitiveCube.h"
#include "Pipeline/SceneGraph/PrimitiveCircle.h"
#include "Pipeline/SceneGraph/PrimitiveCylinder.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_CLASS(Light);

D3DMATERIAL9 Light::s_Material;

void Light::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    comp.AddField( &Light::m_Color, "m_Color" );
}

void Light::InitializeType()
{
    Reflect::RegisterClassType< Light >( TXT( "SceneGraph::Light" ) );

    ZeroMemory(&s_Material, sizeof(s_Material));

    PropertiesGenerator::InitializePanel( TXT( "Light" ), CreatePanelSignature::Delegate( &Light::CreatePanel ) );
}

void Light::CleanupType()
{
    Reflect::UnregisterClassType< Light >();
}

Light::Light()
{

}

Light::~Light()
{

}

int32_t Light::GetImageIndex() const
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
                InstanceType* type = Reflect::AssertCast<InstanceType>(m_NodeType);
                AlignedBox box (type->GetPointer()->GetBounds());

                Scale scale;
                Matrix3 rotate;
                Vector3 translate;
                m_InverseGlobalTransform.Decompose (scale, rotate, translate);

                // this will compensate for the normalized render of the pointer
                box.Transform (Matrix4 (scale));
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
    HierarchyNode::Render( render );
}

void Light::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const Light* light = Reflect::ConstAssertCast<Light>( object );

    const InstanceType* type = Reflect::ConstAssertCast<InstanceType>( light->GetNodeType() );

    light->SetMaterial( type->GetMaterial() );

    // draw type pointer
    type->GetPointer()->Draw( args );
}

bool Light::Pick( PickVisitor* pick )
{
    InstanceType* type = Reflect::AssertCast<InstanceType>(m_NodeType);

    pick->SetCurrentObject (this, pick->State().m_Matrix.Normalized());

    if (type->GetPointer()->Pick(pick))
    {
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Light.
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
    args.m_Generator->PushContainer( TXT( "Light" ) );
    {
        args.m_Generator->PushContainer();
        {
            const tstring helpText = TXT( "Determines the color for the currently selected light." );
            args.m_Generator->AddLabel( TXT( "Color" ) )->a_HelpText.Set( helpText );
            args.m_Generator->AddColorPicker<Light, Color3>( args.m_Selection, &Light::GetColor, &Light::SetColor )->a_HelpText.Set( helpText );
        }
        args.m_Generator->Pop();
    }
    args.m_Generator->Pop();
}

Color3 Light::GetColor() const
{
    return m_Color;
}

void Light::SetColor( Color3 color )
{
    m_Color = color;
}