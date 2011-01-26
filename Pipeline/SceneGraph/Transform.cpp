/*#include "Precompile.h"*/
#include "Pipeline/SceneGraph/Transform.h"

#include "Foundation/Math/EulerAngles.h"
#include "Foundation/Math/Constants.h"

#include "Pipeline/SceneGraph/Manipulator.h"
#include "Foundation/Undo/PropertyCommand.h"
#include "PrimitiveAxes.h"

#include "Pipeline/SceneGraph/Scene.h"
#include "HierarchyNodeType.h"

#include "Color.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT( SceneGraph::Transform );

struct ScaleColorInfo
{
    D3DCOLORVALUE m_StartColor;
    D3DCOLORVALUE m_EndColor;
    float32_t m_ScaleMin;
    float32_t m_ScaleMax;
};

void Transform::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( (Vector3 Transform::*)&Transform::m_Scale,   TXT( "m_Scale" ) );
    comp.AddField( (Vector3 Transform::*)&Transform::m_Rotate,  TXT( "m_Rotate" ) );
    comp.AddField( &Transform::m_Translate,                     TXT( "m_Translate" ) );

    comp.AddField( &Transform::m_ObjectTransform,               TXT( "m_ObjectTransform" ) );
    comp.AddField( &Transform::m_GlobalTransform,               TXT( "m_GlobalTransform" ) );
    comp.AddField( &Transform::m_InheritTransform,              TXT( "m_InheritTransform" ) );
}

void Transform::InitializeType()
{
    Reflect::RegisterClassType< SceneGraph::Transform >( TXT( "SceneGraph::Transform" ) );

    PropertiesGenerator::InitializePanel( TXT( "Transform" ), CreatePanelSignature::Delegate( &Transform::CreatePanel ));
}

void Transform::CleanupType()
{
    Reflect::UnregisterClassType< SceneGraph::Transform >();
}

Transform::Transform()
: m_InheritTransform( true )
, m_BindIsDirty( true )
{

}

Transform::~Transform()
{

}

void Transform::Initialize()
{
    Base::Initialize();

    SceneGraph::PrimitiveAxes* axes = static_cast< SceneGraph::PrimitiveAxes* >( m_Owner->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::TransformAxes ) );
    m_ObjectBounds.minimum = Vector3(-axes->m_Length, -axes->m_Length, -axes->m_Length);
    m_ObjectBounds.maximum = Vector3(axes->m_Length, axes->m_Length, axes->m_Length);
}

SceneGraph::Transform* Transform::GetTransform()
{
    return this;
}

const SceneGraph::Transform* Transform::GetTransform() const
{
    return this;
}

Scale Transform::GetScale() const
{
    return m_Scale;
}

void Transform::SetScale( const Scale& value )
{
    m_Scale = value;

    Dirty();
}

Vector3 Transform::GetScalePivot() const 
{
    return Vector3::Zero;
}

void Transform::SetScalePivot( const Vector3& value )
{

}

EulerAngles Transform::GetRotate() const
{
    return m_Rotate;
}

void Transform::SetRotate( const EulerAngles& value )
{
    m_Rotate = value;

    Dirty();
}

Vector3 Transform::GetRotatePivot() const
{
    return Vector3::Zero;
}

void Transform::SetRotatePivot( const Vector3& value )
{

}

Vector3 Transform::GetTranslate() const
{
    return m_Translate;
}

void Transform::SetTranslate( const Vector3& value )
{
    m_Translate = value;

    Dirty();
}

Vector3 Transform::GetTranslatePivot() const
{
    return Vector3::Zero;
}

void Transform::SetTranslatePivot( const Vector3& value )
{

}

void Transform::SetObjectTransform( const Matrix4& transform )
{
    Scale scale;
    EulerAngles rotate;
    Vector3 translate;
    transform.Decompose( scale, rotate, translate );

    m_Scale = scale;
    m_Rotate = rotate;
    m_Translate = translate;
    m_ObjectTransform = transform;
}

void Transform::SetGlobalTransform( const Matrix4& transform )
{
    m_GlobalTransform = transform;

    ComputeObjectComponents();
}

Matrix4 Transform::GetBindTransform() const
{
    return m_BindTransform;
}

Matrix4 Transform::GetInverseBindTransform() const
{
    return m_InverseBindTransform;
}

bool Transform::GetInheritTransform() const
{
    return m_InheritTransform;
}

void Transform::SetInheritTransform(bool inherit)
{
    m_InheritTransform = inherit;

    ComputeObjectComponents();
}

Matrix4 Transform::GetScaleComponent() const
{
    return Matrix4 (m_Scale);
}

Matrix4 Transform::GetRotateComponent() const
{
    return Matrix4 (m_Rotate);
}

Matrix4 Transform::GetTranslateComponent() const
{
    return Matrix4 (m_Translate);
}

Undo::CommandPtr Transform::ResetTransform()
{
    Undo::CommandPtr command = new Undo::PropertyCommand<Matrix4>( new Helium::MemberProperty<SceneGraph::Transform, Matrix4> (this, &Transform::GetObjectTransform, &Transform::SetObjectTransform) );

    m_Scale = Scale::Identity;
    m_Rotate = EulerAngles::Zero;
    m_Translate = Vector3::Zero;

    Dirty();

    return command;
}

Undo::CommandPtr Transform::ComputeObjectComponents()
{
    Undo::CommandPtr command = ResetTransform();

    Scale scale;
    EulerAngles rotate;
    Vector3 translate;
    if (GetInheritTransform())
    {
        // compute the new object space transform from our global transform and our parent's inverse
        Matrix4 objectTransform = (m_GlobalTransform * m_Parent->GetTransform()->GetInverseGlobalTransform());

        // decompose into the new set of components
        objectTransform.Decompose(scale, rotate, translate);
    }
    else
    {
        // we are not inheriting our parent's transform, use the global transform for our components
        m_GlobalTransform.Decompose(scale, rotate, translate);
    }

    m_Scale = scale;
    m_Rotate = rotate;
    m_Translate = translate;

    return command;
}

Undo::CommandPtr Transform::CenterTransform()
{
    // copy global matrix
    Matrix4 transform = m_GlobalTransform;

    // reset the transform state
    Undo::CommandPtr command = ResetTransform();

    // set the global matrix (will re-localize)
    SetGlobalTransform(transform);

    return command;
}

void Transform::Evaluate(GraphDirection direction)
{
    SCENE_GRAPH_EVALUATE_SCOPE_TIMER( ("") );

    switch (direction)
    {
    case GraphDirections::Downstream:
        {
            {
                SCENE_GRAPH_EVALUATE_SCOPE_TIMER( ("Compose Local Matrices") );

                //
                // Compute Local Transform
                //

                m_ObjectTransform = GetScaleComponent() * GetRotateComponent() * GetTranslateComponent();
            }


            {
                SCENE_GRAPH_EVALUATE_SCOPE_TIMER( ("Compute Global Matrices") );

                //
                // Compute Global Transform
                //

                if (m_Parent == NULL || !GetInheritTransform())
                {
                    m_GlobalTransform = m_ObjectTransform;
                }
                else
                {
                    m_GlobalTransform = m_ObjectTransform * m_Parent->GetTransform()->GetGlobalTransform();
                }
            }


            {
                SCENE_GRAPH_EVALUATE_SCOPE_TIMER( ("Compute Inverse Matrices") );

                //
                // Compute Inverses
                //

                m_InverseObjectTransform = m_ObjectTransform;
                m_InverseObjectTransform.Invert();

                m_InverseGlobalTransform = m_GlobalTransform;
                m_InverseGlobalTransform.Invert();
            }


            //
            // Compute Object and Global Bind Transform, if Dirty
            //

            if (m_BindIsDirty)
            {
                SCENE_GRAPH_EVALUATE_SCOPE_TIMER( ("Compute Bind Matrix and Inverse") );

                if (m_Parent == NULL)
                    m_BindTransform = m_ObjectTransform;
                else
                    m_BindTransform = m_ObjectTransform * m_Parent->GetTransform()->GetBindTransform();

                m_InverseBindTransform = m_BindTransform;
                m_InverseBindTransform.Invert();

                m_BindIsDirty = false;
            }

            break;
        }
    }

    Base::Evaluate(direction);
}

void Transform::Render( RenderVisitor* render )
{
#ifdef DRAW_TRANFORMS
    RenderEntry* entry = render->Allocate(this);

    entry->m_Location = render->State().m_Matrix;
    entry->m_Center = m_ObjectBounds.Center();

    if (IsSelected() && m_Scene->IsCurrent())
    {
        entry->m_Draw = &Transform::DrawSelected;
    }
    else
    {
        entry->m_Draw = &Transform::DrawNormal;
    }
#endif

    Base::Render( render );
}

void Transform::DrawNormal( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const SceneGraph::HierarchyNode* node = Reflect::AssertCast<SceneGraph::HierarchyNode>( object );

    node->GetOwner()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::TransformAxes )->Draw( args );
}

void Transform::DrawSelected( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const SceneGraph::HierarchyNode* node = Reflect::AssertCast<SceneGraph::HierarchyNode>( object );

    node->GetOwner()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::SelectedAxes )->Draw( args );
}

bool Transform::Pick( PickVisitor* pick )
{
#ifdef DRAW_TRANFORMS
    pick->SetCurrentObject (this, pick->State().m_Matrix);

    return m_Scene->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::TransformAxes )->Pick(pick);
#else
    return false;
#endif
}

void Transform::ConnectManipulator(ManiuplatorAdapterCollection* collection)
{
    switch (collection->GetMode())
    {
    case ManipulatorModes::Scale:
        {
            collection->AddManipulatorAdapter(new TransformScaleManipulatorAdapter (this));
            break;
        }

    case ManipulatorModes::ScalePivot:
        {
            collection->AddManipulatorAdapter(new TransformScalePivotManipulatorAdapter (this));
            break;
        }

    case ManipulatorModes::Rotate:
        {
            collection->AddManipulatorAdapter(new TransformRotateManipulatorAdapter (this));
            break;
        }

    case ManipulatorModes::RotatePivot:
        {
            collection->AddManipulatorAdapter(new TransformRotatePivotManipulatorAdapter (this));
            break;
        }

    case ManipulatorModes::Translate:
        {
            collection->AddManipulatorAdapter(new TransformTranslateManipulatorAdapter (this));
            break;
        }

    case ManipulatorModes::TranslatePivot:
        {
            collection->AddManipulatorAdapter(new TransformTranslatePivotManipulatorAdapter (this));
            break;
        }
    }
}

bool Transform::ValidatePanel(const tstring& name)
{
    if (name == TXT( "Transform" ) )
    {
        return true;
    }

    return Base::ValidatePanel(name);
}

void Transform::CreatePanel(CreatePanelArgs& args)
{
    args.m_Generator->PushContainer( TXT( "Transform" ) );

    Inspect::Label* label;
    Inspect::Value* value;
    {
        args.m_Generator->PushContainer( TXT( "Inherit Transform Control" ) );
        label = args.m_Generator->AddLabel( TXT( "Inherit Transform" ) );
        label->a_HelpText.Set( TXT( "Causes this node to inherit its transform from its parent node." ) );
        Inspect::CheckBox* checkBox = args.m_Generator->AddCheckBox<SceneGraph::Transform, bool>(args.m_Selection, &Transform::GetInheritTransform, &Transform::SetInheritTransform);
        checkBox->a_HelpText.Set( TXT( "Causes this node to inherit its transform from its parent node." ) );
        args.m_Generator->Pop();
    }

    {
        args.m_Generator->PushContainer( TXT( "Scale Container" ) );
        label = args.m_Generator->AddLabel( TXT( "Scale" ) );
        label->a_HelpText.Set( TXT( "Controls the scaling of this node in the x, y and z dimensions." ) );

        value = args.m_Generator->AddValue<SceneGraph::Transform, float32_t>(args.m_Selection, &Transform::GetScaleX, &Transform::SetScaleX);
        value->a_HelpText.Set( TXT( "Controls the scaling of this node in the x dimension." ) );

        value = args.m_Generator->AddValue<SceneGraph::Transform, float32_t>(args.m_Selection, &Transform::GetScaleY, &Transform::SetScaleY);
        value->a_HelpText.Set( TXT( "Controls the scaling of this node in the y dimension." ) );

        value = args.m_Generator->AddValue<SceneGraph::Transform, float32_t>(args.m_Selection, &Transform::GetScaleZ, &Transform::SetScaleZ);
        value->a_HelpText.Set( TXT( "Controls the scaling of this node in the z dimension." ) );

        args.m_Generator->Pop();
    }

    {
        args.m_Generator->PushContainer( TXT( "Rotate Container" ) );
        label = args.m_Generator->AddLabel( TXT( "Rotate" ) );
        label->a_HelpText.Set( TXT( "Controls the rotation of this node about its x, y and z axes." ) );

        value = args.m_Generator->AddValue<SceneGraph::Transform, float32_t>(args.m_Selection, &Transform::GetRotateX, &Transform::SetRotateX);
        value->a_HelpText.Set( TXT( "Controls the rotation of this node about its x axis." ) );

        value = args.m_Generator->AddValue<SceneGraph::Transform, float32_t>(args.m_Selection, &Transform::GetRotateY, &Transform::SetRotateY);
        value->a_HelpText.Set( TXT( "Controls the rotation of this node about its y axis." ) );

        value = args.m_Generator->AddValue<SceneGraph::Transform, float32_t>(args.m_Selection, &Transform::GetRotateZ, &Transform::SetRotateZ);
        value->a_HelpText.Set( TXT( "Controls the rotation of this node about its z axis." ) );

        args.m_Generator->Pop();
    }

    {
        args.m_Generator->PushContainer( TXT( "Translate Container" ) );
        label = args.m_Generator->AddLabel( TXT( "Translate" ) );
        label->a_HelpText.Set( TXT( "Controls the location of this node in space with respect to the origin." ) );

        value = args.m_Generator->AddValue<SceneGraph::Transform, float32_t>(args.m_Selection, &Transform::GetTranslateX, &Transform::SetTranslateX);
        value->a_HelpText.Set( TXT( "Controls the offset of this node from the origin along the x axis." ) );

        value = args.m_Generator->AddValue<SceneGraph::Transform, float32_t>(args.m_Selection, &Transform::GetTranslateY, &Transform::SetTranslateY);
        value->a_HelpText.Set( TXT( "Controls the offset of this node from the origin along the y axis." ) );

        value = args.m_Generator->AddValue<SceneGraph::Transform, float32_t>(args.m_Selection, &Transform::GetTranslateZ, &Transform::SetTranslateZ);
        value->a_HelpText.Set( TXT( "Controls the offset of this node from the origin along the z axis." ) );

        args.m_Generator->Pop();
    }

    args.m_Generator->Pop();
}

float32_t Transform::GetScaleX() const
{
    return GetScale().x;
}

void Transform::SetScaleX(float32_t scale)
{
    Scale s = GetScale();
    s.x = scale;
    SetScale(s);
}

float32_t Transform::GetScaleY() const
{
    return GetScale().y;
}

void Transform::SetScaleY(float32_t scale)
{
    Scale s = GetScale();
    s.y = scale;
    SetScale(s);
}

float32_t Transform::GetScaleZ() const
{
    return GetScale().z;
}

void Transform::SetScaleZ(float32_t scale)
{
    Scale s = GetScale();
    s.z = scale;
    SetScale(s);
}

float32_t Transform::GetRotateX() const
{
    return GetRotate().angles.x * static_cast< float32_t >( HELIUM_RAD_TO_DEG );
}

void Transform::SetRotateX(float32_t rotate)
{
    Vector3 s = GetRotate().angles;
    s.x = rotate * static_cast< float32_t >( HELIUM_DEG_TO_RAD );
    SetRotate(EulerAngles (s, m_Rotate.order));
}

float32_t Transform::GetRotateY() const
{
    return GetRotate().angles.y * static_cast< float32_t >( HELIUM_RAD_TO_DEG );
}

void Transform::SetRotateY(float32_t rotate)
{
    Vector3 s = GetRotate().angles;
    s.y = rotate * static_cast< float32_t >( HELIUM_DEG_TO_RAD );
    SetRotate(EulerAngles (s, m_Rotate.order));
}

float32_t Transform::GetRotateZ() const
{
    return GetRotate().angles.z * static_cast< float32_t >( HELIUM_RAD_TO_DEG );
}

void Transform::SetRotateZ(float32_t rotate)
{
    Vector3 s = GetRotate().angles;
    s.z = rotate * static_cast< float32_t >( HELIUM_DEG_TO_RAD );
    SetRotate(EulerAngles (s, m_Rotate.order));
}

float32_t Transform::GetTranslateX() const
{
    return GetTranslate().x;
}

void Transform::SetTranslateX(float32_t translate)
{
    Vector3 s = GetTranslate();
    s.x = translate;
    SetTranslate(s);
}

float32_t Transform::GetTranslateY() const
{
    return GetTranslate().y;
}

void Transform::SetTranslateY(float32_t translate)
{
    Vector3 s = GetTranslate();
    s.y = translate;
    SetTranslate(s);
}

float32_t Transform::GetTranslateZ() const
{
    return GetTranslate().z;
}

void Transform::SetTranslateZ(float32_t translate)
{
    Vector3 s = GetTranslate();
    s.z = translate;
    SetTranslate(s);
}

Matrix4 TransformScaleManipulatorAdapter::GetFrame(ManipulatorSpace space)
{
    Matrix4 frame;

    switch (space)
    {
    case ManipulatorSpace::Object:
        {
            frame = m_Transform->GetGlobalTransform();
            frame.x *= 1.f / m_Transform->GetScale().x;
            frame.y *= 1.f / m_Transform->GetScale().y;
            frame.z *= 1.f / m_Transform->GetScale().z;
            break;
        }

    case ManipulatorSpace::Local:
        {
            frame = m_Transform->GetParentTransform();
            break;
        }

    case ManipulatorSpace::World:
        {
            // fall through, world space IS identity
            break;
        }
    }

    Vector3 pos (GetPivot());
    m_Transform->GetTranslateComponent().TransformVertex(pos);
    m_Transform->GetParentTransform().TransformVertex(pos);

    // move the translation to the correction location
    frame.t.x = pos.x;
    frame.t.y = pos.y;
    frame.t.z = pos.z;

    return frame;
}

Matrix4 TransformScaleManipulatorAdapter::GetObjectMatrix()
{
    return m_Transform->GetObjectTransform();
}

Matrix4 TransformScaleManipulatorAdapter::GetParentMatrix()
{
    return m_Transform->GetParentTransform();
}

Matrix4 TransformScalePivotManipulatorAdapter::GetFrame(ManipulatorSpace space)
{
    Matrix4 frame;

    switch (space)
    {
    case ManipulatorSpace::Object:
        {
            frame = m_Transform->GetGlobalTransform();
            frame.x *= 1.f / m_Transform->GetScale().x;
            frame.y *= 1.f / m_Transform->GetScale().y;
            frame.z *= 1.f / m_Transform->GetScale().z;
            break;
        }

    case ManipulatorSpace::Local:
        {
            frame = m_Transform->GetParentTransform();
            break;
        }

    case ManipulatorSpace::World:
        {
            // fall through, world space IS identity
            break;
        }
    }

    Vector3 pos (GetValue());
    m_Transform->GetTranslateComponent().TransformVertex(pos);
    m_Transform->GetParentTransform().TransformVertex(pos);

    // move the translation to the correction location
    frame.t.x = pos.x;
    frame.t.y = pos.y;
    frame.t.z = pos.z;

    return frame;
}

Matrix4 TransformScalePivotManipulatorAdapter::GetObjectMatrix()
{
    return Matrix4::Identity;
}

Matrix4 TransformScalePivotManipulatorAdapter::GetParentMatrix()
{
    return m_Transform->GetTranslateComponent() * m_Transform->GetParentTransform();
}

Matrix4 TransformRotateManipulatorAdapter::GetFrame(ManipulatorSpace space)
{
    Matrix4 frame;

    switch (space)
    {
    case ManipulatorSpace::Object:
        {
            frame = m_Transform->GetGlobalTransform();
            frame.x *= 1.f / m_Transform->GetScale().x;
            frame.y *= 1.f / m_Transform->GetScale().y;
            frame.z *= 1.f / m_Transform->GetScale().z;
            break;
        }

    case ManipulatorSpace::Local:
        {
            frame = m_Transform->GetParentTransform();
            break;
        }

    case ManipulatorSpace::World:
        {
            // fall through, world space IS identity
            break;
        }
    }

    Vector3 pos (GetPivot());
    m_Transform->GetTranslateComponent().TransformVertex(pos);
    m_Transform->GetParentTransform().TransformVertex(pos);

    // move the translation to the correction location
    frame.t.x = pos.x;
    frame.t.y = pos.y;
    frame.t.z = pos.z;

    return frame;
}

Matrix4 TransformRotateManipulatorAdapter::GetObjectMatrix()
{
    return m_Transform->GetObjectTransform();
}

Matrix4 TransformRotateManipulatorAdapter::GetParentMatrix()
{
    return m_Transform->GetParentTransform();
}

Matrix4 TransformRotatePivotManipulatorAdapter::GetFrame(ManipulatorSpace space)
{
    Matrix4 frame;

    switch (space)
    {
    case ManipulatorSpace::Object:
        {
            frame = m_Transform->GetGlobalTransform();
            frame.x *= 1.f / m_Transform->GetScale().x;
            frame.y *= 1.f / m_Transform->GetScale().y;
            frame.z *= 1.f / m_Transform->GetScale().z;
            break;
        }

    case ManipulatorSpace::Local:
        {
            frame = m_Transform->GetParentTransform();
            break;
        }

    case ManipulatorSpace::World:
        {
            // fall through, world space IS identity
            break;
        }
    }

    Vector3 pos (GetValue());
    m_Transform->GetTranslateComponent().TransformVertex(pos);
    m_Transform->GetParentTransform().TransformVertex(pos);

    // move the translation to the correction location
    frame.t.x = pos.x;
    frame.t.y = pos.y;
    frame.t.z = pos.z;

    return frame;
}

Matrix4 TransformRotatePivotManipulatorAdapter::GetObjectMatrix()
{
    return Matrix4::Identity;
}

Matrix4 TransformRotatePivotManipulatorAdapter::GetParentMatrix()
{
    return m_Transform->GetTranslateComponent() * m_Transform->GetParentTransform();
}

Matrix4 TransformTranslateManipulatorAdapter::GetFrame(ManipulatorSpace space)
{
    Matrix4 frame;

    switch (space)
    {
    case ManipulatorSpace::Object:
        {
            frame = m_Transform->GetGlobalTransform();
            frame.x *= 1.f / m_Transform->GetScale().x;
            frame.y *= 1.f / m_Transform->GetScale().y;
            frame.z *= 1.f / m_Transform->GetScale().z;
            break;
        }

    case ManipulatorSpace::Local:
        {
            frame = m_Transform->GetParentTransform();
            break;
        }

    case ManipulatorSpace::World:
        {
            // fall through, world space IS identity
            break;
        }
    }

    Vector3 pos (GetPivot());
    m_Transform->GetTranslateComponent().TransformVertex(pos);
    m_Transform->GetParentTransform().TransformVertex(pos);

    // move the translation to the correction location
    frame.t.x = pos.x;
    frame.t.y = pos.y;
    frame.t.z = pos.z;

    return frame;
}

Matrix4 TransformTranslateManipulatorAdapter::GetObjectMatrix()
{
    return m_Transform->GetObjectTransform();
}

Matrix4 TransformTranslateManipulatorAdapter::GetParentMatrix()
{
    return m_Transform->GetParentTransform();
}

Matrix4 TransformTranslatePivotManipulatorAdapter::GetFrame(ManipulatorSpace space)
{
    Matrix4 frame;

    switch (space)
    {
    case ManipulatorSpace::Object:
        {
            frame = m_Transform->GetGlobalTransform();
            frame.x *= 1.f / m_Transform->GetScale().x;
            frame.y *= 1.f / m_Transform->GetScale().y;
            frame.z *= 1.f / m_Transform->GetScale().z;
            break;
        }

    case ManipulatorSpace::Local:
        {
            frame = m_Transform->GetParentTransform();
            break;
        }

    case ManipulatorSpace::World:
        {
            // fall through, world space IS identity
            break;
        }
    }

    Vector3 pos (GetValue());
    m_Transform->GetTranslateComponent().TransformVertex(pos);
    m_Transform->GetParentTransform().TransformVertex(pos);

    // move the translation to the correction location
    frame.t.x = pos.x;
    frame.t.y = pos.y;
    frame.t.z = pos.z;

    return frame;
}

Matrix4 TransformTranslatePivotManipulatorAdapter::GetObjectMatrix()
{
    return Matrix4::Identity;
}

Matrix4 TransformTranslatePivotManipulatorAdapter::GetParentMatrix()
{
    return m_Transform->GetTranslateComponent() * m_Transform->GetParentTransform();
}