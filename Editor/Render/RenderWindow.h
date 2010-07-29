#pragma once

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Render/Renderer.h"
#include "Camera.h"

namespace Render
{
  struct D3DEventArgs;

  typedef std::vector< Render::Scene* > V_Scene;
  
  class RenderWindow : public wxWindow
  {
  public:
    RenderWindow( wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE, const wxString& name = wxT( "Editor::RenderWindow" ) );
    virtual ~RenderWindow();

    bool LoadScene( const tstring& path );
    void ClearScene();
    
    virtual bool SaveScreenShotAs( const tstring& path );

    virtual void DisplayReferenceAxis( bool display );

    virtual void Frame();
    
  protected:
    virtual void SetupLighting( Render::Scene* scene );
    virtual void Draw();
    virtual void Resize( const wxSize& size );
    virtual void ShowContextMenu( const wxPoint& pos );

    bool RenderScene();

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
    Render::Renderer m_Render;
    Render::Scene* m_Scene;
    bool m_IsDeviceLost;
    Editor::Camera m_Camera;
    u32 m_MeshHandle;
    wxMenu m_ContextMenu;
    bool m_DisplayAxis;
    wxMenu* m_AxisSubMenu;
    i32 m_AxisOnMenuID;
    i32 m_AxisOffMenuID;
    
  private:
    DECLARE_EVENT_TABLE();
  };
}
