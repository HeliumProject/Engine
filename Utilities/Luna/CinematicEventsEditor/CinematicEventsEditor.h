#pragma once

#include "API.h"
#include "CinematicEventsDocument.h"
#include "Editor/Editor.h"
#include "CinematicEventsPanels.h"
#include "CinematicEventsManager.h"
#include "UIToolKit/MenuMRU.h"

namespace Luna
{
  // Forwards
  class BrowserToolBar;
  class CinematicMainPanel;
  class CinematicEventsTimelinePanel;
  class CinematicActiveEventsPanel;
  class CinematicPropertyPanel;
  class EditorInfo;

  /////////////////////////////////////////////////////////////////////////////
  // Main UI window for editing AssetClasses and their Attributes.
  // 
  class CinematicEventsEditor : public Editor
  {
  private:
    CinematicEventsManager m_Manager;

    UIToolKit::MenuMRUPtr m_MRU;

    wxToolBar* m_StandardToolBar;
    BrowserToolBar* m_BrowserToolBar;
    wxMenu* m_MenuFile;
    wxMenu* m_MenuPanels;
    wxMenu* m_MenuMRU;
    wxMenuItem* m_MenuItemOpenRecent;

    CinematicMainPanel* m_MainPanel;
    CinematicActiveEventsPanel* m_ActiveEventsPanel;
    CinematicEventsTimelinePanel* m_TimelinePanel;
    CinematicPropertyPanel* m_PropertyPanel;

  public:
    CinematicEventsEditor();
    virtual ~CinematicEventsEditor();

    static void InitializeEditor();
    static void CleanupEditor();

    virtual void SaveWindowState() NOC_OVERRIDE;
    virtual DocumentManager* GetDocumentManager() NOC_OVERRIDE;

    UIToolKit::MenuMRU* GetMRU() const { return m_MRU; }

  private:
    void PerformOpen( const std::string& path );

    // 
    // UI callbacks
    // 
  private:
    void OnMenuOpen( wxMenuEvent& args );
    void OnOpen( wxCommandEvent& args );
    void OnFind( wxCommandEvent& args );
    void OnClose( wxCommandEvent& args );
    void OnExit( wxCommandEvent& args );
    void OnSave( wxCommandEvent& args );
    void OnUndo( wxCommandEvent& args );
    void OnRedo( wxCommandEvent& args );
    void OnCut( wxCommandEvent& args );
    void OnCopy( wxCommandEvent& args );
    void OnPaste( wxCommandEvent& args );
    void OnChar( wxKeyEvent& event );
    void OnMRUMenuItem( const UIToolKit::MRUArgs& args );
    void OnExiting( wxCloseEvent& args );
    void OnSelectAll( wxCommandEvent& args );
    
    void OnDocumentModified( const DocumentChangedArgs& args );
    void OnDocumentClosed( const DocumentChangedArgs& args );

  private:
    DECLARE_EVENT_TABLE();
  };
}
