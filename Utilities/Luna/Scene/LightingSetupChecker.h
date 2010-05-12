#pragma once

#include "tuid/TUID.h"
#include "LightmapUVViewer.h"
#include "Common/Container/BitArray.h"
#include "SceneEditor.h"

namespace UIToolKit
{
  class SortableListView;
};
namespace Luna
{
  namespace SetupCheckerStates
  {
    enum SetupCheckerState
    {
      UnclassifiedMeshes = 0,
      LightmapUVsMissing,
      LightmapUVsOutOfRange,
      LightmapUVsOverlap,
      LowUVSurfaceArea,
      NumSetupCheckerStates
    };

    static const char* SetupCheckerStateStrings[] =
    {
      "Unclassified Meshes",
      "No Lightmap UVs",
      "Lightmap UVs Out of Range",
      "Lightmap UVs Overlap",
      "LowUVSurfaceArea"
    };
    typedef SetupCheckerStates::SetupCheckerState SetupCheckerStates;
  }

  

  struct SetupCheckerItem : public Nocturnal::RefCountBase< SetupCheckerItem >
  {
    SetupCheckerItem() 
      : m_ErrorFlags( SetupCheckerStates::NumSetupCheckerStates )
    {}
    
    tuid m_AssetID;
    std::string m_AssetName;
    BitArray m_ErrorFlags;
  };

  typedef Nocturnal::SmartPtr< SetupCheckerItem > SetupCheckerItemPtr;
  typedef std::vector< SetupCheckerItemPtr > V_SetupCheckerItem;

  
  
class LightingSetupChecker : public wxFrame
{

protected:
  wxTextCtrl* m_TextControl;
  wxListCtrl* m_ListControl;
  wxButton* m_CloseButton;
  wxButton* m_SetUpErrorButton;
  wxButton* m_SetUpSelectedButton;
  wxButton* m_ViewUVsButton;
  wxButton* m_EditInMayaButton;
  wxButton* m_SelectInSceneButton;

  LightmapUVViewer* m_UVViewer;

  SceneEditor* m_SceneEditor;


public:

  S_tuid m_ErrorEntityClasses;
  S_tuid m_SelectedClasses;
  V_SetupCheckerItem m_SetupCheckerItems;



public:
  LightingSetupChecker( wxWindow* parent, SceneEditor* editor, const std::string& title, int xpos, int ypos, int width, int height );
  ~LightingSetupChecker();

  wxTextCtrl* GetTextCtrl()
  {
    return m_TextControl;
  }

  wxListCtrl* GetListCtrl();


private:

  //nocturnal event handlers
  void OnViewerClosed( ViewerClosedArgs& args );

  // wx event handlers
  void OnClose( wxCommandEvent& event );
  void OnSetupErrors( wxCommandEvent& event );
  void OnSetupSelected( wxCommandEvent& event );
  void OnItemSelected( wxListEvent& event );
  void OnItemDeSelected( wxListEvent& event );
  void OnViewSelectedUVs( wxCommandEvent& event );
  void OnEditInMaya( wxCommandEvent& event );
  void OnSortEntityClasses( wxListEvent& event );
  void OnSelectInScene( wxCommandEvent& event );



public:


  DECLARE_EVENT_TABLE();

};
}
