#pragma once

#include "Common/Types.h"
#include "Core/Selection.h"

namespace Luna
{
  // Forwards
  class Entity;
  class Scene;
  class SceneEditor;
  class InnerLiveLinkPanel;
  class RemoteScene;
  struct SceneChangeArgs;
  struct NodeChangeArgs;
  struct ViewerControlChangeArgs;
  struct RuntimeConnectionStatusArgs;

  typedef Nocturnal::SmartPtr< Luna::Entity > EntityPtr;
  typedef std::vector< Luna::Entity* > V_EntityDumbPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Main panel for the lighting UI.  Wraps the inner lighting panel which is
  // generated from wxFormBuilder.  Hooks up all the various application and
  // UI event listeners to make the UI work.
  // 
  class LiveLinkPanel : public wxPanel
  {
  private:
    SceneEditor* m_SceneEditor;
    RemoteScene* m_RemoteScene;
    InnerLiveLinkPanel* m_Panel;

  public:
    LiveLinkPanel( SceneEditor* sceneEditor );
    virtual ~LiveLinkPanel();

    void EnableSim( bool enable );

  private:
    void GetSelectedMobys( V_EntityDumbPtr& mobys );
    void GetHighlightedMobys( V_EntityDumbPtr& mobys );
    void GetAllMobys( V_EntityDumbPtr& mobys );
    std::string GetListNameForEntity( const Luna::Entity* entity );

    void SceneChanging( const SceneChangeArgs& args );
    void SceneChanged( const SceneChangeArgs& args );
    void SceneNodeDeleting( const NodeChangeArgs& args );
    void SelectionChanged( const OS_SelectableDumbPtr& selection );
    void ViewerControlChanged( const ViewerControlChangeArgs& args );
    void ConnectionStatusChanged( const RuntimeConnectionStatusArgs& args );

    void OnButtonAdd( wxCommandEvent& args );
    void OnButtonRemove( wxCommandEvent& args );
    void OnButtonSync( wxCommandEvent& args );
    void OnButtonReset( wxCommandEvent& args );
    void OnButtonSimulate( wxCommandEvent& args );
    void OnMobySelected( wxCommandEvent& args );

    void OnLiveSyncToggled( wxCommandEvent& args );
    void OnLiveGameSyncToggled( wxCommandEvent& args );
    void OnCameraSyncToggled( wxCommandEvent& args );
    void OnLightingSyncToggled( wxCommandEvent& args );
    void OnButtonBuildAll( wxCommandEvent& args );
    void OnButtonBuildCollisionDb( wxCommandEvent& args );
    void OnButtonBuildStaticDb( wxCommandEvent& args );
    void OnButtonBuildLightingData( wxCommandEvent& args );
    void OnButtonTakeScreenshot( wxCommandEvent& args );
  };
}
