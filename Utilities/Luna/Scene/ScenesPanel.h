#pragma once

#include <map>

#include "Common/File/Path.h"

namespace Luna
{
  // Forwards
  class Scene;
  class SceneEditor;
  class SceneManager;
  class SceneRowPanel;
  class Zone;
  struct NodeTypeExistenceArgs;
  struct SceneChangeArgs;
  struct LoadArgs;
  struct NodeChangeArgs;

  /////////////////////////////////////////////////////////////////////////////
  // Class ScenesPanel
  // 
  class ScenesPanel : public wxPanel 
  {
  private:
    typedef std::map< Zone*, SceneRowPanel* > M_ZoneRows;

  private:
    SceneEditor* m_Editor;
    Luna::SceneManager* m_SceneManager;
    M_ZoneRows m_ZoneRows;
	  wxBitmapButton* m_ButtonNewZone;
    wxBitmapButton* m_ButtonAddZone;
	  wxBitmapButton* m_ButtonMoveSelToZone;
	  wxScrolledWindow* m_ScrollWindow;
    wxBoxSizer* m_ScrollSizer;
    bool m_IsSortingEnabled;
    bool m_IsSceneSwitchEnabled;
	
  public:
	  ScenesPanel( SceneEditor* editor, Luna::SceneManager* sceneManager, wxWindow* parent, int id = -1 );
    virtual ~ScenesPanel();

    wxScrolledWindow* GetScrollWindow();

    void Sort();

    void EnableSceneSwitch( bool enable = true );
    bool IsSceneSwitchEnabled() const;

  private:
    void AddRow( Luna::Scene* scene, Zone* zone );
    void DeleteRow( Zone* zone );
    void DeleteAllRows();
    std::string PromptNewZone( const std::string& defaultPath );
    std::string PromptAddZone( const std::string& defaultPath );
    Zone* AddZone( std::string ( ScenesPanel::*PromptFunction )( const std::string& ) );
    void AddNewZone();
    bool ContainsZone( const Nocturnal::Path& zonePath ) const;
    void PromptIfNoZones();

  private:
    void SceneAdded( const SceneChangeArgs& args );
    void SceneRemoved( const SceneChangeArgs& args );
    void CurrentSceneChanged( const SceneChangeArgs& args );
    void NodeAdded( const NodeChangeArgs& args );
    void NodeRemoved( const NodeChangeArgs& args );
    void SceneLoadStarted( const LoadArgs& args );
    void SceneLoadFinished( const LoadArgs& args );

  private:
    void OnNewZone( wxCommandEvent& args );
    void OnAddZone( wxCommandEvent& args );
    void OnMoveSelToZone( wxCommandEvent& args );
  public:
    // This function has to be public because the SceneRowPanel rows hookup the callback.
    void OnDeleteZoneButton( wxCommandEvent& args );
  };
}
