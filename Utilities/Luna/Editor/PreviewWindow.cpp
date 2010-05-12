#include "Precompile.h"
#include "PreviewWindow.h"

#include "Finder/ExtensionSpecs.h"
#include "Math/Utils.h"
#include "igDXContent/ContentLoader.h"
#include "Scene/Camera.h"
#include "UIToolKit/FileDialog.h"

using namespace Luna;

static const u32 s_InvalidMesh = (u32)(-1);

BEGIN_EVENT_TABLE( PreviewWindow, wxWindow )
EVT_SIZE( PreviewWindow::OnSize )
EVT_PAINT( PreviewWindow::OnPaint )
EVT_MOUSE_CAPTURE_LOST( PreviewWindow::OnMouseCaptureLost )
EVT_LEFT_DOWN( PreviewWindow::OnMouseDown )
EVT_LEFT_UP( PreviewWindow::OnMouseUp )
EVT_RIGHT_DOWN( PreviewWindow::OnMouseDown )
EVT_RIGHT_UP( PreviewWindow::OnMouseUp )
EVT_MIDDLE_DOWN( PreviewWindow::OnMouseDown )
EVT_MIDDLE_UP( PreviewWindow::OnMouseUp )
EVT_MOTION( PreviewWindow::OnMouseMove )
EVT_MOUSEWHEEL( PreviewWindow::OnMouseWheel )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
PreviewWindow::PreviewWindow( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
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
    m_Scene = new igDXRender::Scene( &m_Render );
    m_Scene->m_normalscale = 0.1f;     // set the scale of the normals before loading the mesh (default scale is 1.0)
    m_Scene->m_render_reference_grid = false;
    m_Scene->m_render_wireframe = false;
    m_Scene->m_render_env_cube = false;
    igDXRender::Light* t1 = new igDXRender::Light();
    t1->m_color = D3DXVECTOR4( 1, 1, 1, 0 );
    t1->m_direction = D3DXVECTOR4( 0, 1, 0, 0 );
    m_Scene->m_lights.push_back( t1 );
  }

  // Context menu
  wxMenuItem* current = m_ContextMenu.Append( wxID_ANY, "Frame" );
  Connect( current->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PreviewWindow::OnFrame ), NULL, this );
  
  current = m_ContextMenu.Append( wxID_ANY, "Bangles..." );
  Connect( current->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PreviewWindow::OnBangles ), NULL, this );

  m_AxisSubMenu = new wxMenu();
  current = m_AxisSubMenu->Append( wxID_ANY, "On", wxEmptyString, wxITEM_CHECK );
  m_AxisOnMenuID = current->GetId();
  Connect( current->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PreviewWindow::OnChangeAxisDisplay ), NULL, this );
  current = m_AxisSubMenu->Append( wxID_ANY, "Off", wxEmptyString, wxITEM_CHECK );
  m_AxisOffMenuID = current->GetId();
  Connect( current->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PreviewWindow::OnChangeAxisDisplay ), NULL, this );
  m_ContextMenu.AppendSubMenu( m_AxisSubMenu, "Axis" );

  wxMenu* screenShotSubMenu = new wxMenu();
  current = screenShotSubMenu->Append( wxID_ANY, "Save to file..." );
  Connect( current->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PreviewWindow::OnScreenShotToFile ), NULL, this );
  m_ContextMenu.AppendSubMenu( screenShotSubMenu, "Screenshot" );

  m_BangleWindow = new BangleWindow( this, wxID_ANY, "Bangles" );
  m_BangleWindow->Hide();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
PreviewWindow::~PreviewWindow()
{
  if ( m_BangleWindow )
  {
    RemoveChild( m_BangleWindow );
    delete m_BangleWindow;
  }

  for ( M_BangleScene::iterator itr = m_BangleScenes.begin(), end = m_BangleScenes.end(); itr != end; ++itr )
  {
    delete itr->second.m_Scene;
  }

  delete m_Scene;
}

///////////////////////////////////////////////////////////////////////////////
// Display the specified mesh.  Path should point to a static content file.
// 
bool PreviewWindow::LoadScene( const std::string& path )
{
  NOC_ASSERT( m_MeshHandle == s_InvalidMesh );
  
  igDXContent::IRBObjectLoader loader;
  loader.IncrRefCount();
  if ( m_Scene )
  {
    m_MeshHandle = m_Scene->LoadMesh( path.c_str(), &loader, 0 );
    
    if ( m_MeshHandle != s_InvalidMesh )
    {
      for ( std::map<int, bool>::iterator itr = loader.m_bangleInfo.begin(), end = loader.m_bangleInfo.end(); itr != end; ++itr )
      {
        igDXRender::Scene* scene = new igDXRender::Scene( &m_Render );
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

        scene->SetEnvironmentHandle( scene->LoadEnvironment( "@@default", 0x40404040 ) );
        scene->SetMeshHandle( meshHandle );
        D3DXMatrixIdentity( &scene->m_worldmat );
        scene->m_render_reference_grid = m_DisplayAxis;
        SetupLighting( scene );
        
        BangleScene bangleScene;
        bangleScene.m_Scene = scene;
        bangleScene.m_Draw = itr->second;

        m_BangleScenes.insert( std::make_pair( itr->first, bangleScene ) );
      }

      m_Scene->SetEnvironmentHandle( m_Scene->LoadEnvironment( "@@default", 0x40404040 ) );
      
      m_Scene->SetMeshHandle( m_MeshHandle );
      D3DXMatrixIdentity( &m_Scene->m_worldmat );
      m_Scene->m_render_reference_grid = m_DisplayAxis;
      SetupLighting( m_Scene );
      m_BangleWindow->RefreshBangles();
      Refresh();

      return true;
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Remove the currently displayed mesh from the scene.
void PreviewWindow::ClearScene()
{
  for ( M_BangleScene::iterator itr = m_BangleScenes.begin(), end = m_BangleScenes.end(); itr != end; ++itr )
  {
    delete itr->second.m_Scene;
  }
  m_BangleScenes.clear();
  
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
bool PreviewWindow::SaveScreenShotAs( const std::string& path )
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
void PreviewWindow::DisplayReferenceAxis( bool display )
{
  if ( m_DisplayAxis != display )
  {
    m_DisplayAxis = display;
    if ( m_Scene )
    {
      m_Scene->m_render_reference_grid = display;
      
      for ( M_BangleScene::iterator itr = m_BangleScenes.begin(), end = m_BangleScenes.end(); itr != end; ++itr )
      {
        itr->second.m_Scene->m_render_reference_grid = display;
      }
    }

    Refresh();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Resets the view so that the mesh is centered in the view.
// 
void PreviewWindow::Frame()
{
  if ( m_MeshHandle != s_InvalidMesh && m_Scene )
  {
    const Math::Vector3& min = *( const Math::Vector3* )( &m_Scene->m_min );
    const Math::Vector3& max = *( const Math::Vector3* )( &m_Scene->m_max );

    Math::AlignedBox box;
    box.Merge( min );
    box.Merge( max );
    
    for ( M_BangleScene::iterator itr = m_BangleScenes.begin(), end = m_BangleScenes.end(); itr != end; ++itr )
    {
      const Math::Vector3& bangleMin = *( const Math::Vector3* )( &itr->second.m_Scene->m_min );
      const Math::Vector3& bangleMax = *( const Math::Vector3* )( &itr->second.m_Scene->m_max );
      
      box.Merge( bangleMin );
      box.Merge( bangleMax );
    }

    m_Camera.Frame( box );
    Refresh();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Sets up lighting in the scene.
// 
const M_BangleScene& PreviewWindow::GetBangleScenes()
{
  return m_BangleScenes;
}

///////////////////////////////////////////////////////////////////////////////
// Sets up lighting in the scene.
// 
void PreviewWindow::SetBangleDraw( u32 bangleIndex, bool draw )
{
  M_BangleScene::iterator itr = m_BangleScenes.find( bangleIndex );
  if ( itr != m_BangleScenes.end() )
  {
    itr->second.m_Draw = draw;
    Refresh();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Sets up lighting in the scene.
// 
void PreviewWindow::SetupLighting( igDXRender::Scene* scene )
{
  if ( !scene )
  {
    return;
  }

  for ( std::vector< igDXRender::Light* >::iterator itr = scene->m_lights.begin(), end = scene->m_lights.end(); itr != end; ++itr )
  {
    delete *itr;
  }

  scene->m_lights.clear();
  
  scene->m_ambient.x = 0.4f;
  scene->m_ambient.y = 0.4f;
  scene->m_ambient.z = 0.4f;
  scene->m_ambient.w = 0.0f;
  
  igDXRender::Light* light1 = new igDXRender::Light();
  light1->m_direction = D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 0.0f );
  light1->m_color = D3DXVECTOR4( 0.5f, 0.5f, 0.5f, 0.0f );
  scene->m_lights.push_back( light1 );

  igDXRender::Light* light2 = new igDXRender::Light();
  light2->m_direction = D3DXVECTOR4( -1.0f, -1.0f, -1.0f, 0.0f );
  light2->m_color = D3DXVECTOR4( 0.45f, 0.45f, 0.45f, 0.0f );
  scene->m_lights.push_back( light2 );
}

///////////////////////////////////////////////////////////////////////////////
// Draw the scene.
// 
void PreviewWindow::Draw()
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
void PreviewWindow::Resize( const wxSize& size )
{
  if ( size.x > 0 && size.y > 0 && m_Scene )
  {
    if ( size.x != m_Render.GetWidth() || size.y != m_Render.GetHeight() )
    {
      m_Render.Resize( size.x, size.y );

      m_Scene->m_width = size.x;
      m_Scene->m_height = size.y;
      
      for ( M_BangleScene::iterator itr = m_BangleScenes.begin(), end = m_BangleScenes.end(); itr != end; ++itr )
      {
        itr->second.m_Scene->m_width = size.x;
        itr->second.m_Scene->m_height = size.y;
      }

      Refresh();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Show a context menu of operations.  Derived classes can override this to 
// completely replace the menu that is show (or do nothing).
// 
void PreviewWindow::ShowContextMenu( const wxPoint& pos )
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
bool PreviewWindow::RenderScene()
{
  if ( m_MeshHandle != s_InvalidMesh && m_Scene )
  {
    std::vector< igDXRender::Scene* > renderScenes;
    renderScenes.reserve( m_BangleScenes.size() + 1 );

    m_Scene->m_viewmat = *( ( D3DMATRIX* )( &m_Camera.GetView() ) );
    m_Scene->m_projmat = *( ( D3DMATRIX* )( &m_Camera.SetProjection( m_Scene->m_width, m_Scene->m_height ) ) );
    renderScenes.push_back( m_Scene );

    for ( M_BangleScene::iterator itr = m_BangleScenes.begin(), end = m_BangleScenes.end(); itr != end; ++itr )
    {
      if ( itr->second.m_Draw )
      {
        igDXRender::Scene* scene = itr->second.m_Scene;

        scene->m_viewmat = *( ( D3DMATRIX* )( &m_Camera.GetView() ) );
        scene->m_projmat = *( ( D3DMATRIX* )( &m_Camera.SetProjection( scene->m_width, scene->m_height ) ) );
        renderScenes.push_back( scene );
      }
    }
    
    m_Render.RenderScenes( (int) renderScenes.size(), &renderScenes[ 0 ] );
    
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Handle size event, update the D3D device.
// 
void PreviewWindow::OnSize( wxSizeEvent& args )
{
  // Do custom work for your window here
  Resize( args.GetSize() );
}

///////////////////////////////////////////////////////////////////////////////
// Handle all D3D drawing.
// 
void PreviewWindow::OnPaint( wxPaintEvent& args )
{
  Draw();

  ::ValidateRect( ( HWND )GetHandle(), NULL );
}

///////////////////////////////////////////////////////////////////////////////
// Required by wxWidgets.
// 
void PreviewWindow::OnMouseCaptureLost( wxMouseCaptureLostEvent& args )
{
  // We don't have anything to clean up
}

///////////////////////////////////////////////////////////////////////////////
// Start tracking mouse movement for moving the camera.
// 
void PreviewWindow::OnMouseDown( wxMouseEvent& args )
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
void PreviewWindow::OnMouseUp( wxMouseEvent& args )
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
void PreviewWindow::OnMouseMove( wxMouseEvent& args )
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
void PreviewWindow::OnMouseWheel( wxMouseEvent& args )
{
  args.Skip();

  m_Camera.MouseScroll( args );
  Refresh();
}

///////////////////////////////////////////////////////////////////////////////
// Prompt for a location and save a screenshot there.
// 
void PreviewWindow::OnScreenShotToFile( wxCommandEvent& args )
{
  UIToolKit::FileDialog dialog( this, wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, FinderSpecs::Extension::TGA.GetDialogFilter(), UIToolKit::FileDialogStyles::DefaultSave );
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
void PreviewWindow::OnFrame( wxCommandEvent& args )
{
  Frame();
}

///////////////////////////////////////////////////////////////////////////////
// Toggle visibility of the reference Axis.
// 
void PreviewWindow::OnChangeAxisDisplay( wxCommandEvent& args )
{
  DisplayReferenceAxis( args.GetId() == m_AxisOnMenuID );
}

///////////////////////////////////////////////////////////////////////////////
// Displays the bangles window
// 
void PreviewWindow::OnBangles( wxCommandEvent& args )
{
  m_BangleWindow->Show();
}
