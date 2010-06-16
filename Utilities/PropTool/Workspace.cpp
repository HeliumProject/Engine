#include "Workspace.h"
#include "Camera.h"

#include "Application/UI/FileDialog.h"
#include "Foundation/Math/Utils.h"

static const u32 s_InvalidMesh = (u32)(-1);

BEGIN_EVENT_TABLE( Workspace, wxWindow )
EVT_SIZE( Workspace::OnSize )
EVT_PAINT( Workspace::OnPaint )
EVT_MOUSE_CAPTURE_LOST( Workspace::OnMouseCaptureLost )
EVT_LEFT_DOWN( Workspace::OnMouseDown )
EVT_LEFT_UP( Workspace::OnMouseUp )
EVT_RIGHT_DOWN( Workspace::OnMouseDown )
EVT_RIGHT_UP( Workspace::OnMouseUp )
EVT_MIDDLE_DOWN( Workspace::OnMouseDown )
EVT_MIDDLE_UP( Workspace::OnMouseUp )
EVT_MOTION( Workspace::OnMouseMove )
EVT_MOUSEWHEEL( Workspace::OnMouseWheel )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Workspace::Workspace( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxPanel( parent, id, pos, size, style, name )
, m_Scene( NULL )
//, m_Light( NULL )
, m_IsDeviceLost( false )
, m_MeshHandle( s_InvalidMesh )
, m_DisplayAxis( false )
, m_AxisSubMenu( NULL )
, m_AxisOnMenuID( -1 )
, m_AxisOffMenuID( -1 )
{
  // Don't erase background
  SetBackgroundStyle( wxBG_STYLE_CUSTOM );

  // Rendering
  m_Render.Init( (HWND)GetHWND(), 513, 541, 0 );
  m_Scene = new igDXRender::Scene( &m_Render );
  m_Scene->m_normalscale = 0.1f;     // set the scale of the normals before loading the mesh (default scale is 1.0)
  m_Scene->m_render_reference_grid = false;
  m_Scene->m_render_wireframe = false;
  m_Scene->m_render_env_cube = false;
  // Context menu
  wxMenuItem* current = m_ContextMenu.Append( wxID_ANY, "Frame" );
  Connect( current->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Workspace::OnFrame ), NULL, this );

  m_AxisSubMenu = new wxMenu();
  current = m_AxisSubMenu->Append( wxID_ANY, "On", wxEmptyString, wxITEM_CHECK );
  m_AxisOnMenuID = current->GetId();
  Connect( current->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Workspace::OnChangeAxisDisplay ), NULL, this );
  current = m_AxisSubMenu->Append( wxID_ANY, "Off", wxEmptyString, wxITEM_CHECK );
  m_AxisOffMenuID = current->GetId();
  Connect( current->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Workspace::OnChangeAxisDisplay ), NULL, this );
  m_ContextMenu.AppendSubMenu( m_AxisSubMenu, "Axis" );

  wxMenu* screenShotSubMenu = new wxMenu();
  current = screenShotSubMenu->Append( wxID_ANY, "Save to file..." );
  Connect( current->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Workspace::OnScreenShotToFile ), NULL, this );
  m_ContextMenu.AppendSubMenu( screenShotSubMenu, "Screenshot" );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
//  
Workspace::~Workspace()
{
  delete m_Scene;
}

///////////////////////////////////////////////////////////////////////////////
// Display the specified mesh.  Path should point to a static content file.
// 
bool Workspace::LoadScene( const std::string& path, igDXRender::ObjectLoader* loader )
{
  NOC_ASSERT( m_MeshHandle == s_InvalidMesh );
  m_MeshHandle = m_Scene->LoadMesh( path.c_str(), loader );
  if ( m_MeshHandle != s_InvalidMesh )
  {
    // load and set the default env (its always loaded, this just finds the handle)
    m_Scene->SetEnvironmentHandle(m_Scene->LoadEnvironment("@@default",0x40404040));
    //m_Scene->SetEnvironmentHandle(m_Scene->LoadEnvironment("x:\\cube.dds",0x80808080));
    //m_Scene->m_render_env_cube = true;

    m_Scene->SetMeshHandle( m_MeshHandle );
    D3DXMatrixIdentity( &m_Scene->m_worldmat );
    m_Scene->m_render_reference_grid = m_DisplayAxis;
    Frame();
    Refresh();
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Renders the scene if there is one.  Returns true if the scene is valid and
// has at least one mesh.  Otherwise, returns false.
// 
bool Workspace::RenderScene()
{
  if ( m_MeshHandle != s_InvalidMesh )
  {
//    Math::Vector3 dir;
//    m_Camera.GetDirection(dir);
//    dir = Math::Vector3::Zero - dir; // wierd?  why?  -Geoff

    m_Scene->m_viewmat = *( ( D3DMATRIX* )( &m_Camera.GetView() ) );
    m_Scene->m_projmat = *( ( D3DMATRIX* )( &m_Camera.SetProjection( m_Scene->m_width, m_Scene->m_height ) ) );
    m_Render.RenderScene( m_Scene );
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Remove the currently displayed mesh from the scene.
void Workspace::ClearScene()
{
  m_MeshHandle = s_InvalidMesh;
  m_Scene->SetMeshHandle( m_MeshHandle );
  m_Camera.Reset();
  Refresh();
}

///////////////////////////////////////////////////////////////////////////////
// Draw the scene.
// 
void Workspace::Draw()
{
  IDirect3DDevice9* device = m_Render.GetD3DDevice();
  if ( !device )
  {
    return;
  }

  if ( m_IsDeviceLost )
  {
    if ( device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET )
    {
      m_Render.Reset();
      m_IsDeviceLost = device->TestCooperativeLevel() != D3D_OK;
    }
  }

  if ( m_IsDeviceLost )
  {
    return;
  }

  // If we have a scene with a mesh, draw it
  if ( !RenderScene() )
  {
    // No scene or no mesh, just clear the screen
    device->BeginScene();
    device->SetRenderTarget( 0, m_Render.GetBackBuffer() );
    device->SetDepthStencilSurface( m_Render.GetDepthBuffer() );
    device->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xFF, 0xB8, 0xB8, 0xB8), 1.0f, 0 );
    device->EndScene();
  }

  m_IsDeviceLost = ( m_Render.Display( (HWND)GetHWND() ) == D3DERR_DEVICELOST );
}

///////////////////////////////////////////////////////////////////////////////
// Resizes the renderer.
// 
void Workspace::Resize( const wxSize& size )
{
  if ( size.x > 0 && size.y > 0 )
  {
    if ( size.x != m_Render.GetWidth() || size.y != m_Render.GetHeight() )
    {
      m_Render.Resize( size.x, size.y );

      m_Scene->m_width = size.x;
      m_Scene->m_height = size.y;

      Refresh();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Show a context menu of operations.  Derived classes can override this to 
// completely replace the menu that is show (or do nothing).
// 
void Workspace::ShowContextMenu( const wxPoint& pos )
{
  m_AxisSubMenu->Check( m_AxisOnMenuID, m_Scene->m_render_reference_grid );
  m_AxisSubMenu->Check( m_AxisOffMenuID, !m_Scene->m_render_reference_grid );

  PopupMenu( &m_ContextMenu, pos );
}

///////////////////////////////////////////////////////////////////////////////
// Resets the view so that the mesh is centered in the view.
// 
void Workspace::Frame()
{
  if ( m_MeshHandle != s_InvalidMesh )
  {
    const Math::Vector3& min = *( const Math::Vector3* )( &m_Scene->m_min );
    const Math::Vector3& max = *( const Math::Vector3* )( &m_Scene->m_max );

    Math::AlignedBox box;
    box.Merge( min );
    box.Merge( max );

    m_Camera.Frame( box );
    Refresh();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Turns the reference Axis on or off.
// 
void Workspace::DisplayReferenceAxis( bool display )
{
  if ( m_DisplayAxis != display )
  {
    m_DisplayAxis = display;
    m_Scene->m_render_reference_grid = display;
    Refresh();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Saves a screenshot to the specified location.  Returns true if the screenshot
// was saved.
// 
bool Workspace::SaveScreenShotAs( const std::string& path )
{
  if ( RenderScene() )
  {
    return m_Render.SaveTGA( path.c_str() );
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Handle size event, update the D3D device.
// 
void Workspace::OnSize( wxSizeEvent& args )
{
  // Do custom work for your window here
  Resize( args.GetSize() );
}

///////////////////////////////////////////////////////////////////////////////
// Handle all D3D drawing.
// 
void Workspace::OnPaint( wxPaintEvent& args )
{
  Draw();

  ::ValidateRect( ( HWND )GetHandle(), NULL );
}

///////////////////////////////////////////////////////////////////////////////
// Required by wxWidgets.
// 
void Workspace::OnMouseCaptureLost( wxMouseCaptureLostEvent& args )
{
  // We don't have anything to clean up
}

///////////////////////////////////////////////////////////////////////////////
// Start tracking mouse movement for moving the camera.
// 
void Workspace::OnMouseDown( wxMouseEvent& args )
{
  args.Skip();

  bool modifier = args.ControlDown() || args.MetaDown() || args.ShiftDown() || args.AltDown();
  // Right-click with no modifier keys means to show a context menu
  if ( args.RightDown() && !modifier )
  {
    ShowContextMenu( args.GetPosition() );
  }
  else
  {
    CaptureMouse();
    m_Camera.MouseDown( args );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Stop tracking mouse movement for moving the camera.
// 
void Workspace::OnMouseUp( wxMouseEvent& args )
{
  args.Skip();
  if ( HasCapture() )
  {
    ReleaseMouse();
  }
  m_Camera.MouseUp( args );
}

///////////////////////////////////////////////////////////////////////////////
// Move the camera.
// 
void Workspace::OnMouseMove( wxMouseEvent& args )
{
  args.Skip();

  if ( HasCapture() )
  {
    m_Camera.MouseMove( args );
    Refresh();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Zooms the camera in and out.
// 
void Workspace::OnMouseWheel( wxMouseEvent& args )
{
  args.Skip();

  m_Camera.MouseScroll( args );
  Refresh();
}

///////////////////////////////////////////////////////////////////////////////
// Prompt for a location and save a screenshot there.
// 
void Workspace::OnScreenShotToFile( wxCommandEvent& args )
{
  Nocturnal::FileDialog dialog( this, wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, "*.tga", Nocturnal::FileDialogStyles::DefaultSave );
  if ( dialog.ShowModal() == wxID_OK )
  {
    std::string path = dialog.GetPath();
    if ( !path.empty() )
    {
      SaveScreenShotAs( path );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Center the mesh.
// 
void Workspace::OnFrame( wxCommandEvent& args )
{
  Frame();
}

///////////////////////////////////////////////////////////////////////////////
// Toggle visibility of the reference Axis.
// 
void Workspace::OnChangeAxisDisplay( wxCommandEvent& args )
{
  DisplayReferenceAxis( args.GetId() == m_AxisOnMenuID );
}
