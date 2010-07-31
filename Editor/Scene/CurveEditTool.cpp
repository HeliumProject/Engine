#include "Precompile.h"
#include "CurveEditTool.h"

#include "Point.h"
#include "Pick.h"

#include "TranslateManipulator.h"
#include "Curve.h"
#include "Scene.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Editor;

EDITOR_DEFINE_TYPE(Editor::CurveEditTool);

void CurveEditTool::InitializeType()
{
  Reflect::RegisterClass< Editor::CurveEditTool >( TXT( "Editor::CurveEditTool" ) );
}

void CurveEditTool::CleanupType()
{
  Reflect::UnregisterClass< Editor::CurveEditTool >();
}

CurveEditMode CurveEditTool::s_EditMode = CurveEditModes::Modify;
bool CurveEditTool::s_CurrentSelection = false;

CurveEditTool::CurveEditTool(Editor::Scene *scene, Editor::PropertiesGenerator *generator)
: Tool( scene, generator )
, m_HotEditMode ( CurveEditModes::None )
{
  Initialize();

  m_ControlPointManipulator = new Editor::TranslateManipulator( ManipulatorModes::Translate, scene, generator );
}

CurveEditTool::~CurveEditTool()
{
  delete m_ControlPointManipulator;
}

CurveEditMode CurveEditTool::GetEditMode() const
{
  return m_HotEditMode != CurveEditModes::None ? m_HotEditMode : s_EditMode;
}

bool CurveEditTool::MouseDown( wxMouseEvent &e )
{
  bool success = true;

  if ( GetEditMode() == CurveEditModes::Modify )
  {
    success = m_ControlPointManipulator->MouseDown( e );
  }
  else
  {
    Curve* curve = NULL;
    
    {
      FrustumLinePickVisitor pick (m_Scene->GetViewport()->GetCamera(), e.GetX(), e.GetY());

      m_Scene->Pick( &pick );

      V_PickHitSmartPtr sorted;
      PickHit::Sort(m_Scene->GetViewport()->GetCamera(), pick.GetHits(), sorted, PickSortTypes::Intersection);

      for ( V_PickHitSmartPtr::const_iterator itr = sorted.begin(), end = sorted.end(); itr != end; ++itr )
      {
        if ( curve = Reflect::ObjectCast<Curve>( (*itr)->GetObject() ) )
        {
          break;
        }
      }
    }

    if ( !curve || !m_Scene->IsEditable() )
    {
      return false;
    }

    LinePickVisitor pick (m_Scene->GetViewport()->GetCamera(), e.GetX(), e.GetY());

    switch ( GetEditMode() )
    {
    case CurveEditModes::Insert:
      {
        std::pair<u32, u32> points;
        if ( !curve->ClosestControlPoints( &pick, points ) )
        {
          return false;
        }

        Editor::Point* p0 = curve->GetControlPointByIndex( points.first );
        Editor::Point* p1 = curve->GetControlPointByIndex( points.second );

        Vector3 a( p0->GetPosition() );
        Vector3 b( p1->GetPosition() );
        Vector3 p;

        if ( curve->GetCurveType() == Content::CurveTypes::Linear )
        {
          float mu;

          if ( !pick.GetPickSpaceLine().IntersectsSegment( a, b, -1.0f, &mu ) )
          {
            return false;
          }

          p = a * ( 1.0f - mu ) + b * mu;
        }
        else
        {
          p = ( a + b ) * 0.5f;
        }

        u32 index = points.first > points.second ? points.first : points.second;

        PointPtr point = new Editor::Point( m_Scene, new Content::Point( p ) );

        curve->GetScene()->Push( curve->InsertControlPointAtIndex( index, point ) );
        break;
      }

    case CurveEditModes::Remove:
      {
        i32 index = curve->ClosestControlPoint( &pick );

        if ( index < 0 )
        {
          return false;
        }

        curve->GetScene()->Push( curve->RemoveControlPointAtIndex( index ) );
        break;
      }
    }

    curve->Dirty();

    m_Scene->Execute( false );
  }

  return success || __super::MouseDown( e );
}

void CurveEditTool::MouseUp( wxMouseEvent& e )
{
  if ( GetEditMode() )
  {
    m_ControlPointManipulator->MouseUp( e );
  }

  m_AllowSelection = true;

  __super::MouseUp( e );
}

void CurveEditTool::MouseMove( wxMouseEvent& e )
{
  if ( GetEditMode() )
  {
    m_ControlPointManipulator->MouseMove( e );

    if ( e.Dragging() )
    {
      // clear the current highlight
      m_Scene->ClearHighlight( ClearHighlightArgs(true) );

      // disallow selection when dragging
      m_AllowSelection = false;
    }
  }

  __super::MouseMove( e );
}

void CurveEditTool::KeyPress(wxKeyEvent &e)
{
  if ( !m_Scene->IsEditable() )
  {
    return;
  }

  i32 keyCode = e.GetKeyCode();

  if ( keyCode == WXK_LEFT || keyCode == WXK_UP || keyCode == WXK_RIGHT || keyCode == WXK_DOWN )
  {
    OS_SelectableDumbPtr selection = m_Scene->GetSelection().GetItems();

    if ( selection.Empty() )
    {
      return;
    }

    Editor::Point* point = Reflect::ObjectCast<Editor::Point>( selection.Front() );

    if ( !point )
    {
      return;
    }

    Editor::Curve* curve = Reflect::ObjectCast<Editor::Curve>( point->GetParent() );

    if ( !curve )
    {
      return;
    }

    i32 index =  curve->GetIndexForControlPoint( point );

    if ( index == -1 )
    {
      return;
    }

    u32 countControlPoints = curve->GetNumberControlPoints();
    if ( keyCode == WXK_LEFT || keyCode == WXK_DOWN )
    {
      index--;
      index += countControlPoints;
      index %= countControlPoints;
    }
    else if ( keyCode == WXK_RIGHT || keyCode == WXK_UP ) 
    {
      index++;
      index %= countControlPoints;
    }

    point = curve->GetControlPointByIndex( index );

    selection.Clear();
    selection.Append( point );
    m_Scene->GetSelection().SetItems( selection );
  }

  __super::KeyPress( e );
}

void CurveEditTool::KeyDown( wxKeyEvent& e )
{
  CurveEditMode mode = m_HotEditMode;

  switch (e.GetKeyCode())
  {
  case wxT('M'):
    m_HotEditMode = CurveEditModes::Modify;
    break;

  case wxT('I'):
    m_HotEditMode = CurveEditModes::Insert;
    break;

  case wxT('R'):
    m_HotEditMode = CurveEditModes::Remove;
    break;

  default:
    __super::KeyDown( e );
    break;
  }

  if ( mode != m_HotEditMode )
  {
    m_Generator->GetContainer()->Read();
  }
}

void CurveEditTool::KeyUp( wxKeyEvent& e )
{
  CurveEditMode mode = m_HotEditMode;

  switch (e.GetKeyCode())
  {
  case wxT('M'):
  case wxT('I'):
  case wxT('R'):
    m_HotEditMode = CurveEditModes::None;
    break;

  default:
    __super::KeyUp( e );
    break;
  }

  if ( mode != m_HotEditMode )
  {
    m_Generator->GetContainer()->Read();
  }
}

bool CurveEditTool::ValidateSelection( OS_SelectableDumbPtr& items )
{
  OS_SelectableDumbPtr result;

  OS_SelectableDumbPtr::Iterator itr = items.Begin();
  OS_SelectableDumbPtr::Iterator end = items.End();
  for( ; itr != end; ++itr )
  {
    Editor::Point* p = Reflect::ObjectCast<Editor::Point>( *itr );

    if ( !p )
    {
      continue;
    }
    
    bool appendPoint = true;
    if ( s_CurrentSelection )
    {
      appendPoint = false;
      OS_SelectableDumbPtr::Iterator curveItr = m_SelectedCurves.Begin();
      OS_SelectableDumbPtr::Iterator curveEnd = m_SelectedCurves.End();
      for ( ; curveItr != curveEnd; ++curveItr )
      {
        if ( p->GetParent() == *curveItr )
        {
          appendPoint = true;
          break;
        }
      }
    }
    
    if ( appendPoint )
    {
      result.Append( p );
    }
  }

  items = result;

  if ( items.Empty() )
  {
    OS_SelectableDumbPtr::Iterator itr = items.Begin();
    OS_SelectableDumbPtr::Iterator end = items.End();
    for( ; itr != end; ++itr )
    {
      Editor::Curve* c = Reflect::ObjectCast<Editor::Curve>( *itr );

      if ( !c )
      {
        continue;
      }

      result.Append( c );
      break;
    }
  }

  items = result;

  return !items.Empty();
}

void CurveEditTool::Evaluate()
{
  if ( GetEditMode() )
  {
    m_ControlPointManipulator->Evaluate();
  }

  __super::Evaluate();
}

void CurveEditTool::Draw( DrawArgs* args )
{
  if ( GetEditMode() )
  {
    m_ControlPointManipulator->Draw( args );
  }

  __super::Draw( args );
}

void CurveEditTool::CreateProperties()
{
  __super::CreateProperties();

  m_Generator->PushPanel( TXT( "Edit Curve" ), true );
  {
    m_Generator->PushContainer();
    { 
      m_Generator->AddLabel( TXT( "Edit Control Points" ) );
      Inspect::Choice* choice = m_Generator->AddChoice<int>( new Helium::MemberProperty<Editor::CurveEditTool, int> (this, &CurveEditTool::GetCurveEditMode, &CurveEditTool::SetCurveEditMode ) );
      choice->SetDropDown( true );
      Inspect::V_Item items;

      {
        tostringstream str;
        str << CurveEditModes::Modify;
        items.push_back( Inspect::Item( TXT( "Modify Points" ), str.str() ) );
      }

      {
        tostringstream str;
        str << CurveEditModes::Insert;
        items.push_back( Inspect::Item( TXT( "Insert Points" ), str.str() ) );
      }

      {
        tostringstream str;
        str << CurveEditModes::Remove;
        items.push_back( Inspect::Item( TXT( "Remove Points" ), str.str() ) );
      }
      choice->SetItems( items );
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    { 
      m_Generator->AddLabel( TXT( "Selected Curves Only" ) );
      m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<Editor::CurveEditTool, bool> (this, &CurveEditTool::GetSelectionMode, &CurveEditTool::SetSelectionMode ) );
    }
    m_Generator->Pop();
  }
  m_Generator->Pop();

  m_ControlPointManipulator->CreateProperties();
}

int CurveEditTool::GetCurveEditMode() const
{
  return s_EditMode;
}

void CurveEditTool::SetCurveEditMode( int mode )
{
  s_EditMode = (CurveEditMode)mode;

  m_Scene->GetSelection().Clear();
}

bool CurveEditTool::GetSelectionMode() const
{
  return s_CurrentSelection;
}

void CurveEditTool::SetSelectionMode( bool mode )
{
  s_CurrentSelection = mode;
}

void CurveEditTool::StoreSelectedCurves()
{
  m_SelectedCurves.Clear();
  OS_SelectableDumbPtr::Iterator selection_itr = m_Scene->GetSelection().GetItems().Begin();
  OS_SelectableDumbPtr::Iterator selection_end = m_Scene->GetSelection().GetItems().End();
  for ( ; selection_itr != selection_end; ++selection_itr )
  {
    Editor::Curve* curve = Reflect::ObjectCast<Editor::Curve>( *selection_itr );
    if ( curve )
    {
      m_SelectedCurves.Append( curve );
    }
  }
}
