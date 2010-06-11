#pragma once

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"

#include "igDXRender/Render.h"

#include "wx/wx.h"

#include "Camera.h"

struct D3DEventArgs;

class Workspace : public wxPanel
{
public:
  Workspace( wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE, const wxString& name = "Luna::Workspace" );
  virtual ~Workspace();

  igDXRender::Scene* GetScene()
  {
    return m_Scene;
  }

  bool LoadScene( const std::string& path, igDXRender::ObjectLoader* loader = NULL );
  bool RenderScene();
  void ClearScene();
  
  void Draw();
  void Resize( const wxSize& size );
  void ShowContextMenu( const wxPoint& pos );

  void Frame();
  void DisplayReferenceAxis( bool display );
  bool SaveScreenShotAs( const std::string& path );

private:
  void OnSize( wxSizeEvent& args );
  void OnPaint( wxPaintEvent& args );

  void OnMouseCaptureLost( wxMouseCaptureLostEvent& args );

  void OnMouseDown( wxMouseEvent& args );
  void OnMouseUp( wxMouseEvent& args );
  void OnMouseMove( wxMouseEvent& args );
  void OnMouseWheel( wxMouseEvent& args );

  void OnScreenShotToFile( wxCommandEvent& args );
  void OnFrame( wxCommandEvent& args );
  void OnChangeAxisDisplay( wxCommandEvent& args );

protected:
  igDXRender::Render m_Render;
  igDXRender::Scene* m_Scene;
  //igDXRender::Light* m_Light;
  bool m_IsDeviceLost;
  Camera m_Camera;
  u32 m_MeshHandle;
  wxMenu m_ContextMenu;
  bool m_DisplayAxis;
  wxMenu* m_AxisSubMenu;
  i32 m_AxisOnMenuID;
  i32 m_AxisOffMenuID;

private:
  DECLARE_EVENT_TABLE();
};
