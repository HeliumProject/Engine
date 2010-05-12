#pragma once

#include "DocumentManager.h"

class MainFrame : public wxFrame
{
public:
  MainFrame( wxWindow *parent = NULL,
                  wxWindowID id = wxID_ANY,
                  const wxString& title = "ProfileView",
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxSize (600, 600),
                  long style = wxDEFAULT_FRAME_STYLE,
                  const wxString& name = wxFrameNameStr );

  ~MainFrame();

private:
  // UI events
  void OnCommand(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

  // manager events
  void OnDocumentAdded(const DocumentArgs& args);
  void OnDocumentRemoved(const DocumentArgs& args);

  // document events
  void OnRequestOpenFile(FileArgs& args);

private:
  // GUI managment
  wxAuiManager            m_FrameManager;
  wxAuiNotebook*          m_Notebook;

  // GUI elements
  wxMenu*                 m_FileMenu;
  wxMenu*                 m_EditMenu;
  wxToolBar*              m_ToolBar;

  // data managment
  DocumentManager         m_Manager;

  DECLARE_EVENT_TABLE();
};
