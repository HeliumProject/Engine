/*#include "Precompile.h"*/
#include "CurveCreateTool.h"

#include "Pipeline/SceneGraph/CreateTool.h"
#include "Pipeline/SceneGraph/CurveControlPoint.h"
#include "Pipeline/SceneGraph/Pick.h"
#include "Pipeline/SceneGraph/Scene.h"
#include "Pipeline/SceneGraph/SceneManager.h"
#include "Pipeline/SceneGraph/HierarchyNodeType.h"

using namespace Helium;
using namespace Helium::SceneGraph;

CurveType CurveCreateTool::s_CurveType = CurveType::BSpline;
bool CurveCreateTool::s_SurfaceSnap = false;
bool CurveCreateTool::s_ObjectSnap = false;

REFLECT_DEFINE_ABSTRACT(SceneGraph::CurveCreateTool);

void CurveCreateTool::InitializeType()
{
    Reflect::RegisterClassType< SceneGraph::CurveCreateTool >( TXT( "SceneGraph::CurveCreateTool" ) );
}

void CurveCreateTool::CleanupType()
{
    Reflect::UnregisterClassType< SceneGraph::CurveCreateTool >();
}

CurveCreateTool::CurveCreateTool( SceneGraph::Scene* scene, PropertiesGenerator* generator )
: Tool( scene, generator )
, m_Instance( NULL )
, m_Created( false )
{
    CreateInstance( Vector3::Zero );
}

CurveCreateTool::~CurveCreateTool()
{
    if (m_Instance.ReferencesObject())
    {
        if ( m_Instance->GetChildren().Size() > 1 )
        {
            AddToScene();
        }
        else
        {
            // remove temp reference
            m_Scene->RemoveObject( m_Instance );
        }
    }

    m_Scene->Push( m_Scene->GetSelection().SetItems( m_Selection ) );
}

void CurveCreateTool::CreateInstance( const Vector3& position )
{
    if (m_Instance.ReferencesObject())
    {
        // remove temp reference
        m_Scene->RemoveObject( m_Instance );
    }

    m_Instance = new Curve();
    m_Instance->Initialize( m_Scene );
    m_Instance->SetSelected( true );
    m_Instance->SetTransient( true );
    m_Instance->SetCurveType( (int)s_CurveType );
    m_Scene->AddObject( m_Instance );

    CurveControlPointPtr point = new CurveControlPoint();
    point->Initialize( m_Scene );
    point->SetParent( m_Instance );
    point->SetPosition( position );
    point->SetTransient( true );
    m_Scene->AddObject( point );

    m_Instance->Evaluate( GraphDirections::Downstream );
}

void CurveCreateTool::PickPosition(int x, int y, Vector3 &position)
{
    FrustumLinePickVisitor pick (m_Scene->GetViewport()->GetCamera(), x, y);

    // pick in the world
    m_PickWorld.Raise( PickArgs( &pick ) );

    bool set = false;

    if (s_SurfaceSnap || s_ObjectSnap)
    {
        V_PickHitSmartPtr sorted;
        PickHit::Sort(m_Scene->GetViewport()->GetCamera(), pick.GetHits(), sorted, PickSortTypes::Intersection);

        V_PickHitSmartPtr::const_iterator itr = sorted.begin();
        V_PickHitSmartPtr::const_iterator end = sorted.end();
        for ( ; itr != end; ++itr )
        {
            // don't snap if we are surface snapping with no normal
            if ( s_SurfaceSnap && !(*itr)->HasNormal() )
            {
                continue;
            }

            if ( (*itr)->GetHitObject() != m_Instance )
            {
                SceneGraph::HierarchyNode* node = Reflect::ObjectCast<SceneGraph::HierarchyNode>( (*itr)->GetHitObject() );

                if ( s_ObjectSnap )
                {
                    Vector4 v = node->GetTransform()->GetGlobalTransform().t;
                    position.x = v.x;
                    position.y = v.y;
                    position.z = v.z;
                }
                else
                {
                    position = (*itr)->GetIntersection();
                }

                set = true;
                break;
            }
        }
    }

    if (!set)
    {
        // place the object on the camera plane
        m_Scene->GetViewport()->GetCamera()->ViewportToPlaneVertex( (float32_t)x, (float32_t)y, SceneGraph::CreateTool::s_PlaneSnap, position);
    }
}

void CurveCreateTool::AddToScene()
{
    if ( !m_Instance.ReferencesObject() )
    {
        return;
    }

    if ( !m_Scene->IsEditable()  )
    {
        return;
    }

    uint32_t countControlPoints = m_Instance->GetNumberControlPoints();
    if ( countControlPoints > 2 ) 
    {
        m_Instance->RemoveControlPointAtIndex( countControlPoints - 1 );
        m_Instance->Evaluate( GraphDirections::Downstream );
    }

    // remove temp reference
    m_Scene->RemoveObject( m_Instance );

    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    if ( !m_Created )
    {
        batch->Push( m_Scene->GetSelection().Clear() );
        m_Created = true;
    }

    m_Instance->SetTransient( false );

    for ( OS_HierarchyNodeDumbPtr::Iterator childItr = m_Instance->GetChildren().Begin(), childEnd = m_Instance->GetChildren().End(); childItr != childEnd; ++childItr )
    {
        (*childItr)->SetTransient( false );
    }

    // add the existence of this object to the batch
    batch->Push( new SceneNodeExistenceCommand( Undo::ExistenceActions::Add, m_Scene, m_Instance ) );

    // initialize
    m_Instance->Initialize( m_Scene );

    // center origin
    m_Instance->CenterTransform();

    // append instance to selection
    m_Selection.Append( m_Instance );

    // commit the changes
    m_Scene->Push( batch );
    m_Scene->Execute( false );

    m_Instance = NULL;
    CreateInstance( Vector3::Zero );
}

bool CurveCreateTool::AllowSelection()
{
    return false;
}

bool CurveCreateTool::MouseDown( const MouseButtonInput& e )
{
    if ( m_Instance.ReferencesObject() && m_Scene->IsEditable() )
    {
        Vector3 position;
        PickPosition( e.GetPosition().x, e.GetPosition().y, position );

        CurveControlPointPtr point = new CurveControlPoint();
        point->Initialize( m_Scene );
        point->SetParent( m_Instance );
        point->SetTransient( true );
        point->SetPosition( position );
        m_Scene->AddObject( point );

        m_Instance->Dirty();

        m_Scene->Execute( true );
    }

    return __super::MouseDown( e );
}

void CurveCreateTool::MouseMove( const MouseMoveInput& e )
{
    if ( m_Instance.ReferencesObject() )
    {
        uint32_t countControlPoints = m_Instance->GetNumberControlPoints();

        if ( countControlPoints > 0 )
        {
            Vector3 position;
            PickPosition( e.GetPosition().x, e.GetPosition().y, position );

            CurveControlPoint* current = m_Instance->GetControlPointByIndex( countControlPoints - 1 );
            current->SetPosition( position );

            m_Instance->Dirty();

            m_Scene->Execute( true );
        }
    }

    __super::MouseMove( e );
} 

void CurveCreateTool::KeyPress( const KeyboardInput& e )
{
    const int keyCode = e.GetKeyCode();

    switch( keyCode )
    { 
    case KeyCodes::Return:
        {
            AddToScene();
            break;
        }
    }

    __super::KeyPress( e );
}

void CurveCreateTool::CreateProperties()
{
    m_Generator->PushContainer( TXT( "Create Curve" ) );
    {
        m_Generator->PushContainer();
        {
            m_Generator->AddLabel( TXT( "Surface Snap" ) );   
            m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<SceneGraph::CurveCreateTool, bool> (this, &CurveCreateTool::GetSurfaceSnap, &CurveCreateTool::SetSurfaceSnap ) );
        }
        m_Generator->Pop();

        m_Generator->PushContainer();
        {
            m_Generator->AddLabel( TXT( "Object Snap" ) );   
            m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<SceneGraph::CurveCreateTool, bool> (this, &CurveCreateTool::GetObjectSnap, &CurveCreateTool::SetObjectSnap ) );
        }
        m_Generator->Pop();

        m_Generator->PushContainer();
        {
            m_Generator->AddLabel( TXT( "Plane Snap" ) );
            Inspect::Choice* choice = m_Generator->AddChoice<int>( new Helium::MemberProperty<SceneGraph::CurveCreateTool, int> (this, &CurveCreateTool::GetPlaneSnap, &CurveCreateTool::SetPlaneSnap) );
            choice->a_IsDropDown.Set( true );
            std::vector< Inspect::ChoiceItem > items;

            {
                tostringstream str;
                str << IntersectionPlanes::Viewport;
                items.push_back( Inspect::ChoiceItem( TXT( "Viewport" ), str.str() ) );
            }

            {
                tostringstream str;
                str << IntersectionPlanes::Ground;
                items.push_back( Inspect::ChoiceItem( TXT( "Ground" ), str.str() ) );
            }

            choice->a_Items.Set( items );
        }
        m_Generator->Pop();

        m_Generator->PushContainer();
        {
            m_Generator->AddLabel( TXT( "Curve Type" ) );
            Inspect::Choice* choice = m_Generator->AddChoice<int>( new Helium::MemberProperty<SceneGraph::CurveCreateTool, int> (this, &CurveCreateTool::GetCurveType, &CurveCreateTool::SetCurveType ) );
            choice->a_IsDropDown.Set( true );
            std::vector< Inspect::ChoiceItem > items;

            {
                tostringstream str;
                str << CurveType::Linear;
                items.push_back( Inspect::ChoiceItem( TXT( "Linear" ), str.str() ) );
            }

            {
                tostringstream str;
                str << CurveType::BSpline;
                items.push_back( Inspect::ChoiceItem( TXT( "BSpline" ), str.str() ) );
            }

            {
                tostringstream str;
                str << CurveType::CatmullRom;
                items.push_back( Inspect::ChoiceItem( TXT( "Catmull-Rom" ), str.str() ) );
            }

            choice->a_Items.Set( items );

        }
        m_Generator->Pop();

        m_Generator->PushContainer();
        {
            m_Generator->AddLabel( TXT( "Closed" ) );
            m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<SceneGraph::CurveCreateTool, bool> (this, &CurveCreateTool::GetClosed, &CurveCreateTool::SetClosed ) );
        }
        m_Generator->Pop();

    }
    m_Generator->Pop();
}

bool CurveCreateTool::GetSurfaceSnap() const
{
    return s_SurfaceSnap;
}

void CurveCreateTool::SetSurfaceSnap( bool snap )
{
    s_SurfaceSnap = snap;

    if (s_SurfaceSnap)
    {
        s_ObjectSnap = false;
        m_Generator->GetContainer()->Read();
    }

    m_Scene->Execute( true );
}

bool CurveCreateTool::GetObjectSnap() const
{
    return s_ObjectSnap;
}

void CurveCreateTool::SetObjectSnap( bool snap )
{
    s_ObjectSnap = snap;

    if (s_ObjectSnap)
    {
        s_SurfaceSnap = false;
        m_Generator->GetContainer()->Read();
    }

    m_Scene->Execute( true );
}

int CurveCreateTool::GetPlaneSnap() const
{
    return SceneGraph::CreateTool::s_PlaneSnap;
}

void CurveCreateTool::SetPlaneSnap(int snap)
{
    SceneGraph::CreateTool::s_PlaneSnap = (IntersectionPlane)snap;

    m_Scene->Execute(false);
}

int CurveCreateTool::GetCurveType() const
{ 
    if ( m_Instance.ReferencesObject() )
    {
        return m_Instance->GetCurveType();
    }
    else
    {
        return 0;
    }
}

void CurveCreateTool::SetCurveType( int selection )
{
    if ( m_Instance.ReferencesObject() )
    {
        m_Instance->SetCurveType( selection );  
        m_Scene->Execute( true );
    }
}

bool CurveCreateTool::GetClosed() const
{  
    if ( m_Instance.ReferencesObject() )
    {
        return m_Instance->GetClosed();
    }
    else
    {
        return false;
    }
}

void CurveCreateTool::SetClosed(bool closed)
{
    if ( m_Instance.ReferencesObject() )
    {
        m_Instance->SetClosed( closed );
        m_Scene->Execute( true );
    }
}
