#include "Precompile.h"

#include "Foundation/Math/Utils.h"
#include "Editor/Render/RBObjectLoader.h"
#include "Editor/Render/RenderWindow.h"
#include "Editor/Scene/Camera.h"
#include "Application/UI/FileDialog.h"

using namespace Helium;
using namespace Helium::Render;

static const u32 s_InvalidMesh = (u32)(-1);

BEGIN_EVENT_TABLE( RenderWindow, wxWindow )
EVT_SIZE( RenderWindow::OnSize )
EVT_PAINT( RenderWindow::OnPaint )
EVT_MOUSE_CAPTURE_LOST( RenderWindow::OnMouseCaptureLost )
EVT_LEFT_DOWN( RenderWindow::OnMouseDown )
EVT_LEFT_UP( RenderWindow::OnMouseUp )
EVT_RIGHT_DOWN( RenderWindow::OnMouseDown )
EVT_RIGHT_UP( RenderWindow::OnMouseUp )
EVT_MIDDLE_DOWN( RenderWindow::OnMouseDown )
EVT_MIDDLE_UP( RenderWindow::OnMouseUp )
EVT_MOTION( RenderWindow::OnMouseMove )
EVT_MOUSEWHEEL( RenderWindow::OnMouseWheel )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
RenderWindow::RenderWindow( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxWindow( parent, id, pos, size, style, name )
, m_Scene( NULL )
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
  m_Render.Init( GetHwnd(), 513, 541, 0 );
  if ( m_Render.GetD3DDevice() )
  {
    m_Scene = new Render::Scene( &m_Render );
    m_Scene->m_normalscale = 0.1f;     // set the scale of the normals before loading the mesh (default scale is 1.0)
    m_Scene->m_render_reference_grid = false;
    m_Scene->m_render_wireframe = false;
    m_Scene->m_render_env_cube = false;
    Render::Light* t1 = new Render::Light();
    t1->m_color = D3DXVECTOR4( 1, 1, 1, 0 );
    t1->m_direction = D3DXVECTOR4( 0, 1, 0, 0 );
    m_Scene->m_lights.push_back( t1 );
  }

  // Context menu
  wxMenuItem* current = m_ContextMenu.Append( wxID_ANY, wxT( "Frame" ) );
  Connect( current->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( RenderWindow::OnFrame ), NULL, this );
  
  m_AxisSubMenu = new wxMenu();
  current = m_AxisSubMenu->Append( wxID_ANY, wxT( "On" ), wxEmptyString, wxITEM_CHECK );
  m_AxisOnMenuID = current->GetId();
  Connect( current->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( RenderWindow::OnChangeAxisDisplay ), NULL, this );
  current = m_AxisSubMenu->Append( wxID_ANY, wxT( "Off" ), wxEmptyString, wxITEM_CHECK );
  m_AxisOffMenuID = current->GetId();
  Connect( current->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( RenderWindow::OnChangeAxisDisplay ), NULL, this );
  m_ContextMenu.AppendSubMenu( m_AxisSubMenu, wxT( "Axis" ) );

  wxMenu* screenShotSubMenu = new wxMenu();
  current = screenShotSubMenu->Append( wxID_ANY, wxT( "Save to file..." ) );
  Connect( current->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( RenderWindow::OnScreenShotToFile ), NULL, this );
  m_ContextMenu.AppendSubMenu( screenShotSubMenu, wxT( "Screenshot" ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
RenderWindow::~RenderWindow()
{
  delete m_Scene;
}

///////////////////////////////////////////////////////////////////////////////
// Display the specified mesh.  Path should point to a static content file.
// 
bool RenderWindow::LoadScene( const tstring& path )
{
  HELIUM_ASSERT( m_MeshHandle == s_InvalidMesh );
  
  Content::RBObjectLoader loader;
  loader.IncrRefCount();
  if ( m_Scene )
  {
    m_MeshHandle = m_Scene->LoadMesh( path.c_str(), &loader, 0 );
    
    if ( m_MeshHandle != s_InvalidMesh )
    {
      for ( std::map<int, bool>::iterator itr = loader.m_bangleInfo.begin(), end = loader.m_bangleInfo.end(); itr != end; ++itr )
      {
        Render::Scene* scene = new Render::Scene( &m_Render );
        if ( !scene )
        {
          continue;
        }

        u32 meshHandle = scene->ExtractMesh( path.c_str(), &loader, itr->first );
        if ( meshHandle == s_InvalidMesh )
        {
          delete scene;
          continue;
        }

        scene->m_width = m_Scene->m_width;
        scene->m_height = m_Scene->m_height;

        scene->m_normalscale = 0.1f;     // set the scale of the normals before loading the mesh (default scale is 1.0)
        scene->m_render_reference_grid = false;
        scene->m_render_wireframe = false;
        scene->m_render_env_cube = false;

        scene->SetEnvironmentHandle( scene->LoadEnvironment( TXT( "@@default" ), 0x40404040 ) );
        scene->SetMeshHandle( meshHandle );
        D3DXMatrixIdentity( &scene->m_worldmat );
        scene->m_render_reference_grid = m_DisplayAxis;
        SetupLighting( scene );
      }

      m_Scene->SetEnvironmentHandle( m_Scene->LoadEnvironment( TXT( "@@default" ), 0x40404040 ) );
      
      m_Scene->SetMeshHandle( m_MeshHandle );
      D3DXMatrixIdentity( &m_Scene->m_worldmat );
      m_Scene->m_render_reference_grid = m_DisplayAxis;
      SetupLighting( m_Scene );
      Refresh();

      return true;
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Remove the currently displayed mesh from the scene.
void RenderWindow::ClearScene()
{
  m_MeshHandle = s_InvalidMesh;
  if ( m_Scene )
  {
    m_Scene->SetMeshHandle( m_MeshHandle );
  }

  Refresh();
}

///////////////////////////////////////////////////////////////////////////////
// Saves a screenshot to the specified location.  Returns true if the screenshot
// was saved.
// 
bool RenderWindow::SaveScreenShotAs( const tstring& path )
{
  if ( RenderScene() )
  {
    return m_Render.SaveTGA( path.c_str() );
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Turns the reference Axis on or off.
// 
void RenderWindow::DisplayReferenceAxis( bool display )
{
  if ( m_DisplayAxis != display )
  {
    m_DisplayAxis = display;
    if ( m_Scene )
    {
      m_Scene->m_render_reference_grid = display;
    }

    Refresh();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Resets the view so that the mesh is centered in the view.
// 
void RenderWindow::Frame()
{
  if ( m_MeshHandle != s_InvalidMesh && m_Scene )
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
// Sets up lighting in the scene.
// 
void RenderWindow::SetupLighting( Render::Scene* scene )
{
  if ( !scene )
  {
    return;
  }

  for ( std::vector< Render::Light* >::iterator itr = scene->m_lights.begin(), end = scene->m_lights.end(); itr != end; ++itr )
  {
    delete *itr;
  }

  scene->m_lights.clear();
  
  scene->m_ambient.x = 0.4f;
  scene->m_ambient.y = 0.4f;
  scene->m_ambient.z = 0.4f;
  scene->m_ambient.w = 0.0f;
  
  Render::Light* light1 = new Render::Light();
  light1->m_direction = D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 0.0f );
  light1->m_color = D3DXVECTOR4( 0.5f, 0.5f, 0.5f, 0.0f );
  scene->m_lights.push_back( light1 );

  Render::Light* light2 = new Render::Light();
  light2->m_direction = D3DXVECTOR4( -1.0f, -1.0f, -1.0f, 0.0f );
  light2->m_color = D3DXVECTOR4( 0.45f, 0.45f, 0.45f, 0.0f );
  scene->m_lights.push_back( light2 );
}

///////////////////////////////////////////////////////////////////////////////
// Draw the scene.
// 
void RenderWindow::Draw()
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

  m_IsDeviceLost = ( m_Render.Display( GetHwnd() ) == D3DERR_DEVICELOST );
}

///////////////////////////////////////////////////////////////////////////////
// Resizes the renderer.
// 
void RenderWindow::Resize( const wxSize& size )
{
  if ( size.x > 0 && size.y > 0 && m_Scene )
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
void RenderWindow::ShowContextMenu( const wxPoint& pos )
{
  if ( m_Scene )
  {
    m_AxisSubMenu->Check( m_AxisOnMenuID, m_Scene->m_render_reference_grid );
    m_AxisSubMenu->Check( m_AxisOffMenuID, !m_Scene->m_render_reference_grid );
  }

  PopupMenu( &m_ContextMenu, pos );
}

///////////////////////////////////////////////////////////////////////////////
// Renders the scene if there is one.  Returns true if the scene is valid and
// has at least one mesh.  Otherwise, returns false.
// 
bool RenderWindow::RenderScene()
{
  if ( m_MeshHandle != s_InvalidMesh && m_Scene )
  {
    std::vector< Render::Scene* > renderScenes;

    m_Scene->m_viewmat = *( ( D3DMATRIX* )( &m_Camera.GetViewport() ) );
    m_Scene->m_projmat = *( ( D3DMATRIX* )( &m_Camera.SetProjection( m_Scene->m_width, m_Scene->m_height ) ) );
    renderScenes.push_back( m_Scene );

    m_Render.RenderScenes( (int) renderScenes.size(), &renderScenes[ 0 ] );
    
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Handle size event, update the D3D device.
// 
void RenderWindow::OnSize( wxSizeEvent& args )
{
  // Do custom work for your window here
  Resize( args.GetSize() );
}

///////////////////////////////////////////////////////////////////////////////
// Handle all D3D drawing.
// 
void RenderWindow::OnPaint( wxPaintEvent& args )
{
  Draw();

  ::ValidateRect( ( HWND )GetHandle(), NULL );
}

///////////////////////////////////////////////////////////////////////////////
// Required by wxWidgets.
// 
void RenderWindow::OnMouseCaptureLost( wxMouseCaptureLostEvent& args )
{
  // We don't have anything to clean up
}

///////////////////////////////////////////////////////////////////////////////
// Start tracking mouse movement for moving the camera.
// 
void RenderWindow::OnMouseDown( wxMouseEvent& args )
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
void RenderWindow::OnMouseUp( wxMouseEvent& args )
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
void RenderWindow::OnMouseMove( wxMouseEvent& args )
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
void RenderWindow::OnMouseWheel( wxMouseEvent& args )
{
  args.Skip();

  m_Camera.MouseScroll( args );
  Refresh();
}

///////////////////////////////////////////////////////////////////////////////
// Prompt for a location and save a screenshot there.
// 
void RenderWindow::OnScreenShotToFile( wxCommandEvent& args )
{
  Helium::FileDialog dialog( this, wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, TXT( "" ), Helium::FileDialogStyles::DefaultSave );

  dialog.AddFilter( TXT( "TGA (*.tga)|*.tga" ) );

  if ( dialog.ShowModal() == wxID_OK )
  {
    tstring path = dialog.GetPath();
    if ( !path.empty() )
    {
      SaveScreenShotAs( path );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Center the mesh.
// 
void RenderWindow::OnFrame( wxCommandEvent& args )
{
  Frame();
}

///////////////////////////////////////////////////////////////////////////////
// Toggle visibility of the reference Axis.
// 
void RenderWindow::OnChangeAxisDisplay( wxCommandEvent& args )
{
  DisplayReferenceAxis( args.GetId() == m_AxisOnMenuID );
}
