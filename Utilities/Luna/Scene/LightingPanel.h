#pragma once

#include "Common/Types.h"
#include "Core/Selection.h"

// Forwards
namespace UIToolKit { struct ListSelectionChangeArgs; }
namespace UIToolKit { struct ListChangeArgs; }

namespace Luna
{
  // Forwards
  class InnerLightingPanel;
  class Entity;
  class Light;
  class LightingVolume;
  class LightingJob;
  class Scene;
  class SceneEditor;
  struct SceneChangeArgs;
  struct LoadArgs;
  struct NodeChangeArgs;
  struct SceneNodeChangeArgs;
  class LightingPanel;

  typedef void (LightingPanel::*SelectionFunctionPointer)(void);

  /////////////////////////////////////////////////////////////////////////////
  // Main panel for the lighting UI.  Wraps the inner lighting panel which is
  // generated from wxFormBuilder.  Hooks up all the various application and
  // UI event listeners to make the UI work.
  // 
  class LightingPanel : public wxPanel
  {
  private:
    SceneEditor* m_SceneEditor;
    InnerLightingPanel* m_Panel;

    static const u32 NumSelectionFuncs = 4;
    SelectionFunctionPointer m_SelectionFuncs[NumSelectionFuncs];    

  public:
    LightingPanel( SceneEditor* sceneEditor );
    virtual ~LightingPanel();

  private:
    void Init();
    void UpdateButtons( wxWindow* selectedTab = NULL );

    void SelectInGui();
    void SelectInScene();
    void SelectNonRenderTargets();
    void SelectAllRenderTargets();

    void UncondenseAllLightingJobs();

    // Application callbacks
  private:
    void SceneAdded( const SceneChangeArgs& args );
    void SceneRemoved( const SceneChangeArgs& args );
    void SelectionChanged( const OS_SelectableDumbPtr& selection );

    // UI callbacks
  private:
    void OnButtonNewJob( wxCommandEvent& args );
    void OnButtonDeleteJob( wxCommandEvent& args );
    void OnButtonCopyJob( wxCommandEvent& args );
    void OnButtonAddJobMembers( wxCommandEvent& args );
    void OnButtonRemoveJobMembers( wxCommandEvent& args );
    void OnButtonRender( wxCommandEvent& args );
    
    void OnButtonView( wxCommandEvent& args );
    void OnButtonSort( wxCommandEvent& args );
    void OnTabChanged( wxNotebookEvent& args );
    void OnJobTreeSelectionChanged( wxTreeEvent& args );

    void OnButtonSelect( wxCommandEvent& args );
    
    void OnButtonCheckSetup( wxCommandEvent& args );
    void OnButtonUpdateShadowCasters( wxCommandEvent& args );
    void OnChoiceSelectRenderType( wxCommandEvent& args );

    void OnButtonUpdateLitColumn( wxCommandEvent& args );
    void OnButtonViewUVs( wxCommandEvent& args );
    void OnButtonSetTPM( wxCommandEvent& args );

    void OnButtonSyncLighting( wxCommandEvent& args );
    void OnButtonQuickSetup( wxCommandEvent& args );
    void OnButtonQuickRender( wxCommandEvent& args );
  };



  


  
}
