#pragma once

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Render/Renderer.h"
#include "Camera.h"
#include "BangleWindow.h"

namespace Luna
{
  struct D3DEventArgs;
  
  typedef struct
  {
    Render::Scene* m_Scene;
    bool m_Draw;
  } BangleScene;
  
  typedef std::map< u32, BangleScene > M_BangleScene;
  typedef std::vector< Render::Scene* > V_Scene;
  
  class PreviewWindow : public wxWindow
  {
  public:
    PreviewWindow( wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE, const wxString& name = "Luna::PreviewWindow" );
    virtual ~PreviewWindow();

    bool LoadScene( const std::string& path );
    void ClearScene();
    
    virtual bool SaveScreenShotAs( const std::string& path );

    virtual void DisplayReferenceAxis( bool display );

    virtual void Frame();
    
    const M_BangleScene& GetBangleScenes();
    void SetBangleDraw( u32 bangleIndex, bool draw );

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
    void OnBangles( wxCommandEvent& args );

  protected:
    Render::Renderer m_Render;
    Render::Scene* m_Scene;
    bool m_IsDeviceLost;
    Luna::Camera m_Camera;
    u32 m_MeshHandle;
    wxMenu m_ContextMenu;
    bool m_DisplayAxis;
    wxMenu* m_AxisSubMenu;
    i32 m_AxisOnMenuID;
    i32 m_AxisOffMenuID;
    
    BangleWindow* m_BangleWindow;
    M_BangleScene m_BangleScenes;

  private:
    DECLARE_EVENT_TABLE();
  };
}
