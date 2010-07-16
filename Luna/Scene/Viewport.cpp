#include "Precompile.h"
#include "Viewport.h"

#include "Camera.h"
#include "CameraMovedCommand.h"
#include "Color.h"
#include "Pick.h"
#include "PrimitiveAxes.h"
#include "PrimitiveFrame.h"
#include "PrimitiveGrid.h"
#include "PrimitiveRings.h"
#include "Resource.h"
#include "SceneEditorIDs.h"
#include "SceneInit.h"
#include "ScenePreferences.h"
#include "Statistics.h"
#include "Tool.h"

#include "Platform/Exception.h"
#include "Orientation.h"

using namespace Math;
using namespace Luna;


BEGIN_EVENT_TABLE(Luna::Viewport, wxWindow)

EVT_SIZE(Viewport::OnSize)

EVT_PAINT(Viewport::OnPaint)
EVT_SET_FOCUS(Viewport::OnSetFocus)
EVT_KILL_FOCUS(Viewport::OnKillFocus)

EVT_KEY_DOWN(Viewport::OnKeyDown)
EVT_KEY_UP(Viewport::OnKeyUp)
EVT_CHAR(Viewport::OnChar)

EVT_LEFT_DOWN(Viewport::OnMouseDown)
EVT_MIDDLE_DOWN(Viewport::OnMouseDown)
EVT_RIGHT_DOWN(Viewport::OnMouseDown)

EVT_LEFT_UP(Viewport::OnMouseUp)
EVT_MIDDLE_UP(Viewport::OnMouseUp)
EVT_RIGHT_UP(Viewport::OnMouseUp)

EVT_MOTION(Viewport::OnMouseMove)
EVT_MOUSEWHEEL(Viewport::OnMouseScroll)
EVT_LEAVE_WINDOW(Viewport::OnMouseLeave)

EVT_MOUSE_CAPTURE_LOST(Viewport::OnMouseCaptureLost)

END_EVENT_TABLE()

D3DMATERIAL9 Viewport::s_LiveMaterial;
D3DMATERIAL9 Viewport::s_SelectedMaterial;
D3DMATERIAL9 Viewport::s_ReactiveMaterial;
D3DMATERIAL9 Viewport::s_HighlightedMaterial;
D3DMATERIAL9 Viewport::s_UnselectableMaterial;
D3DMATERIAL9 Viewport::s_ComponentMaterial;
D3DMATERIAL9 Viewport::s_SelectedComponentMaterial;
D3DMATERIAL9 Viewport::s_AssetTypeMaterials[ 1 ];
D3DMATERIAL9 Viewport::s_RedMaterial;
D3DMATERIAL9 Viewport::s_YellowMaterial;
D3DMATERIAL9 Viewport::s_GreenMaterial;
D3DMATERIAL9 Viewport::s_BlueMaterial;

void Viewport::InitializeType()
{
  ZeroMemory( &s_LiveMaterial, sizeof( s_LiveMaterial ) );
  s_LiveMaterial.Ambient = Luna::Color::MAGENTA;
  s_LiveMaterial.Diffuse = Luna::Color::BLACK;
  s_LiveMaterial.Specular = Luna::Color::BLACK;

  ZeroMemory( &s_SelectedMaterial, sizeof( s_SelectedMaterial ) );
  s_SelectedMaterial.Ambient = Luna::Color::SPRINGGREEN;
  s_SelectedMaterial.Diffuse = Luna::Color::BLACK;
  s_SelectedMaterial.Specular = Luna::Color::BLACK;

  ZeroMemory( &s_RedMaterial, sizeof( s_RedMaterial ) );
  s_RedMaterial.Ambient = Luna::Color::RED;
  s_RedMaterial.Diffuse = Luna::Color::BLACK;
  s_RedMaterial.Specular = Luna::Color::BLACK;

  ZeroMemory( &s_YellowMaterial, sizeof( s_YellowMaterial ) );
  s_YellowMaterial.Ambient = Luna::Color::YELLOW;
  s_YellowMaterial.Diffuse = Luna::Color::BLACK;
  s_YellowMaterial.Specular = Luna::Color::BLACK;

  ZeroMemory( &s_GreenMaterial, sizeof( s_GreenMaterial ) );
  s_GreenMaterial.Ambient = Luna::Color::GREEN;
  s_GreenMaterial.Diffuse = Luna::Color::BLACK;
  s_GreenMaterial.Specular = Luna::Color::BLACK;


  ZeroMemory( &s_ReactiveMaterial, sizeof( s_ReactiveMaterial ) );
  s_ReactiveMaterial.Ambient = Luna::Color::WHITE;
  s_ReactiveMaterial.Diffuse = Luna::Color::BLACK;
  s_ReactiveMaterial.Specular = Luna::Color::BLACK;

  ZeroMemory( &s_HighlightedMaterial, sizeof( s_HighlightedMaterial ) );
  s_HighlightedMaterial.Ambient = Luna::Color::CYAN;
  s_HighlightedMaterial.Diffuse = Luna::Color::BLACK;
  s_HighlightedMaterial.Specular = Luna::Color::BLACK;

  ZeroMemory( &s_UnselectableMaterial, sizeof( s_UnselectableMaterial ) );
  s_UnselectableMaterial.Ambient = Luna::Color::GRAY;
  s_UnselectableMaterial.Diffuse = Luna::Color::BLACK;
  s_UnselectableMaterial.Specular = Luna::Color::BLACK;

  ZeroMemory( &s_ComponentMaterial, sizeof( s_ComponentMaterial ) );
  s_ComponentMaterial.Ambient = Luna::Color::MAGENTA;
  s_ComponentMaterial.Diffuse = Luna::Color::BLACK;
  s_ComponentMaterial.Specular = Luna::Color::BLACK;

  ZeroMemory( &s_SelectedComponentMaterial, sizeof( s_SelectedComponentMaterial ) );
  s_SelectedComponentMaterial.Ambient = Luna::Color::YELLOW;
  s_SelectedComponentMaterial.Diffuse = Luna::Color::BLACK;
  s_SelectedComponentMaterial.Specular = Luna::Color::BLACK;
  
  ZeroMemory( &s_BlueMaterial, sizeof( s_BlueMaterial ) );
  s_BlueMaterial.Ambient = Luna::Color::BLUE;
  s_BlueMaterial.Diffuse = Luna::Color::BLACK;
  s_BlueMaterial.Specular = Luna::Color::BLACK;
  
  ZeroMemory( s_AssetTypeMaterials, sizeof( s_AssetTypeMaterials ) );
  s_AssetTypeMaterials[ 0 ].Ambient = Luna::Color::BLACK;
  s_AssetTypeMaterials[ 0 ].Diffuse = Luna::Color::BLACK;
  s_AssetTypeMaterials[ 0 ].Specular = Luna::Color::BLACK;
}

void Viewport::CleanupType()
{

}

Viewport::Viewport(wxWindow *parent,
             wxWindowID winid,
             const wxPoint& pos,
             const wxSize& size,
             long style,
             const wxString& name)
             : wxWindow (parent, winid, pos, size, style, name)
             , m_Focused (false)
             , m_ResourceTracker (NULL)
             , m_Tool (NULL)
             , m_CameraMode (CameraModes::Orbit)
             , m_GeometryMode (GeometryModes::Render)
             , m_DragMode (DragModes::None)
             , m_Highlighting (true)
             , m_AxesVisible (true)
             , m_GridVisible (true)
             , m_BoundsVisible (false)
             , m_StatisticsVisible (false)
             , m_Statistics (NULL)
             , m_SelectionFrame (NULL)
{
  // don't erase background
  SetBackgroundStyle(wxBG_STYLE_CUSTOM);

  memset(m_GlobalPrimitives, NULL, sizeof(m_GlobalPrimitives));

  SetMinSize( wxSize(1, 1) );

  InitDevice();

  InitWidgets();

  InitCameras();

  Reset();

  SetHelpText( TXT( "This is the main editing view.  It displays objects in the scene and allows manipulation of the data." ) );
}

Viewport::~Viewport()
{
  m_Cameras[ CameraModes::Orbit ].RemoveMovedListener( CameraMovedSignature::Delegate ( this, &Viewport::CameraMoved ) );

  m_D3DManager.RemoveDeviceFoundListener( DeviceStateSignature::Delegate( this, &Viewport::OnAllocateResources ) );
  m_D3DManager.RemoveDeviceLostListener( DeviceStateSignature::Delegate( this, &Viewport::OnReleaseResources ) );

  for (u32 i=0; i<GlobalPrimitives::Count; i++)
    delete m_GlobalPrimitives[i];

  delete m_Statistics;
  delete m_SelectionFrame;

  delete m_ResourceTracker;
}

ResourceTracker* Viewport::GetResources() const
{
  return m_ResourceTracker;
}

Statistics* Viewport::GetStatistics() const
{
  return m_Statistics;
}

Luna::Camera* Viewport::GetCamera()
{
  return &m_Cameras[m_CameraMode];
}

const Luna::Camera* Viewport::GetCamera() const
{
  return &m_Cameras[m_CameraMode];
}

Luna::Camera* Viewport::GetCameraForMode(CameraMode mode)
{
  return &m_Cameras[mode]; 
}

CameraMode Viewport::GetCameraMode() const
{
  return m_CameraMode;
}

void Viewport::SetCameraMode(CameraMode mode)
{
  if ( mode != m_CameraMode )
  {
    CameraMode old = m_CameraMode;
    m_CameraMode = mode;

    Refresh();

    m_CameraModeChanged.Raise( CameraModeChangeArgs( old, m_CameraMode ) );
  }
}

void Viewport::NextCameraMode()
{
  SetCameraMode((CameraMode)((m_CameraMode + 1) % CameraModes::Count));
}

GeometryMode Viewport::GetGeometryMode() const
{
  return m_GeometryMode;
}

void Viewport::SetGeometryMode(GeometryMode mode)
{
  m_GeometryMode = mode;

  Refresh();
}

void Viewport::NextGeometryMode()
{
  SetGeometryMode((GeometryMode)((m_GeometryMode + 1) % GeometryModes::Count));
}

Luna::Tool* Viewport::GetTool()
{
  return m_Tool;
}

void Viewport::SetTool(Luna::Tool* tool)
{
  if ( m_Tool != tool )
  {
    m_Tool = tool;
    m_ToolChanged.Raise( ToolChangeArgs( m_Tool ) );
  }

  //make sure if we are in the highlight tool that we are in highlight drag mode
  if (!m_Tool)
  {
    m_DragMode = DragModes::None;
  }
}

bool Viewport::IsHighlighting() const
{
  return m_Highlighting;
}

void Viewport::SetHighlighting(bool highlight)
{
  m_Highlighting = highlight;

  m_DragMode = DragModes::None;

  if (m_Highlighting)
  {
    Refresh();
  }
  else
  {
    m_ClearHighlight.Raise( ClearHighlightArgs (true) );
  }
}

bool Viewport::IsAxesVisible() const
{
  return m_AxesVisible;
}

void Viewport::SetAxesVisible(bool visible)
{
  m_AxesVisible = visible;
}

bool Viewport::IsGridVisible() const
{
  return m_GridVisible;
}

void Viewport::SetGridVisible(bool visible)
{
  m_GridVisible = visible;
}

bool Viewport::IsBoundsVisible() const
{
  return m_BoundsVisible;
}

void Viewport::SetBoundsVisible(bool visible)
{
  m_BoundsVisible = visible;
}

bool Viewport::IsStatisticsVisible() const
{
  return m_StatisticsVisible;
}

void Viewport::SetStatisticsVisible(bool visible)
{
  m_StatisticsVisible = visible;
}

Luna::Primitive* Viewport::GetGlobalPrimitive( GlobalPrimitives::GlobalPrimitive which )
{
  Luna::Primitive* prim = NULL;
  if ( which >= 0 && which < GlobalPrimitives::Count )
  {
    prim = m_GlobalPrimitives[which];
  }
  return prim;
}

void Viewport::InitDevice()
{
  m_D3DManager.InitD3D( GetHwnd(), 64, 64 );
  m_D3DManager.AddDeviceFoundListener( DeviceStateSignature::Delegate( this, &Viewport::OnAllocateResources ) );
  m_D3DManager.AddDeviceLostListener( DeviceStateSignature::Delegate( this, &Viewport::OnReleaseResources ) );

  m_ResourceTracker = new ResourceTracker( GetDevice() );
}

void Viewport::InitWidgets()
{
  if ( !GetDevice() )
  {
    return;
  }

  // primitive API uses this, so init it first
  m_Statistics = new Statistics( m_ResourceTracker->GetDevice() );

  m_GlobalPrimitives[GlobalPrimitives::ViewportAxes] = new Luna::PrimitiveAxes (m_ResourceTracker);
  m_GlobalPrimitives[GlobalPrimitives::ViewportAxes]->Update();

  m_GlobalPrimitives[GlobalPrimitives::StandardAxes] = new Luna::PrimitiveAxes (m_ResourceTracker);
  m_GlobalPrimitives[GlobalPrimitives::StandardAxes]->Update();

  m_GlobalPrimitives[GlobalPrimitives::StandardGrid] = new Luna::PrimitiveGrid (m_ResourceTracker);
  SceneEditorPreferences()->GetGridPreferencesPtr()->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &Viewport::OnGridPreferencesChanged ));
  OnGridPreferencesChanged( Reflect::ElementChangeArgs( NULL, NULL ) );

  m_GlobalPrimitives[GlobalPrimitives::StandardRings] = new Luna::PrimitiveRings (m_ResourceTracker);
  m_GlobalPrimitives[GlobalPrimitives::StandardRings]->Update();

  m_GlobalPrimitives[GlobalPrimitives::TransformAxes] = new Luna::PrimitiveAxes (m_ResourceTracker);
  m_GlobalPrimitives[GlobalPrimitives::TransformAxes]->Update();

  m_GlobalPrimitives[GlobalPrimitives::SelectedAxes] = new Luna::PrimitiveAxes (m_ResourceTracker);
  m_GlobalPrimitives[GlobalPrimitives::SelectedAxes]->Update();

  m_GlobalPrimitives[GlobalPrimitives::JointAxes] = new Luna::PrimitiveAxes (m_ResourceTracker);
  m_GlobalPrimitives[GlobalPrimitives::JointAxes]->Update();

  m_GlobalPrimitives[GlobalPrimitives::JointRings] = new Luna::PrimitiveRings (m_ResourceTracker);
  m_GlobalPrimitives[GlobalPrimitives::JointRings]->Update();

  m_SelectionFrame = new Luna::PrimitiveFrame ( m_ResourceTracker );
  m_SelectionFrame->Update();
}

void Viewport::InitCameras()
{
  // create the cameras
  m_Cameras[CameraModes::Orbit].Setup(ProjectionModes::Perspective, Vector3::Zero, Vector3::Zero);
  m_Cameras[CameraModes::Front].Setup(ProjectionModes::Orthographic, OutVector * -1.f, UpVector);
  m_Cameras[CameraModes::Side].Setup(ProjectionModes::Orthographic, SideVector * -1.f, UpVector);
  m_Cameras[CameraModes::Top].Setup(ProjectionModes::Orthographic, UpVector * -1.f, OutVector * -1.f);
  
  // Set the max size of the camera history
  m_CameraHistory[CameraModes::Orbit].SetMaxLength( 10 );
  m_CameraHistory[CameraModes::Front].SetMaxLength( 10 );
  m_CameraHistory[CameraModes::Side].SetMaxLength( 10 );
  m_CameraHistory[CameraModes::Top].SetMaxLength( 10 );

  m_Cameras[ CameraModes::Orbit ].AddMovedListener( CameraMovedSignature::Delegate ( this, &Viewport::CameraMoved ) );
}

void Viewport::Reset()
{
  if ( !GetDevice() )
  {
    return;
  }

  for (u32 i=0; i<CameraModes::Count; i++)
  {
    m_Cameras[i].Reset();
  }

  static_cast<Luna::PrimitiveAxes*>(m_GlobalPrimitives[GlobalPrimitives::ViewportAxes])->m_Length = 0.05f;
  static_cast<Luna::PrimitiveAxes*>(m_GlobalPrimitives[GlobalPrimitives::ViewportAxes])->Update();

  Luna::PrimitiveAxes* transformAxes = static_cast< Luna::PrimitiveAxes* >( m_GlobalPrimitives[GlobalPrimitives::TransformAxes] );
  transformAxes->m_Length = 0.10f;
  transformAxes->Update();

  Luna::PrimitiveAxes* transformAxesSelected = static_cast< Luna::PrimitiveAxes* >( m_GlobalPrimitives[GlobalPrimitives::SelectedAxes] );
  transformAxesSelected->m_Length = 0.10f;
  transformAxesSelected->SetColor( D3DCOLOR_COLORVALUE( s_SelectedMaterial.Ambient.r, s_SelectedMaterial.Ambient.g, s_SelectedMaterial.Ambient.b, s_SelectedMaterial.Ambient.a ) );
  transformAxesSelected->Update();

  Luna::PrimitiveAxes* jointAxes = static_cast< Luna::PrimitiveAxes* >( m_GlobalPrimitives[GlobalPrimitives::JointAxes] );
  jointAxes->m_Length = 0.015f;
  jointAxes->Update();

  Luna::PrimitiveRings* jointRings = static_cast< Luna::PrimitiveRings* >( m_GlobalPrimitives[GlobalPrimitives::JointRings] );
  jointRings->m_Radius = 0.015f;
  jointRings->m_Steps = 18;
  jointRings->Update();

  m_AxesVisible = true;
  m_GridVisible = true;
  m_BoundsVisible = false;
  m_StatisticsVisible = false;

#ifdef _DEBUG
  m_StatisticsVisible = true;
#endif
}

void Viewport::OnSize(wxSizeEvent& e)
{
  if ( !GetDevice() )
  {
    return;
  }

  if ( e.GetSize().x > 0 && e.GetSize().y > 0 )
  {
    m_D3DManager.Resize( e.GetSize().x, e.GetSize().y );
  }

  Refresh();
}

void Viewport::OnPaint(wxPaintEvent& e)
{
  // draw
  Draw();

  // set our entire window to be valid
  ::ValidateRect( (HWND)GetHandle(), NULL );
}

void Viewport::OnSetFocus(wxFocusEvent& e)
{
  m_Focused = true;

  Refresh();
}

void Viewport::OnKillFocus(wxFocusEvent& e)
{
  m_Focused = false;

  Refresh();
}

void Viewport::OnKeyDown(wxKeyEvent& e)
{
  if ( m_Tool )
  {
    m_Tool->KeyDown( e );
  }
  else if ( e.ControlDown() && ( ( e.GetKeyCode() == 'A' ) || ( e.GetKeyCode() == 'a' ) ) )
  {
    wxWindow* frame = GetParent();
    while (frame->GetParent() != NULL)
      frame = frame->GetParent();
    frame->GetEventHandler()->ProcessEvent(e);

    frame->GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, SceneEditorIDs::ID_EditSelectAll) );
  }
  else
  {
    e.Skip();
  }

  if (e.GetSkipped())
  {
    wxWindow* frame = GetParent();
    while (frame->GetParent() != NULL)
      frame = frame->GetParent();
    frame->GetEventHandler()->ProcessEvent(e);
  }
}

void Viewport::OnKeyUp(wxKeyEvent& e)
{
  if ( m_Tool )
  {
    m_Tool->KeyUp( e );
  }
  else
  {
    e.Skip();
  }

  if (e.GetSkipped())
  {
    wxWindow* frame = GetParent();
    while (frame->GetParent() != NULL)
      frame = frame->GetParent();
    frame->GetEventHandler()->ProcessEvent(e);
  }
}

void Viewport::OnChar(wxKeyEvent& e)
{
  if ( m_Tool )
  {
    m_Tool->KeyPress( e );
  }
  else
  {
    e.Skip();
  }

  if (e.GetSkipped())
  {
    wxWindow* frame = GetParent();
    while (frame->GetParent() != NULL)
      frame = frame->GetParent();
    frame->GetEventHandler()->ProcessEvent(e);
  }
}

void Viewport::OnMouseDown(wxMouseEvent& e)
{
#pragma TODO("Freeze here -Geoff")

  if (!m_Focused)
  {
    // focus and eat the event
    SetFocus();
  }
  else
  {
    // reset drag mode
    m_DragMode = DragModes::None;

    // are we going to allow entering select drag?
    bool allowSelection = true;

    // if any key is down
    if (e.LeftIsDown() || e.MiddleIsDown() || e.RightIsDown())
    {
      // and we don't already own the mouse
      if (GetCapture() != this)
      {
        // capture it
        CaptureMouse();
      }
    }

    // if the camera modifier key is down
    if (e.AltDown())
    {
      // notifiy the camera
      m_Cameras[m_CameraMode].MouseDown(e);

      // camera now owns the drag
      m_DragMode = DragModes::Camera;

      // do NOT allow selection while moving the camera
      allowSelection = false;

      // Save the previous view before it is updated
      UpdateCameraHistory();      
    }
    else if (m_Tool) // else if we have a tool object
    {
      // hit test the tool, and if we intersect or its a tool modifier mouse key
      if (m_Tool->MouseDown(e) || e.MiddleDown() || e.RightDown())
      {
        // the tool now owns this drag
        m_DragMode = DragModes::Tool;
      }
      else
      {
        // we can still allow selection if it didn't hit he tool
        allowSelection = m_Tool->AllowSelection();
      }
    }

    // if its the left key if we are still looking for a selection
    if ( (e.LeftIsDown() || e.MiddleIsDown()) && m_DragMode == DragModes::None && allowSelection )
    {
      // we are selecting
      m_DragMode = DragModes::Select;

      // reset point trackers
      m_Start = Math::Point (e.GetX(), e.GetY());
      m_End = Math::Point (e.GetX(), e.GetY());

      // reset selection frame
      m_SelectionFrame->m_Start = m_Start;
      m_SelectionFrame->m_End = m_End;
      m_SelectionFrame->Update();

      // if we are highlighting
      if ( m_Highlighting )
      {
        // clear the previously highlighted set
        m_ClearHighlight.Raise( ClearHighlightArgs (false) );
      }
    }
  }

  Refresh();
  Update();
}

void Viewport::OnMouseUp(wxMouseEvent& e)
{
#pragma TODO("Freeze here -Geoff")

  // focus
  SetFocus();

  // have we completed a drag?
  bool dragComplete = false;

  switch (m_DragMode)
  {
  case DragModes::None:
    {
      if (!m_Tool && e.RightUp())
      {
        m_End = Math::Point (e.GetX(), e.GetY());
        m_SelectionFrame->m_End = m_End;
        m_SelectionFrame->Update();

        PickVisitor* pick = NULL;

        SelectionTargetMode targetMode = SelectionTargetModes::Single;

        if (m_SelectionFrame->m_Start.x == m_SelectionFrame->m_End.x && m_SelectionFrame->m_Start.y == m_SelectionFrame->m_End.y)
        {
          pick = new FrustumLinePickVisitor (&m_Cameras[m_CameraMode], m_SelectionFrame->m_Start.x, m_SelectionFrame->m_Start.y);
          targetMode = SelectionTargetModes::Single;
        }
        else 
        {
          Frustum worldSpaceFrustum;
          if ( m_Cameras[m_CameraMode].ViewportToFrustum(m_SelectionFrame->m_Start.x, m_SelectionFrame->m_Start.y, m_SelectionFrame->m_End.x, m_SelectionFrame->m_End.y, worldSpaceFrustum) )
          {
            Math::Point center ( m_SelectionFrame->m_Start + m_SelectionFrame->m_End / 2 );

            Math::Line line;
            m_Cameras[m_CameraMode].ViewportToLine(center.x, center.y, line);

            pick = new FrustumLinePickVisitor(&m_Cameras[m_CameraMode], line, worldSpaceFrustum );
            targetMode = SelectionTargetModes::Multiple;
          }                   
        }

        if ( pick )
        {
          m_Select.Raise( SelectArgs (pick, SelectionModes::Type, targetMode) );
          delete pick;
          pick = NULL;
        }

        // our drag is complete now
        dragComplete = true;
      }

      break;
    }

  case DragModes::Select:
    {
      if (e.LeftUp() || e.MiddleUp())
      {
        m_End = Math::Point (e.GetX(), e.GetY());
        m_SelectionFrame->m_End = m_End;
        m_SelectionFrame->Update();

        PickVisitor* pick = NULL;

        SelectionTargetMode targetMode = SelectionTargetModes::Single;

        if (m_SelectionFrame->m_Start.x == m_SelectionFrame->m_End.x && m_SelectionFrame->m_Start.y == m_SelectionFrame->m_End.y)
        {
          pick = new FrustumLinePickVisitor (&m_Cameras[m_CameraMode], m_SelectionFrame->m_Start.x, m_SelectionFrame->m_Start.y);
          targetMode = SelectionTargetModes::Single;
        }
        else 
        {
          Frustum worldSpaceFrustum;

          if ( m_Cameras[m_CameraMode].ViewportToFrustum(m_SelectionFrame->m_Start.x, m_SelectionFrame->m_Start.y, m_SelectionFrame->m_End.x, m_SelectionFrame->m_End.y, worldSpaceFrustum) )
          {
            Math::Point center ( m_SelectionFrame->m_Start + m_SelectionFrame->m_End / 2 );

            Math::Line line;
            m_Cameras[m_CameraMode].ViewportToLine(center.x, center.y, line);

            pick = new FrustumLinePickVisitor (&m_Cameras[m_CameraMode], line, worldSpaceFrustum); 
            targetMode = SelectionTargetModes::Multiple;
          }
        }

        if ( pick )
        {
          SelectionModes::SelectionMode selectMode;

          if (e.MiddleUp())
          {
            selectMode = SelectionModes::Manifest;
          }
          else
          {
            if ( e.ControlDown() )
            {
              if ( e.ShiftDown())
              {
                selectMode = SelectionModes::Add;
              }
              else
              {
                selectMode = SelectionModes::Remove;
              }
            }
            else
            {
              if ( e.ShiftDown())
              {
                selectMode = SelectionModes::Toggle;
              }
              else
              {
                selectMode = SelectionModes::Replace;
              }
            }
          }

          m_Select.Raise( SelectArgs (pick, selectMode, targetMode) );
          delete pick;
          pick = NULL;
        }

        // our drag is complete now
        dragComplete = true;
      }

      break;
    }

  case DragModes::Camera:
    {
      if (e.AltDown())
      {
        m_Cameras[m_CameraMode].MouseUp(e);
      }

      break;
    }

  default:
    {
      if (m_Tool)
      {
        m_Tool->MouseUp(e);
      }

      break;
    }
  }

  if (!e.LeftIsDown() && !e.MiddleIsDown() && !e.RightIsDown())
  {
    if (GetCapture() == this)
    {
      ReleaseMouse();
    }
  }

  if (dragComplete)
  {
    m_DragMode = DragModes::None;
  }

  Refresh();
  Update();
}

void Viewport::OnMouseMove(wxMouseEvent& e)
{
  // Grab focus on mouse move if our top level window is active
  if ( !m_Focused )
  {
    wxWindow* parent = GetParent();
    if ( parent->IsTopLevel() )
    {
      wxTopLevelWindow* topLevel = reinterpret_cast< wxTopLevelWindow* >( parent );
      if ( topLevel->IsActive() )
      {
        SetFocus();
      }
    }
  }

  if (m_Focused)
  {
#pragma TODO("Freeze here -Geoff")

    // this sucks, be we don't get a keyup for alt
    if (m_DragMode == DragModes::Camera && !e.AltDown())
    {
      m_DragMode = DragModes::None;
    }

    int highlightStartX = -1, highlightStartY = -1, highlightEndX = -1, highlightEndY = -1;

    switch (m_DragMode)
    {
    case DragModes::None:
      {
        // setup highlight
        if ( m_Highlighting && (!m_Tool || m_Tool->AllowSelection()) )
        {
          highlightStartX = highlightEndX = e.GetX();
          highlightStartY = highlightEndY = e.GetY();
        }

        if ( m_Tool )
        {
          m_Tool->MouseMove(e);
        }

        break;
      }

    case DragModes::Select:
      {
        m_End = Math::Point (e.GetX(), e.GetY());
        m_SelectionFrame->m_End = m_End;
        m_SelectionFrame->Update();

        if ( !m_Tool || m_Tool->AllowSelection() )
        {
          // setup highlight
          highlightStartX = m_SelectionFrame->m_Start.x;
          highlightStartY = m_SelectionFrame->m_Start.y;
          highlightEndX = e.GetX();
          highlightEndY = e.GetY();
        }

        break;
      }

    case DragModes::Camera:
      {
        if (e.AltDown())
        {
          m_Cameras[m_CameraMode].MouseMove(e);
        }

        break;
      }

    case DragModes::Tool:
      {
        if (m_Tool)
        {
          if ( m_Tool->AllowSelection() )
          { 
            // setup highlight
            highlightStartX = highlightEndX = e.GetX();
            highlightStartY = highlightEndY = e.GetY();
          }

          m_Tool->MouseMove(e);
        }
        else
        {
          NOC_BREAK();
        }

        break;
      }
    }

    if (highlightStartX != -1 && highlightStartY != -1 && highlightEndX != -1 && highlightEndY != -1)
    {
      PickVisitor* pick = NULL;
      SelectionTargetMode targetMode = SelectionTargetModes::Single;

      if (highlightStartX == highlightEndX && highlightStartY == highlightEndY)
      {         
        pick = new FrustumLinePickVisitor (&m_Cameras[m_CameraMode], highlightStartX, highlightStartY);
        targetMode = SelectionTargetModes::Single;
      }
      else 
      {
        Frustum worldSpaceFrustum;

        if ( m_Cameras[m_CameraMode].ViewportToFrustum(highlightStartX, highlightStartY, highlightEndX, highlightEndY, worldSpaceFrustum) )
        {
          Math::Point center ( Point (highlightStartX, highlightStartY) + Point (highlightEndX, highlightEndY) / 2 );

          Math::Line line;
          m_Cameras[m_CameraMode].ViewportToLine(center.x, center.y, line);

          pick = new FrustumLinePickVisitor( &m_Cameras[m_CameraMode], line, worldSpaceFrustum );
          targetMode = SelectionTargetModes::Multiple;
        }
      }

      if (pick)
      {
        SetHighlightArgs args (pick, targetMode);
        m_SetHighlight.Raise( args );
        delete pick;
        pick = NULL;
      }
    }

    Refresh();
    Update();
  }
}

void Viewport::OnMouseScroll(wxMouseEvent& e)
{
  if (m_Focused)
  {
    m_Cameras[m_CameraMode].MouseScroll(e);

    Refresh();
    Update();
  }
}

void Viewport::OnMouseLeave(wxMouseEvent& e)
{
  if (m_Focused)
  {
    // if the mouse skips out without a move event, make sure to clear our highlight
    m_ClearHighlight.Raise( ClearHighlightArgs (true) );
  }
}

void Viewport::OnMouseCaptureLost(wxMouseCaptureLostEvent& e)
{
  // If this function does not exist, then an assert is thrown off by the wxWidgets 
  // framework when you hold down either the left or middle mouse buttons and click
  // the right mouse button.
}

void Viewport::Draw()
{
  if ( !m_D3DManager.TestDeviceReady() )
  {
    return;
  }

  IDirect3DDevice9* device = GetDevice();
  if ( !device )
  {
    return;
  }

  LUNA_SCENE_DRAW_SCOPE_TIMER( ("") );

  u64 start = Platform::TimerGetClock();

  DrawArgs args;

  // this seems like a bad place to do this
  if (m_Tool)
  {
    LUNA_SCENE_DRAW_SCOPE_TIMER( ("Tool Evaluate") );
    
    m_Tool->Evaluate();
  }


  //
  // Begin Rendering
  //
  {
    LUNA_SCENE_DRAW_SCOPE_TIMER( ("Clear and Reset Scene") );

    device->BeginScene();

    device->SetRenderTarget( 0, m_D3DManager.GetBackBuffer() );
    device->SetDepthStencilSurface( m_D3DManager.GetDepthBuffer() );

    device->Clear(NULL, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255, 80, 80, 80), 1.0f, 0);

    device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Math::Matrix4::Identity);

    m_ResourceTracker->ResetState();
  }


  //
  // Set Camera Transforms
  //

  {
    LUNA_SCENE_DRAW_SCOPE_TIMER( ("Setup Viewport and Projection") );

    // proj
    int w, h;
    GetSize(&w, &h);
    device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&m_Cameras[m_CameraMode].SetProjection(w, h));

    // view
    device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&m_Cameras[m_CameraMode].GetViewport());
  }


  {
    LUNA_SCENE_DRAW_SCOPE_TIMER( ("Set RenderState (culling, lighting, and fill") );


    //
    // Set default blending equations
    //

    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


    //
    // Set Camera State
    //

    if (m_Cameras[m_CameraMode].IsBackFaceCulling())
    {
      device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    }
    else
    {
      device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    }


    //
    // Set Lights
    //

    device->SetRenderState(D3DRS_LIGHTING, TRUE);
    device->SetRenderState(D3DRS_COLORVERTEX, FALSE);
    device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
    device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
    device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);

    device->SetRenderState(D3DRS_ZENABLE, TRUE);
    device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
    device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

    device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    device->SetPixelShader( NULL );
    device->SetVertexShader( NULL );

    D3DLIGHT9 light;    
    ZeroMemory(&light, sizeof(light));

    D3DCOLORVALUE ambient;
    D3DCOLORVALUE diffuse;
    D3DCOLORVALUE specular;
    if ( m_Cameras[m_CameraMode].GetShadingMode() == ShadingModes::Wireframe )
    {
      ambient = Luna::Color::DIMGRAY;
      diffuse = Luna::Color::BLACK;
      specular = Luna::Color::BLACK;
    }
    else
    {
      ambient = Luna::Color::DIMGRAY;
      diffuse = Luna::Color::SILVER;
      specular = Luna::Color::SILVER;
    }

    Vector3 dir;
    m_Cameras[m_CameraMode].GetDirection(dir);

    // setup light
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Ambient = ambient;
    light.Diffuse = diffuse;
    light.Specular = specular;

    // set light into runtime
    light.Direction = *(D3DVECTOR*)&dir;
    device->SetLight(0, &light);
    device->LightEnable(0, true);

    // light from the back
    dir *= -1.0f;

    // set light into runtime
    light.Direction = *(D3DVECTOR*)&dir;
    device->SetLight(1, &light);
    device->LightEnable(1, true);
  }


  //
  // Render
  //

  {
    LUNA_SCENE_DRAW_SCOPE_TIMER( ("PreRender") );

    PreDraw( &args );
  }

  {
    LUNA_SCENE_DRAW_SCOPE_TIMER( ("Render") );

    {
      LUNA_SCENE_DRAW_SCOPE_TIMER( ("Render Setup") );

      m_RenderVisitor.Reset( &args, this );
    }

    {
      LUNA_SCENE_DRAW_SCOPE_TIMER( ("Render Walk") );

      m_Render.Raise( &m_RenderVisitor );
    }

    if (m_Tool)
    {
      LUNA_SCENE_DRAW_SCOPE_TIMER( ("Render Tool") );

      m_Tool->Draw( &args );
    }

    {
      LUNA_SCENE_DRAW_SCOPE_TIMER( ("Render Draw") );

      m_RenderVisitor.Draw();
    }

    args.m_EntryCount = m_RenderVisitor.GetSize();
  }

  {
    LUNA_SCENE_DRAW_SCOPE_TIMER( ("Post Render") );

    PostDraw( &args );
  }


  //
  // Stats
  //

  {
    LUNA_SCENE_DRAW_SCOPE_TIMER( ("Process Statistics") );

    m_Statistics->m_FrameNumber++;
    m_Statistics->m_FrameCount++;

    m_Statistics->m_RenderTime += Platform::CyclesToMillis( Platform::TimerGetClock() - start );
    m_Statistics->m_RenderWalkTime += args.m_WalkTime;
    m_Statistics->m_RenderSortTime += args.m_SortTime;
    m_Statistics->m_RenderCompareTime += args.m_CompareTime;
    m_Statistics->m_RenderDrawTime += args.m_DrawTime;

    m_Statistics->m_EntryCount += args.m_EntryCount;
    m_Statistics->m_TriangleCount += args.m_TriangleCount;
    m_Statistics->m_LineCount += args.m_LineCount;

    m_Statistics->Update();

    if (m_StatisticsVisible)
    {
      m_Statistics->Draw(&args);
    }
  }


  {
    LUNA_SCENE_DRAW_SCOPE_TIMER( ("End Scene") );

    //
    // End Rendering
    //

    device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    device->EndScene();

  }


  {
    LUNA_SCENE_DRAW_SCOPE_TIMER( ("Present") );

    //
    // Buffer Swap
    //

    if ( m_D3DManager.Display( GetHwnd() ) == D3DERR_DEVICELOST )
    {
      m_D3DManager.SetDeviceLost();
    }
  }

  return;
}

void Viewport::UndoTransform( )
{
  UndoTransform( m_CameraMode );
}

void Viewport::UndoTransform( CameraMode camMode )
{
  m_CameraHistory[camMode].Undo();
  Refresh();
}

void Viewport::RedoTransform( )
{
  RedoTransform( m_CameraMode );
}

void Viewport::RedoTransform( CameraMode camMode )
{
  m_CameraHistory[camMode].Redo();
  Refresh();
}

void Viewport::UpdateCameraHistory( )
{  
  // We only work for the Orbit camera at the moment. SetTransform assumes a Perspective view
  // Not sure how to handle orthographic at the moment.
  if( m_CameraMode != CameraModes::Orbit )
  {
    return;
  }

  m_CameraHistory[m_CameraMode].Push( new CameraMovedCommand( this, &m_Cameras[m_CameraMode] ) );
}

void Viewport::PreDraw( DrawArgs* args )
{
  IDirect3DDevice9* device = GetDevice();
  device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix4::Identity);

  if (m_GridVisible)
  {
    m_GlobalPrimitives[GlobalPrimitives::StandardGrid]->Draw( args );
  }
}

void Viewport::PostDraw( DrawArgs* args )
{
  IDirect3DDevice9* device = GetDevice();
  device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix4::Identity);


  //
  // Render Helpers
  //

  device->Clear(NULL, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

  if (m_AxesVisible)
  {
    static_cast<Luna::PrimitiveAxes*>(m_GlobalPrimitives[GlobalPrimitives::ViewportAxes])->DrawViewport(args, &m_Cameras[m_CameraMode]);
  }


  //
  // Draw Tool
  //

  if (m_Tool)
  {
    m_Tool->Draw(args);
  }


  //
  // Draw Frames
  //

  if (m_Focused)
  {
    unsigned w = 3;
    unsigned x = GetSize().x;
    unsigned y = GetSize().y;

    wxColour temp = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    u32 color = D3DCOLOR_ARGB(255, temp.Red(), temp.Green(), temp.Blue());

    std::vector< TransformedColored > vertices;

    //   <--
    //  | \  ^
    //  v  \ |
    //   -->

    // top
    vertices.push_back(TransformedColored ((float)0,    (float)0,     1.0f,   color));
    vertices.push_back(TransformedColored ((float)0,    (float)w,     1.0f,   color));
    vertices.push_back(TransformedColored ((float)x,    (float)w,     1.0f,   color));
    vertices.push_back(TransformedColored ((float)x,    (float)0,     1.0f,   color));
    vertices.push_back(TransformedColored ((float)0,    (float)0,     1.0f,   color));

    // bottom
    vertices.push_back(TransformedColored ((float)0,    (float)y-w,   1.0f,   color));
    vertices.push_back(TransformedColored ((float)0,    (float)y,     1.0f,   color));
    vertices.push_back(TransformedColored ((float)x,    (float)y,     1.0f,   color));
    vertices.push_back(TransformedColored ((float)x,    (float)y-w,   1.0f,   color));
    vertices.push_back(TransformedColored ((float)0,    (float)y-w,   1.0f,   color));

    // left
    vertices.push_back(TransformedColored ((float)0,    (float)0,     1.0f,   color));
    vertices.push_back(TransformedColored ((float)0,    (float)y,     1.0f,   color));
    vertices.push_back(TransformedColored ((float)w,    (float)y,     1.0f,   color));
    vertices.push_back(TransformedColored ((float)w,    (float)0,     1.0f,   color));
    vertices.push_back(TransformedColored ((float)0,    (float)0,     1.0f,   color));

    // right
    vertices.push_back(TransformedColored ((float)x-w,  (float)0,     1.0f,   color));
    vertices.push_back(TransformedColored ((float)x-w,  (float)y,     1.0f,   color));
    vertices.push_back(TransformedColored ((float)x,    (float)y,     1.0f,   color));
    vertices.push_back(TransformedColored ((float)x,    (float)0,     1.0f,   color));
    vertices.push_back(TransformedColored ((float)x-w,  (float)0,     1.0f,   color));

    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetFVF(ElementFormats[ElementTypes::TransformedColored]);
    device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &vertices.front(), sizeof(TransformedColored));
    device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &(vertices[5]), sizeof(TransformedColored));
    device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &(vertices[10]), sizeof(TransformedColored));
    device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &(vertices[15]), sizeof(TransformedColored));
    device->SetRenderState(D3DRS_ZENABLE, TRUE);

    m_ResourceTracker->ResetState();
  }

  if (m_DragMode == DragModes::Select)
  {
    m_SelectionFrame->Draw(args);
  }
}

void Viewport::OnReleaseResources( const DeviceStateArgs& args )
{
  m_ResourceTracker->DeviceLost();
  m_Statistics->Delete();
}

void Viewport::OnAllocateResources( const DeviceStateArgs& args )
{
  m_ResourceTracker->DeviceReset();
  m_Statistics->Create();
}

void Viewport::CameraMoved( const Luna::CameraMovedArgs& args )
{
  m_CameraMoved.Raise( args );  
}

void Viewport::RemoteCameraMoved( const Math::Matrix4& transform )
{
  m_Cameras[ CameraModes::Orbit ].SetTransform( transform );
}

void Viewport::OnGridPreferencesChanged( const Reflect::ElementChangeArgs& args )
{
  GridPreferences* gridPreferences = SceneEditorPreferences()->GetGridPreferences();
  Luna::PrimitiveGrid* grid = (Luna::PrimitiveGrid*) m_GlobalPrimitives[GlobalPrimitives::StandardGrid];

  grid->m_Width = gridPreferences->GetWidth();
  grid->m_Length = gridPreferences->GetLength();
  grid->m_MajorStep = gridPreferences->GetMajorStep();
  grid->m_MinorStep = gridPreferences->GetMinorStep();
  grid->SetAxisColor( gridPreferences->GetAxisColor().r, gridPreferences->GetAxisColor().g, gridPreferences->GetAxisColor().b, 0xFF );
  grid->SetMajorColor( gridPreferences->GetMajorColor().r, gridPreferences->GetMajorColor().g, gridPreferences->GetMajorColor().b, 0xFF );
  grid->SetMinorColor( gridPreferences->GetMinorColor().r, gridPreferences->GetMinorColor().g, gridPreferences->GetMinorColor().b, 0xFF );
  grid->Update();
  Refresh();
}
