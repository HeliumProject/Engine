#pragma once

#include "API.h"
#include "Editor/Editor.h"
#include "AnimationEventsPanels.h"
#include "AnimationEventsManager.h"
#include "UIToolKit/MenuMRU.h"

namespace Luna
{
  // Forwards
  class AnimationEventsMainPanel;
  class AnimationEventsTimelinePanel;
  class AnimationActiveEventsPanel;
  class AnimationEventsPropertyPanel;
  class BrowserToolBar;
  class EditorInfo;

  /////////////////////////////////////////////////////////////////////////////
  // Main UI window for editing AssetClasses and their Attributes.
  // 
  class AnimationEventsEditor : public Editor
  {
  private:
    AnimationEventsManager m_Manager;

    UIToolKit::MenuMRUPtr m_MRU;

    wxToolBar* m_StandardToolBar;
    BrowserToolBar* m_BrowserToolBar;
    wxMenu* m_MenuFile;
    wxMenu* m_MenuPanels;
    wxMenu* m_MenuMRU;
    wxMenuItem* m_MenuItemOpenRecent;

    AnimationEventsMainPanel* m_MainPanel;
    AnimationActiveEventsPanel* m_ActiveEventsPanel;
    AnimationEventsTimelinePanel* m_TimelinePanel;
    AnimationEventsPropertyPanel* m_PropertyPanel;

  public:
    AnimationEventsEditor();
    virtual ~AnimationEventsEditor();

    static void InitializeEditor();
    static void CleanupEditor();

    virtual void SaveWindowState() NOC_OVERRIDE;
    virtual DocumentManager* GetDocumentManager() NOC_OVERRIDE;

    UIToolKit::MenuMRU* GetMRU() const { return m_MRU; }

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
