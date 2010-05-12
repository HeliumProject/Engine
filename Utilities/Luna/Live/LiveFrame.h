#pragma once

#include "RuntimeConnection.h"
#include "TargetManager/TargetManager.h"
#include "wx/listbook.h"

namespace Luna
{
  class LivePanel;
  class LiveFrame;

  class LiveFrame : public wxFrame
  {
  public:
    LiveFrame( wxWindow *parent, wxWindowID id = wxID_ANY, const wxString& title = "Luna Viewer Link", 
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ),
                long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "Luna Viewer Link" );
    ~LiveFrame();

    void PopulateTargetList( const std::string& toSelect = std::string() );
    void SetAutoStartViewer( bool enable );
    void SetForceAutoStartViewer( bool enable );
    void SetAutoOpenFrame( bool enable );
    void SetLastBackTrace( const std::string& backtrace );
    
    void ConnectionStatusChanged( const RuntimeConnectionStatusArgs& args );

    bool ParseTextForColor( std::string& line, wxColour& color );

    // event handlers
    void OnCloseWindow( wxCloseEvent& evt );
    void OnRefreshTargetList( wxCommandEvent& evt );
    void OnStartViewer( wxCommandEvent& evt );
    void OnResetConnection( wxCommandEvent& evt );
    void OnAutoStartViewer( wxCommandEvent& evt );
    void OnForceAutoStartViewer( wxCommandEvent& evt );
    void OnAutoOpenFrame( wxCommandEvent& evt );
    void OnTargetSelection( wxCommandEvent& evt );
    void OnLightingSelected( wxListbookEvent& evt );
    void OnLightingEnvironmentSelected( wxCommandEvent& evt );
    void OnFilterLightingList( wxCommandEvent& evt );
  

  protected:
    wxAuiManager                m_FrameManager;
    LivePanel*                 m_LivePanel;
  
    DECLARE_EVENT_TABLE()
  };
}