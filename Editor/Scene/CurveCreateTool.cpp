#include "Precompile.h"
#include "CurveCreateTool.h"
#include "CreateTool.h"

#include "Point.h"
#include "Pick.h"

#include "Scene.h"
#include "SceneManager.h"
#include "HierarchyNodeType.h"

using namespace Math;
using namespace Editor;

Content::CurveType CurveCreateTool::s_CurveType = Content::CurveTypes::BSpline;
bool CurveCreateTool::s_SurfaceSnap = false;
bool CurveCreateTool::s_ObjectSnap = false;

LUNA_DEFINE_TYPE(Editor::CurveCreateTool);

void CurveCreateTool::InitializeType()
{
  Reflect::RegisterClass< Editor::CurveCreateTool >( TXT( "Editor::CurveCreateTool" ) );
}

void CurveCreateTool::CleanupType()
{
  Reflect::UnregisterClass< Editor::CurveCreateTool >();
}

CurveCreateTool::CurveCreateTool( Editor::Scene* scene, PropertiesGenerator* generator )
: Tool( scene, generator )
, m_Instance( NULL )
, m_Created( false )
{
  CreateInstance( Math::Vector3::Zero );
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

void CurveCreateTool::CreateInstance( const Math::Vector3& position )
{
  if (m_Instance.ReferencesObject())
  {
    // remove temp reference
    m_Scene->RemoveObject( m_Instance );
  }

  m_Instance = new Editor::Curve( m_Scene, new Content::Curve() );

  m_Instance->SetSelected( true );

  m_Instance->SetTransient( true );

  m_Instance->SetCurveType( s_CurveType );

  m_Scene->AddObject( m_Instance );
  PointPtr point = new Editor::Point( m_Scene, new Content::Point( position ) );
  point->SetParent( m_Instance );
  point->SetTransient( true );
  m_Scene->AddObject( point );

  m_Instance->Evaluate( GraphDirections::Downstream );
}

void CurveCreateTool::PickPosition(int x, int y, Math::Vector3 &position)
{
  FrustumLinePickVisitor pick (m_Scene->GetViewport()->GetCamera(), x, y);

  // pick in the world
  m_Scene->GetManager()->GetRootScene()->Pick(&pick);

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

      if ( (*itr)->GetObject() != m_Instance )
      {
        Editor::HierarchyNode* node = Reflect::ObjectCast<Editor::HierarchyNode>( (*itr)->GetObject() );

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
    m_Scene->GetViewport()->GetCamera()->ViewportToPlaneVertex(x, y, Editor::CreateTool::s_PlaneSnap, position);
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

  u32 countControlPoints = m_Instance->GetNumberControlPoints();
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
  m_Instance->Initialize();

  // center origin
  m_Instance->CenterTransform();

  // append instance to selection
  m_Selection.Append( m_Instance );

  // commit the changes
  m_Scene->Push( batch );
  m_Scene->Execute( false );

  m_Instance = NULL;
  CreateInstance( Math::Vector3::Zero );
}

bool CurveCreateTool::AllowSelection()
{
  return false;
}

bool CurveCreateTool::MouseDown( wxMouseEvent& e )
{
  if ( m_Instance.ReferencesObject() && m_Scene->IsEditable() )
  {
    Math::Vector3 position;
    PickPosition( e.GetX(), e.GetY(), position );

    PointPtr point = new Editor::Point( m_Scene, new Content::Point( position ) );
    point->SetParent( m_Instance );
    point->SetTransient( true );
    m_Scene->AddObject( point );

    m_Instance->Dirty();

    m_Scene->Execute( true );
  }

  return __super::MouseDown( e );
}

void CurveCreateTool::MouseMove( wxMouseEvent& e )
{
  if ( m_Instance.ReferencesObject() )
  {
    u32 countControlPoints = m_Instance->GetNumberControlPoints();

    if ( countControlPoints > 0 )
    {
      Math::Vector3 position;
      PickPosition( e.GetX(), e.GetY(), position );

      Editor::Point* current = m_Instance->GetControlPointByIndex( countControlPoints - 1 );
      current->SetPosition( position );

      m_Instance->Dirty();

      m_Scene->Execute( true );
    }
  }

  __super::MouseMove( e );
} 

void CurveCreateTool::KeyPress(wxKeyEvent &e)
{
  const int keyCode = e.GetKeyCode();

  switch( keyCode )
  { 
  case WXK_RETURN:
    {
      AddToScene();
      break;
    }
  }

  __super::KeyPress( e );
}

void CurveCreateTool::CreateProperties()
{
  __super::CreateProperties();

  m_Generator->PushPanel( TXT( "Create Curve" ), true );
  {
    m_Generator->PushContainer();
    {
      m_Generator->AddLabel( TXT( "Surface Snap" ) );   
      m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<Editor::CurveCreateTool, bool> (this, &CurveCreateTool::GetSurfaceSnap, &CurveCreateTool::SetSurfaceSnap ) );
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
      m_Generator->AddLabel( TXT( "Object Snap" ) );   
      m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<Editor::CurveCreateTool, bool> (this, &CurveCreateTool::GetObjectSnap, &CurveCreateTool::SetObjectSnap ) );
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
      m_Generator->AddLabel( TXT( "Plane Snap" ) );
      Inspect::Choice* choice = m_Generator->AddChoice<int>( new Helium::MemberProperty<Editor::CurveCreateTool, int> (this, &CurveCreateTool::GetPlaneSnap, &CurveCreateTool::SetPlaneSnap) );
      choice->SetDropDown( true );
      Inspect::V_Item items;

      {
        tostringstream str;
        str << IntersectionPlanes::Viewport;
        items.push_back( Inspect::Item( TXT( "Viewport" ), str.str() ) );
      }

      {
        tostringstream str;
        str << IntersectionPlanes::Ground;
        items.push_back( Inspect::Item( TXT( "Ground" ), str.str() ) );
      }

      choice->SetItems( items );
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
      m_Generator->AddLabel( TXT( "Curve Type" ) );
      Inspect::Choice* choice = m_Generator->AddChoice<int>( new Helium::MemberProperty<Editor::CurveCreateTool, int> (this, &CurveCreateTool::GetCurveType, &CurveCreateTool::SetCurveType ) );
      choice->SetDropDown( true );
      Inspect::V_Item items;

      {
        tostringstream str;
        str << Content::CurveTypes::Linear;
        items.push_back( Inspect::Item( TXT( "Linear" ), str.str() ) );
      }

      {
        tostringstream str;
        str << Content::CurveTypes::BSpline;
        items.push_back( Inspect::Item( TXT( "BSpline" ), str.str() ) );
      }

      {
        tostringstream str;
        str << Content::CurveTypes::CatmullRom;
        items.push_back( Inspect::Item( TXT( "Catmull-Rom" ), str.str() ) );
      }

      choice->SetItems( items );

    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
      m_Generator->AddLabel( TXT( "Closed" ) );
      m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<Editor::CurveCreateTool, bool> (this, &CurveCreateTool::GetClosed, &CurveCreateTool::SetClosed ) );
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
  return Editor::CreateTool::s_PlaneSnap;
}

void CurveCreateTool::SetPlaneSnap(int snap)
{
  Editor::CreateTool::s_PlaneSnap = (IntersectionPlane)snap;

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
    m_Instance->SetCurveType( (Content::CurveType) selection );  
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
