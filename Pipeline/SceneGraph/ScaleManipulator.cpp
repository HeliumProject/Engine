#include "PipelinePch.h"
#include "ScaleManipulator.h"

#include "Pipeline/SceneGraph/Pick.h"
#include "Pipeline/SceneGraph/Viewport.h"
#include "Pipeline/SceneGraph/Camera.h"
#include "Color.h"

#include "PrimitiveAxes.h"
#include "PrimitiveCube.h"

#include "Pipeline/SceneGraph/Scene.h"
#include "Pipeline/SceneGraph/SceneManager.h"
#include "SceneSettings.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT(SceneGraph::ScaleManipulator);

void ScaleManipulator::InitializeType()
{
    Reflect::RegisterClassType< SceneGraph::ScaleManipulator >( TXT( "SceneGraph::ScaleManipulator" ) );
}

void ScaleManipulator::CleanupType()
{
    Reflect::UnregisterClassType< SceneGraph::ScaleManipulator >();
}

ScaleManipulator::ScaleManipulator( SettingsManager* settingsManager, const ManipulatorMode mode, SceneGraph::Scene* scene, PropertiesGenerator* generator)
: SceneGraph::TransformManipulator (mode, scene, generator)
, m_SettingsManager( settingsManager )
, m_Size( 0.3f )
, m_GridSnap( false )
, m_Distance( 1.0f )
{
    SceneSettings* settings = m_SettingsManager->GetSettings< SceneSettings >();
    m_Size = settings->ScaleManipulatorSize();
    m_GridSnap = settings->ScaleManipulatorGridSnap();
    m_Distance = settings->ScaleManipulatorDistance();

    m_Axes = new SceneGraph::PrimitiveAxes ();
    m_Axes->Update();

    m_Cube = new SceneGraph::PrimitiveCube ();
    m_Cube->SetSolid(true);
    m_Cube->Update();

    m_XCube = new SceneGraph::PrimitiveCube ();
    m_XCube->SetSolid(true);
    m_XCube->Update();

    m_YCube = new SceneGraph::PrimitiveCube ();
    m_YCube->SetSolid(true);
    m_YCube->Update();

    m_ZCube = new SceneGraph::PrimitiveCube ();
    m_ZCube->SetSolid(true);
    m_ZCube->Update();

    ResetSize();
}

ScaleManipulator::~ScaleManipulator()
{
    delete m_Axes;
    delete m_Cube;

    delete m_XCube;
    delete m_YCube;
    delete m_ZCube;
}

void ScaleManipulator::ResetSize()
{
    m_Axes->m_Length = 1.0f;
    m_Cube->SetRadius(0.05f);

    m_XCube->SetRadius(0.04f);
    m_XPosition = Vector3::BasisX;
    m_YCube->SetRadius(0.04f);
    m_YPosition = Vector3::BasisY;
    m_ZCube->SetRadius(0.04f);
    m_ZPosition = Vector3::BasisZ;
}

void ScaleManipulator::ScaleTo(float factor)
{
    ResetSize();

    m_Axes->m_Length *= factor;
    m_Axes->Update();

    m_Cube->ScaleRadius( factor );
    m_Cube->Update();

    m_XCube->ScaleRadius( factor );
    m_XCube->Update();
    m_XPosition *= factor;

    m_YCube->ScaleRadius( factor );
    m_YCube->Update();
    m_YPosition *= factor;

    m_ZCube->ScaleRadius( factor );
    m_ZCube->Update();
    m_ZPosition *= factor;
}

void ScaleManipulator::Evaluate()
{
    ScaleManipulatorAdapter* primary = PrimaryObject<ScaleManipulatorAdapter>();

    if (primary)
    {
        // get the transform for our object
        Matrix4 frame = primary->GetFrame(ManipulatorSpace::Object);

        // compute the scaling factor
        float factor = m_View->GetCamera()->ScalingTo(Vector3 (frame.t.x, frame.t.y, frame.t.z));

        // scale this
        ScaleTo(factor * m_Size);
    }
}

void ScaleManipulator::SetResult()
{
    if (m_Manipulated)
    {
        m_Manipulated = false;

        if (!m_ManipulationStart.empty())
        {
            ScaleManipulatorAdapter* primary = PrimaryObject<ScaleManipulatorAdapter>();

            if (primary != NULL)
            {
                if (!primary->GetNode()->GetOwner()->IsEditable())
                {
                    for each (ScaleManipulatorAdapter* accessor in CompleteSet<ScaleManipulatorAdapter>())
                    {
                        Vector3 val = m_ManipulationStart.find(accessor)->second.m_StartValue;

                        accessor->SetValue(Scale (val));
                    }
                }
                else
                {
                    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

                    for each (ScaleManipulatorAdapter* accessor in CompleteSet<ScaleManipulatorAdapter>())
                    {
                        // get current (resultant) value
                        Scale result (accessor->GetValue());

                        // set start value without undo support so its set for handling undo state
                        accessor->SetValue(Scale (m_ManipulationStart.find(accessor)->second.m_StartValue));

                        // set result with undo support
                        batch->Push ( accessor->SetValue(Scale (result)) );
                    }

                    m_Scene->Push( batch );
                }

                // apply modification
                primary->GetNode()->GetOwner()->Execute(false);
            }
        }
    }
}

void ScaleManipulator::Draw( DrawArgs* args )
{
    ScaleManipulatorAdapter* primary = PrimaryObject<ScaleManipulatorAdapter>();

    if (primary == NULL)
    {
        return;
    }

    ManipulationStart start;

    M_ManipulationStart::iterator found = m_ManipulationStart.find( primary );

    if (found != m_ManipulationStart.end())
    {
        start = found->second;
    }
    else
    {
        start.m_StartValue.x = 1.0f;
        start.m_StartValue.y = 1.0f;
        start.m_StartValue.z = 1.0f;
    }

    Scale offset;

    if (m_Manipulating)
    {
        offset.x = primary->GetValue().x / start.m_StartValue.x;
        offset.y = primary->GetValue().y / start.m_StartValue.y;
        offset.z = primary->GetValue().z / start.m_StartValue.z;

        if ((primary->GetValue().x / fabs(primary->GetValue().x)) != (start.m_StartValue.x / fabs(start.m_StartValue.x)))
        {
            offset.x *= -1.0f;
        }

        if ((primary->GetValue().y / fabs(primary->GetValue().y)) != (start.m_StartValue.y / fabs(start.m_StartValue.y)))
        {
            offset.y *= -1.0f;
        }

        if ((primary->GetValue().z / fabs(primary->GetValue().z)) != (start.m_StartValue.z / fabs(start.m_StartValue.z)))
        {
            offset.z *= -1.0f;
        }
    }

    // get the transform for our object
    Matrix4 frame = Matrix4 (offset) * primary->GetFrame(ManipulatorSpace::Object).Normalized();

    Scale inverse (1.0f / offset.x, 1.0f / offset.y, 1.0f / offset.z);

    AxesFlags parallelAxis = m_View->GetCamera()->ParallelAxis(frame, HELIUM_CRITICAL_DOT_PRODUCT);

#ifdef VIEWPORT_REFACTOR
    m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&frame);
    m_Axes->DrawAxes(args, (AxesFlags)(~parallelAxis & MultipleAxes::All));
#endif

    if (m_SelectedAxes == MultipleAxes::All)
    {
        m_AxisMaterial = SceneGraph::Color::YELLOW;
    }
    else
    {
        m_AxisMaterial = SceneGraph::Color::SKYBLUE;
    }

#ifdef VIEWPORT_REFACTOR
    m_View->GetDevice()->SetMaterial(&m_AxisMaterial);
    m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&(Matrix4 (inverse) * frame));
    m_Cube->Draw(args);
#endif

#ifdef VIEWPORT_REFACTOR
    if (parallelAxis != MultipleAxes::X)
    {
        SetAxisMaterial(MultipleAxes::X);
        m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&(Matrix4 (inverse) * Matrix4 (m_XPosition) * frame));
        m_XCube->Draw(args);
    }

    if (parallelAxis != MultipleAxes::Y)
    {
        SetAxisMaterial(MultipleAxes::Y);
        m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&(Matrix4 (inverse) * Matrix4 (m_YPosition) * frame));
        m_YCube->Draw(args);
    }

    if (parallelAxis != MultipleAxes::Z)
    {
        SetAxisMaterial(MultipleAxes::Z);
        m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&(Matrix4 (inverse) * Matrix4 (m_ZPosition) * frame));
        m_ZCube->Draw(args);
    }
#endif
}

bool ScaleManipulator::Pick( PickVisitor* pick )
{
    ScaleManipulatorAdapter* primary = PrimaryObject<ScaleManipulatorAdapter>();

    if (primary == NULL || pick->GetPickType() != PickTypes::Line)
    {
        return false;
    }

    // get the transform for our object
    Matrix4 frame = primary->GetFrame(ManipulatorSpace::Object).Normalized();

    // setup the pick object
    LinePickVisitor* linePick = dynamic_cast<LinePickVisitor*>(pick);
    linePick->SetCurrentObject (this, frame);
    linePick->ClearHits();

    AxesFlags parallelAxis = m_View->GetCamera()->ParallelAxis(frame, HELIUM_CRITICAL_DOT_PRODUCT);

    if (m_Cube->Pick(linePick))
    {
        m_SelectedAxes = MultipleAxes::All;
    }
    else
    {
        m_SelectedAxes = m_Axes->PickAxis (frame, linePick->GetWorldSpaceLine(), m_XCube->GetBounds().maximum.Length());

        //
        // Prohibit picking a parallel axis
        //

        if (m_SelectedAxes != MultipleAxes::None)
        {
            if (parallelAxis != MultipleAxes::None)
            {
                switch (m_SelectedAxes)
                {
                case MultipleAxes::X:
                    {
                        if (parallelAxis == MultipleAxes::X)
                        {
                            m_SelectedAxes = MultipleAxes::None;
                        }
                        break;
                    }

                case MultipleAxes::Y:
                    {
                        if (parallelAxis == MultipleAxes::Y)
                        {
                            m_SelectedAxes = MultipleAxes::None;
                        }
                        break;
                    }

                case MultipleAxes::Z:
                    {
                        if (parallelAxis == MultipleAxes::Z)
                        {
                            m_SelectedAxes = MultipleAxes::None;
                        }
                        break;
                    }
                }
            }
        }

        if (m_SelectedAxes == MultipleAxes::None)
        {
            linePick->SetCurrentObject (this, Matrix4 (m_XPosition) * frame);
            if (parallelAxis != MultipleAxes::X && m_XCube->Pick(linePick))
            {
                m_SelectedAxes = MultipleAxes::X;
            }
            else
            {
                linePick->SetCurrentObject (this, Matrix4 (m_YPosition) * frame);
                if (parallelAxis != MultipleAxes::Y && m_YCube->Pick(linePick))
                {
                    m_SelectedAxes = MultipleAxes::Y;
                }
                else
                {
                    linePick->SetCurrentObject (this, Matrix4 (m_ZPosition) * frame);
                    if (parallelAxis != MultipleAxes::Z && m_ZCube->Pick(linePick))
                    {
                        m_SelectedAxes = MultipleAxes::Z;
                    }
                }
            }
        }
    }

#pragma TODO("How to poll for ctrl button state? -Geoff")
    if (m_SelectedAxes != MultipleAxes::All && m_SelectedAxes != MultipleAxes::None && false /*wxIsCtrlDown()*/)
    {
        m_SelectedAxes = (AxesFlags)(~m_SelectedAxes & MultipleAxes::All);
    }

    if (m_SelectedAxes != MultipleAxes::None)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ScaleManipulator::MouseDown( const MouseButtonInput& e )
{
    AxesFlags previous = m_SelectedAxes;

    LinePickVisitor pick (m_View->GetCamera(), e.GetPosition().x, e.GetPosition().y);

    if (!Pick(&pick))
    {
        if (e.MiddleIsDown())
        {
            m_SelectedAxes = previous;
        }
        else
        {
            return false;
        }
    }

    if (!Base::MouseDown(e))
    {
        return false;
    }

    m_ManipulationStart.clear();

    for each (ScaleManipulatorAdapter* accessor in CompleteSet<ScaleManipulatorAdapter>())
    {
        ManipulationStart start;
        start.m_StartValue = Vector3 (accessor->GetValue().x, accessor->GetValue().y, accessor->GetValue().z);
        start.m_StartFrame = accessor->GetFrame(ManipulatorSpace::Object).Normalized();
        start.m_InverseStartFrame = start.m_StartFrame.Inverted();
        m_ManipulationStart.insert( M_ManipulationStart::value_type (accessor, start) );
    }

    return true;
}

void ScaleManipulator::MouseMove( const MouseMoveInput& e )
{
    Base::MouseMove(e);

    ScaleManipulatorAdapter* primary = PrimaryObject<ScaleManipulatorAdapter>();

    if (primary == NULL || m_ManipulationStart.empty() || (!m_Left && !m_Middle && !m_Right))
    {
        return;
    }

    const ManipulationStart& primaryStart = m_ManipulationStart.find( primary )->second;

    bool uniform = false;
    Vector3 reference;

    Vector3 startPoint;
    primaryStart.m_StartFrame.TransformVertex(startPoint);


    //
    // Compute our reference vector in global space, from the object
    //  This is an axis normal (direction) for single axis manipulation, or a plane normal for multi-axis manipulation
    //

    // start out with global manipulator axes
    reference = GetAxesNormal(m_SelectedAxes);

    // use local axes to manipulate
    primaryStart.m_StartFrame.Transform(reference, 0.f);

    if (m_SelectedAxes == MultipleAxes::All)
    {
        uniform = true;
    }
    else if (reference == Vector3::Zero)
    {
        return;
    }

    int sX = m_StartX, sY = m_StartY, eX = e.GetPosition().x, eY = e.GetPosition().y;

    if (uniform)
    {
        // make +x scale up
        sX = -sX;
        eX = -eX;
    }

    // Pick ray from our starting location
    Line startRay;
    m_View->GetCamera()->ViewportToLine( (float32_t)sX, (float32_t)sY, startRay);

    // Pick ray from our current location
    Line endRay;
    m_View->GetCamera()->ViewportToLine( (float32_t)eX, (float32_t)eY, endRay);

    // start and end points of the drag in world space, on the line or on the plane
    Vector3 p1, p2;

    if (!uniform)
    {
        //
        // Linear insersections of the rays with the selected reference line
        //

        if (!startRay.IntersectsLine(startPoint, startPoint + reference, &p1))
        {
            return;
        }

        if (!endRay.IntersectsLine(startPoint, startPoint + reference, &p2))
        {
            return;
        }
    }
    else
    {
        //
        // Planar intersections of the rays with the selected reference plane
        //

        if (!startRay.IntersectsPlane(Plane (startPoint, reference), &p1))
        {
            return;
        }

        if (!endRay.IntersectsPlane(Plane (startPoint, reference), &p2))
        {
            return;
        }
    }

    // bring into transform space
    primary->GetNode()->GetTransform()->GetInverseGlobalTransform().TransformVertex(p1);
    primary->GetNode()->GetTransform()->GetInverseGlobalTransform().TransformVertex(p2);

    // account for pivot point
    p1 = p1 - primary->GetPivot();
    p2 = p2 - primary->GetPivot();

    // scaling factor is the scaling from our starting point p1 to our ending point p2
    float scaling = p2.Dot(p1) / p1.Dot(p1);

    // start with identity
    Scale result;

    // multiply out scaling in along selected axes
    switch (m_SelectedAxes)
    {
    case MultipleAxes::X:
        {
            result.x = scaling;
            break;
        }

    case MultipleAxes::Y:
        {
            result.y = scaling;
            break;
        }

    case MultipleAxes::Z:
        {
            result.z = scaling;
            break;
        }

    case MultipleAxes::X | MultipleAxes::Y:
        {
            result.x = scaling;
            result.y = scaling;
            break;
        }

    case MultipleAxes::Y | MultipleAxes::Z:
        {
            result.y = scaling;
            result.z = scaling;
            break;
        }

    case MultipleAxes::Z | MultipleAxes::X:
        {
            result.z = scaling;
            result.x = scaling;
            break;
        }

    case MultipleAxes::All:
        {
            result.x = ((scaling - 1.0f) * 5.0f) + 1.0f;
            result.y = ((scaling - 1.0f) * 5.0f) + 1.0f;
            result.z = ((scaling - 1.0f) * 5.0f) + 1.0f;
            break;
        }
    }

    //
    // Set Value
    //

    for each (ScaleManipulatorAdapter* target in CompleteSet<ScaleManipulatorAdapter>())
    {
        const ManipulationStart& start = m_ManipulationStart.find( target )->second;

        Scale result (start.m_StartValue.x * result.x, start.m_StartValue.y * result.y, start.m_StartValue.z * result.z);

        if ( m_GridSnap )
        {
            if ( m_SelectedAxes == MultipleAxes::X )
            {
                float32_t delta = result.x - start.m_StartValue.x;
                delta /= m_Distance;
                delta = Round( delta );
                delta *= m_Distance;

                result.x = start.m_StartValue.x + delta;
                if ( fabs( result.x ) < 0.0000001f )
                {
                    return;
                }
            }
            else if ( m_SelectedAxes == MultipleAxes::Y )
            {
                float32_t delta = result.y - start.m_StartValue.y;
                delta /= m_Distance;
                delta = Round( delta );
                delta *= m_Distance;

                result.y = start.m_StartValue.y + delta;
                if ( fabs( result.y ) < 0.0000001f )
                {
                    return;
                }
            }
            else if ( m_SelectedAxes == MultipleAxes::Z )
            {
                float32_t delta = result.z - start.m_StartValue.z;
                delta /= m_Distance;
                delta = Round( delta );
                delta *= m_Distance;

                result.z = start.m_StartValue.z + delta;
                if ( fabs( result.z ) < 0.0000001f )
                {
                    return;
                }
            }
        }

        target->SetValue(result);
    }

    // apply modification
    primary->GetNode()->GetOwner()->Execute(true);

    // flag as changed
    m_Manipulated = true;
}

void ScaleManipulator::CreateProperties()
{
    Base::CreateProperties();

    m_Generator->PushContainer( TXT( "Scale" ) );
    {
        m_Generator->PushContainer();
        {
            m_Generator->AddLabel( TXT( "Grid Snap" ) );
            m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<SceneGraph::ScaleManipulator, bool> (this, &ScaleManipulator::GetGridSnap, &ScaleManipulator::SetGridSnap) );
        }
        m_Generator->Pop();

        m_Generator->PushContainer();
        {
            m_Generator->AddLabel( TXT( "Grid Distance" ) );
            m_Generator->AddValue<float>( new Helium::MemberProperty<SceneGraph::ScaleManipulator, float> (this, &ScaleManipulator::GetDistance, &ScaleManipulator::SetDistance) );
        }
        m_Generator->Pop();
    }
    m_Generator->Pop();
}

float32_t ScaleManipulator::GetSize() const
{
    return m_Size;
}

void ScaleManipulator::SetSize( float32_t size )
{
    m_Size = size;

    ManipulatorAdapter* primary = PrimaryObject<ManipulatorAdapter>();

    if (primary != NULL)
    {
        primary->GetNode()->GetOwner()->Execute(false);
    }

    SceneSettings* settings = m_SettingsManager->GetSettings< SceneSettings >();
    settings->RaiseChanged( settings->GetClass()->FindField( &ScaleManipulator::m_Size ) );
}

bool ScaleManipulator::GetGridSnap() const
{
    return m_GridSnap;
}

void ScaleManipulator::SetGridSnap( bool gridSnap )
{
    m_GridSnap = gridSnap;

    SceneSettings* settings = m_SettingsManager->GetSettings< SceneSettings >();
    settings->RaiseChanged( settings->GetClass()->FindField( &ScaleManipulator::m_GridSnap ) );
}

float ScaleManipulator::GetDistance() const
{
    return m_Distance;
}

void ScaleManipulator::SetDistance( float distance )
{
    m_Distance = distance;

    SceneSettings* settings = m_SettingsManager->GetSettings< SceneSettings >();
    settings->RaiseChanged( settings->GetClass()->FindField( &ScaleManipulator::m_Distance ) );
}
