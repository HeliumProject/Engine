#include "Precompile.h"
#include "TranslateManipulator.h"

#include "Pick.h"
#include "View.h"
#include "Camera.h"
#include "Color.h"

#include "PrimitiveAxes.h"
#include "PrimitiveCone.h"
#include "PrimitiveCircle.h"

#include "SceneManager.h"
#include "Scene.h"
#include "ScenePreferences.h"
#include "Mesh.h"

#include "Editor/Editor.h"

#include "Foundation/Math/AngleAxis.h"
#include "Foundation/Math/Utils.h"

using namespace Math;
using namespace Luna;

LUNA_DEFINE_TYPE(Luna::TranslateManipulator);


void TranslateManipulator::InitializeType()
{
    Reflect::RegisterClass< Luna::TranslateManipulator >( TXT( "Luna::TranslateManipulator" ) );
}

void TranslateManipulator::CleanupType()
{
    Reflect::UnregisterClass< Luna::TranslateManipulator >();
}

TranslateManipulator::TranslateManipulator(const ManipulatorMode mode, Luna::Scene* scene, Enumerator* enumerator)
: Luna::TransformManipulator (mode, scene, enumerator)
, m_HotSnappingMode (TranslateSnappingModes::None)
, m_ShowCones (true)
, m_Factor (1.f)
{
    Luna::ScenePreferences* prefs = SceneEditorPreferences();
    prefs->Get( prefs->TranslateManipulatorSize(), m_Size );
    prefs->GetEnum( prefs->TranslateManipulatorSpace(), m_Space );
    prefs->GetEnum( prefs->TranslateManipulatorSnappingMode(), m_SnappingMode );
    prefs->Get( prefs->TranslateManipulatorDistance(), m_Distance );
    prefs->Get( prefs->TranslateManipulatorLiveObjectsOnly(), m_LiveObjectsOnly );

    m_ShowCones = mode == ManipulatorModes::Translate;
    m_Factor = 1.0f;

    m_Axes = new Luna::PrimitiveAxes (m_Scene->GetView()->GetResources());
    m_Axes->Update();

    m_Ring = new Luna::PrimitiveCircle (m_Scene->GetView()->GetResources());
    m_Ring->Update();

    m_XCone = new Luna::PrimitiveCone (m_Scene->GetView()->GetResources());
    m_XCone->SetSolid( true );
    m_XCone->Update();

    m_YCone = new Luna::PrimitiveCone (m_Scene->GetView()->GetResources());
    m_YCone->SetSolid( true );
    m_YCone->Update();

    m_ZCone = new Luna::PrimitiveCone (m_Scene->GetView()->GetResources());
    m_ZCone->SetSolid( true );
    m_ZCone->Update();

    ResetSize();
}

TranslateManipulator::~TranslateManipulator()
{
    Luna::ScenePreferences* prefs = SceneEditorPreferences();
    prefs->Set( prefs->TranslateManipulatorSize(), m_Size );
    prefs->SetEnum( prefs->TranslateManipulatorSpace(), m_Space );
    prefs->SetEnum( prefs->TranslateManipulatorSnappingMode(), m_SnappingMode );
    prefs->Set( prefs->TranslateManipulatorDistance(), m_Distance );
    prefs->Set( prefs->TranslateManipulatorLiveObjectsOnly(), m_LiveObjectsOnly );

    delete m_Axes;
    delete m_Ring;

    delete m_XCone;
    delete m_YCone;
    delete m_ZCone;
}

TranslateSnappingMode TranslateManipulator::GetSnappingMode() const
{
    return m_HotSnappingMode != TranslateSnappingModes::None ? m_HotSnappingMode : m_SnappingMode;
}

void TranslateManipulator::ResetSize()
{
    m_Axes->m_Length = 1.0f;
    m_Ring->m_Radius = 0.1f;

    m_XCone->m_Length = 0.2f;
    m_XCone->m_Radius = 0.04f;
    m_XPosition = Vector3::BasisX;

    m_YCone->m_Length = 0.2f;
    m_YCone->m_Radius = 0.04f;
    m_YPosition = Vector3::BasisY;

    m_ZCone->m_Length = 0.2f;
    m_ZCone->m_Radius = 0.04f;
    m_ZPosition = Vector3::BasisZ;
}

void TranslateManipulator::ScaleTo(float f)
{
    ResetSize();

    m_Factor = f;

    m_Axes->m_Length *= m_Factor;
    m_Axes->Update();

    m_Ring->m_Radius *= m_Factor;
    m_Ring->Update();

    m_XCone->m_Length *= m_Factor;
    m_XCone->m_Radius *= m_Factor;
    m_XCone->Update();
    m_XPosition *= m_Factor;

    m_YCone->m_Length *= m_Factor;
    m_YCone->m_Radius *= m_Factor;
    m_YCone->Update();
    m_YPosition *= m_Factor;

    m_ZCone->m_Length *= m_Factor;
    m_ZCone->m_Radius *= m_Factor;
    m_ZCone->Update();
    m_ZPosition *= m_Factor;
}

void TranslateManipulator::Evaluate()
{
    TranslateManipulatorAdapter* primary = PrimaryObject<TranslateManipulatorAdapter>();

    if (primary)
    {
        // get the transform for our object
        Matrix4 frame = primary->GetFrame(m_Space);

        // compute the scaling factor
        float factor = m_View->GetCamera()->ScalingTo(Vector3 (frame.t.x, frame.t.y, frame.t.z));

        // scale this
        ScaleTo(factor * m_Size);
    }
}

void TranslateManipulator::SetResult()
{
    if (m_Manipulated)
    {
        m_Manipulated = false;

        if (!m_ManipulationStart.empty())
        {
            TranslateManipulatorAdapter* primary = PrimaryObject<TranslateManipulatorAdapter>();

            if (primary != NULL)
            {
                if (!primary->GetNode()->GetScene()->IsEditable())
                {
                    for each (TranslateManipulatorAdapter* accessor in CompleteSet<TranslateManipulatorAdapter>())
                    {
                        Vector3 val = m_ManipulationStart.find(accessor)->second.m_StartValue;

                        accessor->SetValue(val);
                    }
                }
                else
                {
                    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

                    for each (TranslateManipulatorAdapter* accessor in CompleteSet<TranslateManipulatorAdapter>())
                    {
                        // get current (resultant) value
                        Vector3 result = accessor->GetValue();

                        // set start value without undo support so its set for handling undo state
                        accessor->SetValue(m_ManipulationStart.find(accessor)->second.m_StartValue);

                        // set result with undo support
                        batch->Push( accessor->SetValue(result) );
                    }

                    m_Scene->Push( batch );
                }

                // apply modification
                primary->GetNode()->GetScene()->Execute(false);
            }
        }
    }
}

void TranslateManipulator::DrawPoints(AxesFlags axis)
{
    // this would probably cause a div by zero
    if ( m_Distance <= Math::ValueNearZero )
    {
        return;
    }

    // what does this do -geoff ?
    if ((int)(m_Factor / m_Distance) + 1 == 0)
    {
        return;
    }

    TranslateManipulatorAdapter* primary = PrimaryObject<TranslateManipulatorAdapter>();
    if (primary == NULL)
    {
        return;
    }

    // get the transform for our object
    Matrix4 frame = primary->GetFrame(m_Space);
    Matrix4 inverse = frame.Inverted();

    // our increment to the next grid point
    Vector3 basis;
    switch (axis)
    {
    case MultipleAxes::X:
        {
            basis = Vector3::BasisX;
            break;
        }

    case MultipleAxes::Y:
        {
            basis = Vector3::BasisY;
            break;
        }

    case MultipleAxes::Z:
        {
            basis = Vector3::BasisZ;
            break;
        }
    }

    // build vertex list
    bool done = false;
    float dist = 0.0f;
    std::vector<Position> vertices;
    while ( !done )
    {
        // build next grid point
        Vector3 next = basis * dist;

        // increment distance
        dist += m_Distance;

        if ( GetSnappingMode() == TranslateSnappingModes::Grid )
        {
            if (m_Space == ManipulatorSpaces::World)
            {
                // bring to global space
                frame.TransformVertex(next);
            }

            next.x /= m_Distance;
            next.x = (float)(Round(next.x));
            next.x *= m_Distance;

            next.y /= m_Distance;
            next.y = (float)(Round(next.y));
            next.y *= m_Distance;

            next.z /= m_Distance;
            next.z = (float)(Round(next.z));
            next.z *= m_Distance;

            if (m_Space == ManipulatorSpaces::World)
            {
                // bring back to local space
                inverse.TransformVertex(next);

                // project onto axis
                next = basis * next.Dot(basis);
            }
            else if (m_Space == ManipulatorSpaces::Local)
            {
                // the local value + pivot location
                Vector3 value = primary->GetValue() + primary->GetPivot();

                // if the parent and frame don't line up we need to fixup the local frame
                Matrix4 parentToFrame = primary->GetParentMatrix() * primary->GetFrame( ManipulatorSpaces::Local ).Inverted();
                parentToFrame.Transform(value, 0.f);

                // remove local translation
                next -= basis * value.Dot(basis);
            }
        }

        switch (axis)
        {
        case MultipleAxes::X:
            {
                f32 length = frame.x.Length();
                if ( !Math::IsValid( length ) || Math::Equal( length, 0.0f ) || next.x >= m_Factor / length )
                {
                    done = true;
                }

                if (next.x <= 0.0f)
                {
                    continue;
                }

                break;
            }

        case MultipleAxes::Y:
            {
                f32 length = frame.y.Length();
                if ( !Math::IsValid( length ) || Math::Equal( length, 0.0f ) || next.y >= m_Factor / length )
                {
                    done = true;
                }

                if (next.y <= 0.0f)
                {
                    continue;
                }

                break;
            }

        case MultipleAxes::Z:
            {
                f32 length = frame.z.Length();
                if ( !Math::IsValid( length ) || Math::Equal( length, 0.0f ) || next.z >= m_Factor / length )
                {
                    done = true;
                }

                if (next.z <= 0.0f)
                {
                    continue;
                }

                break;
            }
        }

        if (!done)
        {
            vertices.push_back( Position (next) );
        }
    }


    //
    // Render
    //

    if (!vertices.empty())
    {
        static float pointSize = 4.0f;
        m_View->GetDevice()->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
        m_View->GetDevice()->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&(pointSize)));
        m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&frame);
        m_View->GetDevice()->SetFVF(ElementFormats[ElementTypes::Position]);
        m_View->GetDevice()->DrawPrimitiveUP(D3DPT_POINTLIST, (UINT)vertices.size(), &vertices.front(), sizeof(Position));
        m_View->GetDevice()->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
        m_View->GetResources()->ResetState();
    }
}

void TranslateManipulator::Draw( DrawArgs* args )
{
    TranslateManipulatorAdapter* primary = PrimaryObject<TranslateManipulatorAdapter>();

    if (primary == NULL)
    {
        return;
    }

    // get the transform for our object
    Matrix4 frame = primary->GetFrame(m_Space).Normalized();
    Vector3 position = Vector3 (frame.t.x, frame.t.y, frame.t.z);

    AxesFlags parallelAxis = m_View->GetCamera()->ParallelAxis(frame, Math::CriticalDotProduct);

    m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&frame);

    m_Axes->DrawAxes(args, (Math::AxesFlags)(~parallelAxis & MultipleAxes::All));

    if (parallelAxis != MultipleAxes::X)
    {
        SetAxisMaterial(MultipleAxes::X);

        if (GetSnappingMode() == TranslateSnappingModes::Offset || GetSnappingMode() == TranslateSnappingModes::Grid)
        {
            DrawPoints(MultipleAxes::X);
        }

        if (m_ShowCones)
        {
            m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&(Matrix4::RotateY(Math::HalfPi) * Matrix4 (m_XPosition) * frame));
            m_XCone->Draw(args);
        }
    }

    if (parallelAxis != MultipleAxes::Y)
    {
        SetAxisMaterial(MultipleAxes::Y);

        if (GetSnappingMode() == TranslateSnappingModes::Offset || GetSnappingMode() == TranslateSnappingModes::Grid)
        {
            DrawPoints(MultipleAxes::Y);
        }

        if (m_ShowCones)
        {
            m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&(Matrix4::RotateX(-Math::HalfPi) * Matrix4 (m_YPosition) * frame));
            m_YCone->Draw(args);
        }
    }

    if (parallelAxis != MultipleAxes::Z)
    {
        SetAxisMaterial(MultipleAxes::Z);

        if (GetSnappingMode() == TranslateSnappingModes::Offset || GetSnappingMode() == TranslateSnappingModes::Grid)
        {
            DrawPoints(MultipleAxes::Z);
        }

        if (m_ShowCones)
        {
            m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&(Matrix4 (m_ZPosition) * frame));
            m_ZCone->Draw(args);
        }
    }

    if (m_SelectedAxes == MultipleAxes::All)
    {
        m_AxisMaterial.Ambient = Luna::Color::YELLOW;
    }
    else
    {
        m_AxisMaterial.Ambient = Luna::Color::LIGHTGRAY;
    }

    Vector3 cameraPosition;
    m_View->GetCamera()->GetPosition(cameraPosition);
    Matrix4 border = Matrix4 (AngleAxis::Rotation(Vector3::BasisX, cameraPosition - position)) * Matrix4 (position);

    // render sphere border m_Ring
    m_View->GetDevice()->SetMaterial(&m_AxisMaterial);
    m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&border);
    m_Ring->Draw(args);
}

bool TranslateManipulator::Pick( PickVisitor* pick )
{
    TranslateManipulatorAdapter* primary = PrimaryObject<TranslateManipulatorAdapter>();

    if (primary == NULL || pick->GetPickType() != PickTypes::Line )
    {
        return false;
    }

    // get the transform for our object
    Matrix4 frame = primary->GetFrame(m_Space).Normalized();

    // setup the pick object
    LinePickVisitor* linePick = dynamic_cast<LinePickVisitor*>(pick);
    linePick->SetCurrentObject (this, frame);
    linePick->ClearHits();

    AxesFlags parallelAxis = m_View->GetCamera()->ParallelAxis(frame, Math::CriticalDotProduct);

    if (linePick->PickPoint(Vector3::Zero, m_Ring->m_Radius))
    {
        m_SelectedAxes = MultipleAxes::All;
    }
    else
    {
        m_SelectedAxes = m_Axes->PickAxis (frame, linePick->GetWorldSpaceLine(), m_XCone->m_Radius);

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
                            m_SelectedAxes = MultipleAxes::None;
                        break;
                    }

                case MultipleAxes::Y:
                    {
                        if (parallelAxis == MultipleAxes::Y)
                            m_SelectedAxes = MultipleAxes::None;
                        break;
                    }

                case MultipleAxes::Z:
                    {
                        if (parallelAxis == MultipleAxes::Z)
                            m_SelectedAxes = MultipleAxes::None;
                        break;
                    }
                }
            }
        }

        if (m_SelectedAxes == MultipleAxes::None && m_ShowCones)
        {
            linePick->SetCurrentObject( this, Matrix4::RotateY(-Math::HalfPi) * Matrix4 (m_XPosition) * frame );
            if (parallelAxis != MultipleAxes::X && m_XCone->Pick(pick))
            {
                m_SelectedAxes = MultipleAxes::X;
            }
            else
            {
                linePick->SetCurrentObject( this, Matrix4::RotateX(Math::HalfPi) * Matrix4 (m_YPosition) * frame );
                if (parallelAxis != MultipleAxes::Y && m_YCone->Pick(pick))
                {
                    m_SelectedAxes = MultipleAxes::Y;
                }
                else
                {
                    linePick->SetCurrentObject( this, Matrix4 (m_ZPosition) * frame );
                    if (parallelAxis != MultipleAxes::Z && m_ZCone->Pick(pick))
                    {
                        m_SelectedAxes = MultipleAxes::Z;
                    }
                }
            }
        }

        m_Axes->SetRGB();

        if (m_SelectedAxes != MultipleAxes::None && !m_ShowCones)
        {
            m_Axes->SetColor(m_SelectedAxes, Luna::Color::ColorValueToColor(Luna::Color::YELLOW));
        }

        m_Axes->Update();
    }

    if (m_SelectedAxes != MultipleAxes::All && m_SelectedAxes != MultipleAxes::None && wxIsCtrlDown())
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

bool TranslateManipulator::MouseDown(wxMouseEvent& e)
{
    Math::AxesFlags previous = m_SelectedAxes;

    LinePickVisitor pick (m_View->GetCamera(), e.GetX(), e.GetY());
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

    if (!__super::MouseDown(e))
    {
        return false;
    }

    m_ManipulationStart.clear();

    for each (TranslateManipulatorAdapter* accessor in CompleteSet<TranslateManipulatorAdapter>())
    {
        ManipulationStart start;
        start.m_StartValue = accessor->GetValue();
        start.m_StartFrame = accessor->GetFrame(m_Space).Normalized();
        start.m_InverseStartFrame = start.m_StartFrame.Inverted();
        m_ManipulationStart.insert( M_ManipulationStart::value_type (accessor, start) );
    }

    return true;
}

void TranslateManipulator::MouseMove(wxMouseEvent& e)
{
    __super::MouseMove(e);

    TranslateManipulatorAdapter* primary = PrimaryObject<TranslateManipulatorAdapter>();

    if (primary == NULL || m_ManipulationStart.empty() || (!m_Left && !m_Middle && !m_Right))
    {
        return;
    }

    // our delta drag vector
    Vector3 drag;
    bool set = false;

    // the accessor data from the start of the drag
    const ManipulationStart& start = m_ManipulationStart.find( primary )->second;

    // our starting coordinate
    Vector3 startPoint = Vector3 (start.m_StartFrame.t.x, start.m_StartFrame.t.y, start.m_StartFrame.t.z);

    // compute drag vector
    switch ( GetSnappingMode() )
    {
    case TranslateSnappingModes::Surface:
    case TranslateSnappingModes::Object:
    case TranslateSnappingModes::Vertex:
        {
            // point to set
            Vector3 result;

            // construct pick ray
            FrustumLinePickVisitor pick( m_View->GetCamera(), e.GetX(), e.GetY() );

            // pick stuff in the scene
            primary->GetNode()->GetScene()->GetManager()->GetRootScene()->Pick(&pick);

            if (pick.HasHits())
            {
                V_PickHitSmartPtr sorted;
                PickHit::Sort(primary->GetNode()->GetScene()->GetView()->GetCamera(), pick.GetHits(), sorted, GetSnappingMode() == TranslateSnappingModes::Vertex ? PickSortTypes::Vertex : PickSortTypes::Intersection);

                V_PickHitSmartPtr::const_iterator itr = sorted.begin();
                V_PickHitSmartPtr::const_iterator end = sorted.end();
                for ( ; itr != end; ++itr )
                {
                    Luna::HierarchyNode* node = Reflect::ObjectCast<Luna::HierarchyNode>( (*itr)->GetObject() );

                    // don't use the object we are moving
                    if ( node && node == primary->GetNode() && !primary->AllowSelfSnap() )
                    {
                        continue;
                    }

                    // eliminate nodes that are not live when we are live snapping
                    if ( node && m_LiveObjectsOnly && !node->IsLive() )
                    {
                        continue;
                    }

                    // elminitate picks without normals when surface snapping
                    if ( GetSnappingMode() == TranslateSnappingModes::Surface && !(*itr)->HasNormal() )
                    {
                        continue;
                    }

                    // elimintate picks without vertices when vertex snapping
                    if ( GetSnappingMode() == TranslateSnappingModes::Vertex && !(*itr)->HasVertex() )
                    {
                        continue;
                    }

                    if (GetSnappingMode() == TranslateSnappingModes::Object)
                    {
                        Vector4 t = node->GetTransform()->GetGlobalTransform().t;
                        result.x = t.x;
                        result.y = t.y;
                        result.z = t.z;
                        set = true;
                    }
                    else if (GetSnappingMode() == TranslateSnappingModes::Vertex)
                    {
                        result = (*itr)->GetVertex();
                        set = true;
                    }
                    else if ((*itr)->HasIntersection())
                    {
                        result = (*itr)->GetIntersection();
                        set = true;
                    }

                    break;
                }
            }

            if (set)
            {
                drag = result - startPoint;
            }

            break;
        }
    }

    if ( !set )
    {
        //
        // Compute our reference vector in global space, from the object
        //  This is an axis normal (direction) for single axis manipulation, or a plane normal for multi-axis manipulation
        //

        Vector3 reference;

        // start out with global manipulator axes
        reference = GetAxesNormal(m_SelectedAxes);

        if (reference == Vector3::Zero)
        {
            return;
        }


        //
        // Setup reference vector
        //

        bool linear = false;

        switch (m_SelectedAxes)
        {
        case MultipleAxes::X:
        case MultipleAxes::Y:
        case MultipleAxes::Z:
            {
                linear = true;
                break;
            }
        }

        // if we are working on a particular axis
        if (m_SelectedAxes != MultipleAxes::All)
        {
            // if we are working in world space then save an invert and multiply here
            if (m_Space != ManipulatorSpaces::World)
            {
                // transform the reference vector by the manipulation frame, this orients the reference vector to the correct space
                start.m_StartFrame.Transform(reference, 0.f);
            }
        }

        // Pick ray from our starting location
        Line startRay;
        m_View->GetCamera()->ViewportToLine(m_StartX, m_StartY, startRay);

        // Pick ray from our current location
        Line endRay;
        m_View->GetCamera()->ViewportToLine(e.GetX(), e.GetY(), endRay);

        // start and end points of the drag in world space, on the line or on the plane
        Vector3 p1, p2;

        if (linear)
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

            // Currently we do planar tracking on the reference plane under the cursor, not the reference plane at the origin of the object.
            // At some point we need to compute the height above the reference plane the cursor was at when the the manipulator axes were hit tested.
            // If we don't have enough information here to do it here, then we may need to compute this height value during the pick routine.
#pragma TODO("Improve planar translation tracking")

            if (!startRay.IntersectsPlane(Plane (startPoint, reference), &p1))
            {
                return;
            }

            if (!endRay.IntersectsPlane(Plane (startPoint, reference), &p2))
            {
                return;
            }
        }

        // drag vector
        drag = p2 - p1;
        set = true;
    }

    NOC_ASSERT( set );


    //
    // Set Value
    //

    if ( drag.LengthSquared() > Math::ValueNearZero * Math::ValueNearZero )
    {
        Vector3 startValue = m_ManipulationStart.find(primary)->second.m_StartValue;
        Vector3 targetValue = startValue;

        Matrix4 object = primary->GetObjectMatrix();

        // the object object we get from the primary node is going to include the value we are trying to compute (when grid snapping in object space), so reset it
        object.t.x = startValue.x;
        object.t.y = startValue.y;
        object.t.z = startValue.z;

        // we don't want object space to be scaled by the object itself since its moving in its parent space, but along object axes
        if ( m_Space == ManipulatorSpaces::Object )
        {
            object.x *= 1.f / object.x.Length();
            object.y *= 1.f / object.y.Length();
            object.z *= 1.f / object.z.Length();
        }

        Matrix4 inverseObject = object;
        inverseObject.Invert();

        Matrix4 parent = primary->GetParentMatrix();
        Matrix4 inverseParent = parent;
        inverseParent.Invert();

        Matrix4 global = object * parent;
        Matrix4 inverseGlobal = global;
        inverseGlobal.Invert();

        switch (GetSnappingMode())
        {
        case TranslateSnappingModes::None:
            {
                // bring value into global space
                parent.TransformVertex(targetValue);

                // move
                targetValue += drag;

                // bring back into parent space
                inverseParent.TransformVertex(targetValue);

                break;
            }

        case TranslateSnappingModes::Surface:
        case TranslateSnappingModes::Object:
        case TranslateSnappingModes::Vertex:
            {
                switch (m_Space)
                {
                case ManipulatorSpaces::Object:
                    {
                        inverseObject.Transform(drag, 0.f);
                        break;
                    }

                case ManipulatorSpaces::Local:
                    {
                        inverseParent.Transform(drag, 0.f);
                        break;
                    }
                }

                switch (m_SelectedAxes)
                {
                case MultipleAxes::X:
                    {
                        drag.y = 0.f;
                        drag.z = 0.f;
                        break;
                    }

                case MultipleAxes::Y:
                    {
                        drag.x = 0.f;
                        drag.z = 0.f;
                        break;
                    }

                case MultipleAxes::Z:
                    {
                        drag.x = 0.f;
                        drag.y = 0.f;
                        break;
                    }

                case MultipleAxes::X | MultipleAxes::Y:
                    {
                        drag.z = 0.f;
                        break;
                    }

                case MultipleAxes::Y | MultipleAxes::Z:
                    {
                        drag.x = 0.f;
                        break;
                    }

                case MultipleAxes::Z | MultipleAxes::X:
                    {
                        drag.y = 0.f;
                        break;
                    }

                case MultipleAxes::All:
                    {
                        break;
                    }
                }

                switch (m_Space)
                {
                case ManipulatorSpaces::Object:
                    {
                        object.Transform(drag, 0.f);
                        break;
                    }

                case ManipulatorSpaces::Local:
                    {
                        parent.Transform(drag, 0.f);
                        break;
                    }
                }

                // bring value into global space
                parent.TransformVertex(targetValue);

                // move
                targetValue += drag;

                // bring back into parent space
                inverseParent.TransformVertex(targetValue);

                break;
            }

        case TranslateSnappingModes::Offset:
            {
                switch (m_Space)
                {
                case ManipulatorSpaces::Object:
                    {
                        inverseGlobal.Transform(drag, 0.f);
                        break;
                    }

                case ManipulatorSpaces::Local:
                    {
                        inverseParent.Transform(drag, 0.f);
                        break;
                    }
                }

                // fudge the drag normal vector's length to be a multiple of the distance to offset
                f32 length = drag.Length();
                length /= m_Distance;
                length = Round(length);
                length *= m_Distance;
                drag *= (1.f / drag.Length()) * length;

                switch (m_Space)
                {
                case ManipulatorSpaces::Object:
                    {
                        global.Transform(drag, 0.f);
                        break;
                    }

                case ManipulatorSpaces::Local:
                    {
                        parent.Transform(drag, 0.f);
                        break;
                    }
                }

                // bring value into global space
                parent.TransformVertex(targetValue);

                // move
                targetValue += drag;

                // bring back into parent space
                inverseParent.TransformVertex(targetValue);

                break;
            }

        case TranslateSnappingModes::Grid:
            {
#ifdef DEBUG_TRANSLATE_MANIP_GRID
                Log::Print("Value:   %08.3f, %08.3f, %08.3f\n", targetValue.x, targetValue.y, targetValue.z);
#endif

                targetValue += primary->GetPivot();

#ifdef DEBUG_TRANSLATE_MANIP_GRID
                Log::Print("Val&Piv: %08.3f, %08.3f, %08.3f\n", targetValue.x, targetValue.y, targetValue.z);
#endif

                // bring value into global space
                parent.TransformVertex(targetValue);

                // move
                targetValue += drag;

                // bring back into parent space
                inverseParent.TransformVertex(targetValue);

#ifdef DEBUG_TRANSLATE_MANIP_GRID
                Log::Print("Dragged: %08.3f, %08.3f, %08.3f\n", targetValue.x, targetValue.y, targetValue.z);
#endif

                // if the parent and frame don't line up we need to fixup the local frame, these matrices map those spaces
                Matrix4 parentToFrame;
                Matrix4 frameToParent;

                switch (m_Space)
                {
                case ManipulatorSpaces::Object:
                    {
                        inverseObject.TransformVertex(targetValue);
                        break;
                    }

                case ManipulatorSpaces::Local:
                    {
                        parentToFrame = parent * primary->GetFrame( ManipulatorSpaces::Local ).Inverted();
                        parentToFrame.Transform(targetValue, 0.f);
                        frameToParent = parentToFrame;
                        frameToParent.Invert();
                        break;
                    }

                case ManipulatorSpaces::World:
                    {
                        parent.TransformVertex(targetValue);
                        break;
                    }
                }

#ifdef DEBUG_TRANSLATE_MANIP_GRID
                Log::Print("PreSnap: %08.3f, %08.3f, %08.3f\n", targetValue.x, targetValue.y, targetValue.z);
#endif

                Vector3 startValInObjectSpace = startValue;
                inverseObject.TransformVertex(startValInObjectSpace);

                Vector3 pivotInObjectSpace = primary->GetPivot();
                inverseObject.TransformVertex(pivotInObjectSpace);

                if ( m_Space == ManipulatorSpaces::Object )
                {
                    targetValue -= startValInObjectSpace;
                    targetValue -= pivotInObjectSpace;
                }

                // fudge the target value vector's components to be a multiple of the distance of the grid interval

                if ((m_SelectedAxes & MultipleAxes::X) != MultipleAxes::None)
                {
                    targetValue.x /= m_Distance;
                    targetValue.x = Round(targetValue.x);
                    targetValue.x *= m_Distance;
                }

                if ((m_SelectedAxes & MultipleAxes::Y) != MultipleAxes::None)
                {
                    targetValue.y /= m_Distance;
                    targetValue.y = Round(targetValue.y);
                    targetValue.y *= m_Distance;
                }

                if ((m_SelectedAxes & MultipleAxes::Z) != MultipleAxes::None)
                {
                    targetValue.z /= m_Distance;
                    targetValue.z = Round(targetValue.z);
                    targetValue.z *= m_Distance;
                }

                if ( m_Space == ManipulatorSpaces::Object )
                {
                    targetValue += startValInObjectSpace;
                    targetValue += pivotInObjectSpace;
                }

#ifdef DEBUG_TRANSLATE_MANIP_GRID
                Log::Print("Snapped: %08.3f, %08.3f, %08.3f\n", targetValue.x, targetValue.y, targetValue.z);
#endif

                switch (m_Space)
                {
                case ManipulatorSpaces::Object:
                    {
                        object.TransformVertex(targetValue);
                        break;
                    }

                case ManipulatorSpaces::Local:
                    {
                        parentToFrame.Inverted().Transform(targetValue, 0.f);
                        break;
                    }

                case ManipulatorSpaces::World:
                    {
                        inverseParent.TransformVertex(targetValue);
                        break;
                    }
                }

                targetValue -= primary->GetPivot();

#ifdef DEBUG_TRANSLATE_MANIP_GRID
                Log::Print("Result:  %08.3f, %08.3f, %08.3f\n", targetValue.x, targetValue.y, targetValue.z);
#endif

                break;
            }
        }

        // set result (non-undo checked for preview purposes)
        primary->SetValue(targetValue);

        // compute the overall translation in parent space
        drag = targetValue - m_ManipulationStart.find(primary)->second.m_StartValue;

        // bring overall translation value into global space
        parent.Transform(drag, 0.f);

        for each (TranslateManipulatorAdapter* target in SecondarySet<TranslateManipulatorAdapter>())
        {
            Matrix4 parent = primary->GetParentMatrix();
            Matrix4 inverseParent = parent;
            inverseParent.Invert();

            // the starting point
            targetValue = m_ManipulationStart.find(target)->second.m_StartValue;

            // bring value into global space
            parent.TransformVertex(targetValue);

            // apply the translation
            targetValue += drag;

            // bring back into parent space
            inverseParent.TransformVertex(targetValue);

            // set result (non-undo checked for preview purposes)
            target->SetValue(targetValue);
        }

        // apply modification
        primary->GetNode()->GetScene()->Execute(true);

        // flag as changed
        m_Manipulated = true;
    }
}

void TranslateManipulator::KeyPress( wxKeyEvent& e )
{
    switch (e.GetKeyCode())
    {
    case wxT('L'):
        SetLiveObjectsOnly( !m_LiveObjectsOnly );
        break;

    default:
        __super::KeyPress( e );
        break;
    }
}

void TranslateManipulator::KeyDown( wxKeyEvent& e )
{
    TranslateSnappingMode mode = m_HotSnappingMode;

    switch (e.GetKeyCode())
    {
    case wxT('S'):
        m_HotSnappingMode = TranslateSnappingModes::Surface;
        break;

    case wxT('O'):
        m_HotSnappingMode = TranslateSnappingModes::Object;
        break;

    case wxT('V'):
        m_HotSnappingMode = TranslateSnappingModes::Vertex;
        break;

    case wxT('X'):
        m_HotSnappingMode = TranslateSnappingModes::Grid;
        break;

    default:
        __super::KeyDown( e );
        break;
    }

    if ( mode != m_HotSnappingMode )
    {
        m_Enumerator->GetContainer()->Read();

        TranslateManipulatorAdapter* primary = PrimaryObject<TranslateManipulatorAdapter>();

        if (primary != NULL)
        {
            primary->GetNode()->GetScene()->Execute(false);
        }
    }
}

void TranslateManipulator::KeyUp( wxKeyEvent& e )
{
    TranslateSnappingMode mode = m_HotSnappingMode;

    switch (e.GetKeyCode())
    {
    case wxT('S'):
    case wxT('O'):
    case wxT('V'):
    case wxT('X'):
        m_HotSnappingMode = TranslateSnappingModes::None;
        break;

    default:
        __super::KeyUp( e );
        break;
    }

    if ( mode != m_HotSnappingMode )
    {
        m_Enumerator->GetContainer()->Read();

        TranslateManipulatorAdapter* primary = PrimaryObject<TranslateManipulatorAdapter>();

        if (primary != NULL)
        {
            primary->GetNode()->GetScene()->Execute(false);
        }
    }
}

void TranslateManipulator::CreateProperties()
{
    __super::CreateProperties();

    m_Enumerator->PushPanel( TXT( "Translate" ), true);
    {
        m_Enumerator->PushContainer();
        {
            m_Enumerator->AddLabel( TXT( "Space" ) );
            Inspect::Choice* choice = m_Enumerator->AddChoice<int>( new Nocturnal::MemberProperty<Luna::TranslateManipulator, int> (this, &TranslateManipulator::GetSpace, &TranslateManipulator::SetSpace) );
            choice->SetDropDown( true );
            Inspect::V_Item items;

            {
                tostringstream str;
                str << ManipulatorSpaces::Object;
                items.push_back( Inspect::Item( TXT( "Object" ), str.str() ) );
            }

            {
                tostringstream str;
                str << ManipulatorSpaces::Local;
                items.push_back( Inspect::Item( TXT( "Local" ), str.str() ) );
            }

            {
                tostringstream str;
                str << ManipulatorSpaces::World;
                items.push_back( Inspect::Item( TXT( "World" ), str.str() ) );
            }

            choice->SetItems( items );
        }
        m_Enumerator->Pop();

        m_Enumerator->PushContainer();
        {
            m_Enumerator->AddLabel( TXT( "Snap to live objects only" ) );
            m_Enumerator->AddCheckBox<bool>( new Nocturnal::MemberProperty<Luna::TranslateManipulator, bool> (this, &TranslateManipulator::GetLiveObjectsOnly, &TranslateManipulator::SetLiveObjectsOnly) );
        }
        m_Enumerator->Pop();

        m_Enumerator->PushContainer();
        {
            m_Enumerator->AddLabel( TXT( "Surface Snap" ) );
            m_Enumerator->AddCheckBox<bool>( new Nocturnal::MemberProperty<Luna::TranslateManipulator, bool> (this, &TranslateManipulator::GetSurfaceSnap, &TranslateManipulator::SetSurfaceSnap) );
        }
        m_Enumerator->Pop();

        m_Enumerator->PushContainer();
        {
            m_Enumerator->AddLabel( TXT( "Object Snap" ) );
            m_Enumerator->AddCheckBox<bool>( new Nocturnal::MemberProperty<Luna::TranslateManipulator, bool> (this, &TranslateManipulator::GetObjectSnap, &TranslateManipulator::SetObjectSnap) );
        }
        m_Enumerator->Pop();

        m_Enumerator->PushContainer();
        {
            m_Enumerator->AddLabel( TXT( "Vertex Snap" ) );
            m_Enumerator->AddCheckBox<bool>( new Nocturnal::MemberProperty<Luna::TranslateManipulator, bool> (this, &TranslateManipulator::GetVertexSnap, &TranslateManipulator::SetVertexSnap) );
        }
        m_Enumerator->Pop();

        m_Enumerator->PushContainer();
        {
            m_Enumerator->AddLabel( TXT( "Offset Snap" ) );
            m_Enumerator->AddCheckBox<bool>( new Nocturnal::MemberProperty<Luna::TranslateManipulator, bool> (this, &TranslateManipulator::GetOffsetSnap, &TranslateManipulator::SetOffsetSnap) );
        }
        m_Enumerator->Pop();

        m_Enumerator->PushContainer();
        {
            m_Enumerator->AddLabel( TXT( "Grid Snap" ) );
            m_Enumerator->AddCheckBox<bool>( new Nocturnal::MemberProperty<Luna::TranslateManipulator, bool> (this, &TranslateManipulator::GetGridSnap, &TranslateManipulator::SetGridSnap) );
        }
        m_Enumerator->Pop();

        m_Enumerator->PushContainer();
        {
            m_Enumerator->AddLabel( TXT( "Distance" ) );
            m_Enumerator->AddValue<float>( new Nocturnal::MemberProperty<Luna::TranslateManipulator, float> (this, &TranslateManipulator::GetDistance, &TranslateManipulator::SetDistance) );
        }
        m_Enumerator->Pop();
    }
    m_Enumerator->Pop();
}

int TranslateManipulator::GetSpace() const
{
    return m_Space;
}

void TranslateManipulator::SetSpace(int space)
{
    m_Space = (ManipulatorSpace)space;

    TranslateManipulatorAdapter* primary = PrimaryObject<TranslateManipulatorAdapter>();

    if (primary != NULL)
    {
        primary->GetNode()->GetScene()->Execute(false);
    }
}

bool TranslateManipulator::GetLiveObjectsOnly() const
{
    return m_LiveObjectsOnly;
}

void TranslateManipulator::SetLiveObjectsOnly(bool snap)
{
    m_LiveObjectsOnly = snap;

    TranslateManipulatorAdapter* primary = PrimaryObject<TranslateManipulatorAdapter>();

    if (primary != NULL)
    {
        primary->GetNode()->GetScene()->Execute(false);
    }
}

bool TranslateManipulator::GetSurfaceSnap() const
{
    return m_SnappingMode == TranslateSnappingModes::Surface;
}

void TranslateManipulator::SetSurfaceSnap(bool snap)
{
    if (m_SnappingMode == TranslateSnappingModes::Surface != snap)
    {
        m_SnappingMode = snap ? TranslateSnappingModes::Surface : TranslateSnappingModes::None;

        if (m_SnappingMode == TranslateSnappingModes::Surface)
        {
            m_Enumerator->GetContainer()->Read();
        }

        TranslateManipulatorAdapter* primary = PrimaryObject<TranslateManipulatorAdapter>();

        if (primary != NULL)
        {
            primary->GetNode()->GetScene()->Execute(false);
        }
    }
}

bool TranslateManipulator::GetObjectSnap() const
{
    return m_SnappingMode == TranslateSnappingModes::Object;
}

void TranslateManipulator::SetObjectSnap(bool snap)
{
    if (m_SnappingMode == TranslateSnappingModes::Object != snap)
    {
        m_SnappingMode = snap ? TranslateSnappingModes::Object : TranslateSnappingModes::None;

        if (m_SnappingMode == TranslateSnappingModes::Object)
        {
            m_Enumerator->GetContainer()->Read();
        }

        TranslateManipulatorAdapter* primary = PrimaryObject<TranslateManipulatorAdapter>();

        if (primary != NULL)
        {
            primary->GetNode()->GetScene()->Execute(false);
        }
    }
}

bool TranslateManipulator::GetVertexSnap() const
{
    return m_SnappingMode == TranslateSnappingModes::Vertex;
}

void TranslateManipulator::SetVertexSnap(bool snap)
{
    if (m_SnappingMode == TranslateSnappingModes::Vertex != snap)
    {
        m_SnappingMode = snap ? TranslateSnappingModes::Vertex : TranslateSnappingModes::None;

        if (m_SnappingMode == TranslateSnappingModes::Vertex)
        {
            m_Enumerator->GetContainer()->Read();
        }

        TranslateManipulatorAdapter* primary = PrimaryObject<TranslateManipulatorAdapter>();

        if (primary != NULL)
        {
            primary->GetNode()->GetScene()->Execute(false);
        }
    }
}

bool TranslateManipulator::GetOffsetSnap() const
{
    return m_SnappingMode == TranslateSnappingModes::Offset;
}

void TranslateManipulator::SetOffsetSnap(bool snap)
{
    if (m_SnappingMode == TranslateSnappingModes::Offset != snap)
    {
        m_SnappingMode = snap ? TranslateSnappingModes::Offset : TranslateSnappingModes::None;

        if (m_SnappingMode == TranslateSnappingModes::Offset)
        {
            m_Enumerator->GetContainer()->Read();
        }

        TranslateManipulatorAdapter* primary = PrimaryObject<TranslateManipulatorAdapter>();

        if (primary != NULL)
        {
            primary->GetNode()->GetScene()->Execute(false);
        }
    }
}

bool TranslateManipulator::GetGridSnap() const
{
    return m_SnappingMode == TranslateSnappingModes::Grid;
}

void TranslateManipulator::SetGridSnap(bool snap)
{
    if (m_SnappingMode == TranslateSnappingModes::Grid != snap)
    {
        m_SnappingMode = snap ? TranslateSnappingModes::Grid : TranslateSnappingModes::None;

        if (m_SnappingMode == TranslateSnappingModes::Grid)
        {
            m_Enumerator->GetContainer()->Read();
        }

        TranslateManipulatorAdapter* primary = PrimaryObject<TranslateManipulatorAdapter>();

        if (primary != NULL)
        {
            primary->GetNode()->GetScene()->Execute(false);
        }
    }
}

float TranslateManipulator::GetDistance() const
{
    return m_Distance;
}

void TranslateManipulator::SetDistance(float distance)
{
    m_Distance = distance;

    TranslateManipulatorAdapter* primary = PrimaryObject<TranslateManipulatorAdapter>();

    if (primary != NULL)
    {
        primary->GetNode()->GetScene()->Execute(false);
    }
}
