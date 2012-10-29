#include "SceneGraphPch.h"
#include "SceneGraph/Transform.h"

#include "Foundation/UndoQueue.h"

#include "Math/FpuEulerAngles.h"
#include "Math/Axes.h"

#include "SceneGraph/Manipulator.h"
#include "SceneGraph/PrimitiveAxes.h"
#include "SceneGraph/Scene.h"
#include "SceneGraph/Color.h"

REFLECT_DEFINE_ABSTRACT( Helium::SceneGraph::Transform );

using namespace Helium;
using namespace Helium::SceneGraph;

struct ScaleColorInfo
{
    SceneGraph::Color m_StartColor;
    SceneGraph::Color m_EndColor;
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

}

void Transform::CleanupType()
{

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

UndoCommandPtr Transform::ResetTransform()
{
    UndoCommandPtr command = new PropertyUndoCommand<Matrix4>( new Helium::MemberProperty<SceneGraph::Transform, Matrix4> (this, &Transform::GetObjectTransform, &Transform::SetObjectTransform) );

    m_Scale = Scale::Identity;
    m_Rotate = EulerAngles::Zero;
    m_Translate = Vector3::Zero;

    Dirty();

    return command;
}

UndoCommandPtr Transform::ComputeObjectComponents()
{
    UndoCommandPtr command = ResetTransform();

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

UndoCommandPtr Transform::CenterTransform()
{
    // copy global matrix
    Matrix4 transform = m_GlobalTransform;

    // reset the transform state
    UndoCommandPtr command = ResetTransform();

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
        GetOwner()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::SelectedAxes )->Draw( args );
    }
    else
    {
        GetOwner()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::TransformAxes )->Draw( args );
    }
#endif

    Base::Render( render );
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