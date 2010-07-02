#pragma once

#include "Luna/API.h"

#include "LayerGrid.h"
#include "Scene.h"
#include "SceneDocument.h"
#include "SceneEditorIDs.h"
#include "SceneManager.h"
#include "SceneOutlinerState.h"
#include "TreeMonitor.h"
#include "TreeSortTimer.h"
#include "View.h"

#include "Foundation/Container/ReversibleMap.h"

#include "Application/Inspect/Widgets/Canvas.h"
#include "Application/Inspect/DragDrop/DropTarget.h"
#include "Application/UI/MenuMRU.h"

#include "Core/PropertiesManager.h"
#include "Core/Selection.h"
#include "Editor/Editor.h"
#include "Editor/EditorInfo.h"

#include <wx/dnd.h>

namespace Luna
{
  // Forwards
  class BrowserToolBar;
  class Drawer;
  class DrawerPanel;
  class EntityAssetOutliner;
  class EntityType;
  class HierarchyNode;
  class HierarchyOutliner;
  class InstanceType;
  class TypeGrid;
  class NodeTypeOutliner;
  class View;
  class Zone;

  struct DrawerArgs;
  struct ToolChangeArgs;

  typedef std::map< Luna::Scene*, SceneDocumentPtr > M_SceneToEditorFile;

  class LUNA_SCENE_API SceneEditor : public Editor
  {
  private:
    struct OutlinerStates
    {
      SceneOutlinerState m_Hierarchy;
      SceneOutlinerState m_Types;
      SceneOutlinerState m_EntityAssetes;
    };

    typedef std::map< Luna::Scene*, OutlinerStates > M_OutlinerStates;

  private:
      static tstring s_FileFilter;

    // Stores information about the state of each outliner for each scene
    // that is open.  Restores the state when switching between scenes.
    M_OutlinerStates m_OutlinerStates;

    // the attributes notebook
    wxNotebook* m_Properties;

    // the attributes for the current selection
    EnumeratorPtr m_SelectionEnumerator;
    PropertiesManagerPtr m_SelectionPropertiesManager;
    Inspect::Canvas m_SelectionProperties;
    size_t m_SelectionPropertyPage;

    // the attributes for the current tool
    EnumeratorPtr m_ToolEnumerator;
    PropertiesManagerPtr m_ToolPropertiesManager;
    Inspect::Canvas m_ToolProperties;
    size_t m_ToolPropertyPage;

    DrawerPanel* m_DrawerPanel;

    // the directory notebook
    wxNotebook* m_Directory;

    // the outline of the current scene
    HierarchyOutliner* m_HierarchyOutline;

    // the outline of all nodes by type
    NodeTypeOutliner* m_TypeOutline;

    // the outline of all entity nodes by class
    EntityAssetOutliner* m_EntityOutline;

    // the UI for changing visibility/selectability of layers
    LayerGrid* m_LayerGrid;

    // the UI for changing visibility/selectability of specific runtime types
    TypeGrid* m_TypeGrid;

    // the loaded scenes
    M_SceneToEditorFile m_SceneFiles;

    // the scene manager
    Luna::SceneManager m_SceneManager;

    // the 3d view
    Luna::View* m_View;

    // menu items
    wxMenu* m_FileMenu;
    wxMenu* m_EditMenu;
    wxMenu* m_ViewMenu;
    wxMenu* m_ViewDefaultsMenu; 
    wxMenu* m_PanelsMenu;
    wxMenu* m_HelperMenu;
    wxMenu* m_CameraMenu;
    wxMenu* m_GeometryMenu;

    wxMenu* m_ViewColorMenu;
    typedef std::map< i32, i32 > M_IDToColorMode; // Maps wx ID for menu items to our ViewColorMode enum
    M_IDToColorMode m_ColorModeLookup;

    wxMenu* m_ShadingMenu;
    wxMenu* m_CullingMenu;
    wxMenu* m_MRUMenu;
    wxMenuItem* m_MRUMenuItem;
    Nocturnal::MenuMRUPtr m_MRU;

    // Toolbars
    wxToolBar* m_StandardToolBar;
    wxToolBar* m_ViewToolBar;
    wxToolBar* m_ToolsToolBar;
    BrowserToolBar* m_BrowserToolBar;
    wxToolBar* m_NavToolBar;

    //context items ordered by name  
    V_HierarchyNodeDumbPtr m_OrderedContextItems;

    TreeMonitor m_TreeMonitor;
    TreeSortTimer m_TreeSortTimer;

    typedef Nocturnal::ReversibleMap< CameraModes::CameraMode, SceneEditorIDs::SceneEditorID > RM_CamModeToSceneID;
    static RM_CamModeToSceneID s_CameraModeToSceneID;

  protected:
    DECLARE_EVENT_TABLE();

  public:
    static void InitializeEditor();
    static void CleanupEditor();

  public:
    SceneEditor();
    virtual ~SceneEditor();

    static SceneEditorID CameraModeToSceneEditorID( CameraMode cameraMode );
    static CameraMode SceneEditorIDToCameraMode( SceneEditorID id );

    virtual void SaveWindowState() NOC_OVERRIDE;

    virtual DocumentManager* GetDocumentManager() NOC_OVERRIDE;

    void SyncPropertyThread();

    TreeMonitor& GetTreeMonitor()
    {
      return m_TreeMonitor;
    }

    TreeSortTimer& GetTreeSortTimer()
    {
      return m_TreeSortTimer;
    }

    wxToolBar* GetNavToolBar() { return m_NavToolBar; }

    //
    // wxEventHandler Event Handlers
    //

  private:
    // frame events
    void OnEraseBackground(wxEraseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnShow(wxShowEvent& event);
    void OnMenuOpen(wxMenuEvent& event);

    // application events
    void OnNew(wxCommandEvent& event);
    bool DoOpen( const tstring& path );
    void OnOpen(wxCommandEvent& event);
    void OnFind( wxCommandEvent& event );
    void OnSaveAll(wxCommandEvent& event);
    void OnImport(wxCommandEvent& event);
    void OnExport(wxCommandEvent& event);
    void OnClose(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnExiting( wxCloseEvent& args );

    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);

    void OnHelpIndex( wxCommandEvent& event );
    void OnHelpSearch( wxCommandEvent& event );

    void OnPickWalk( wxCommandEvent& event );
    
    void OnEditPreferences( wxCommandEvent& event );

    void OnViewChange(wxCommandEvent& event);
    void OnViewCameraChange(wxCommandEvent& event);
    void OnViewVisibleChange(wxCommandEvent& event);
    void OnViewColorModeChange(wxCommandEvent& event);
    void OnViewDefaultsChange(wxCommandEvent& event); 

    void OnFrameOrigin(wxCommandEvent& event);
    void OnFrameSelected(wxCommandEvent& event);
    void OnHighlightMode(wxCommandEvent& event);

    void OnPreviousView(wxCommandEvent& event);
    void OnNextView(wxCommandEvent& event);

    void OnToolSelected(wxCommandEvent& event);

    void OnParent(wxCommandEvent& event);
    void OnUnparent(wxCommandEvent& event);
    void OnGroup(wxCommandEvent& event);
    void OnUngroup(wxCommandEvent& event);
    void OnCenter(wxCommandEvent& event);

    void OnInvertSelection(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);

    void OnDuplicate(wxCommandEvent& event);
    void OnSmartDuplicate(wxCommandEvent& event);
    void OnCopyTransform(wxCommandEvent& event);
    void OnPasteTransform(wxCommandEvent& event);
    void OnSnapToCamera(wxCommandEvent& event);
    void OnSnapCameraTo(wxCommandEvent& event);

    void OnManifestContextMenu(wxCommandEvent& event);
    void OnTypeContextMenu(wxCommandEvent& event);

    void SelectItemInScene( wxCommandEvent& event );
    void SelectSimilarItemsInScene( wxCommandEvent& event );

    void OnMRUOpen( const Nocturnal::MRUArgs& args );

    //
    // Scene Editor Implementation
    //

  public:
    Luna::View* GetView()
    {
      return m_View;
    }

    TypeGrid* GetObjectGrid()
    {
      return m_TypeGrid;
    }

    Inspect::Canvas& GetToolProperties()
    {
      return m_ToolProperties;
    }

    Inspect::Canvas& GetSelectionProperties()
    {
      return m_SelectionProperties;
    }

    Luna::SceneManager* GetSceneManager()
    {
      return &m_SceneManager;
    }

    bool Copy( Luna::Scene* scene );
    bool Paste( Luna::Scene* scene );
    void Render( RenderVisitor* render );
    void Select( const SelectArgs& args );
    void SetHighlight( const SetHighlightArgs& args );
    void ClearHighlight( const ClearHighlightArgs& args );

    bool ValidateDrag( const Inspect::DragArgs& args );
    wxDragResult DragOver( const Inspect::DragArgs& args );
    wxDragResult Drop( const Inspect::DragArgs& args );

    //
    // Our Event Handlers
    //

  private:
    void SceneAdded( const SceneChangeArgs& args );
    void SceneRemoving( const SceneChangeArgs& args );
    void SceneLoadFinished( const LoadArgs& args );
    void TitleChanged( const TitleChangeArgs& args );
    void StatusChanged( const StatusChangeArgs& args );
    void CursorChanged( const CursorChangeArgs& args );
    void BusyCursorChanged( const CursorChangeArgs& args );
    void Executed( const ExecuteArgs& args );

    void SelectionChanged( const OS_SelectableDumbPtr& selection );

    void CurrentSceneChanging( const SceneChangeArgs& args );
    void CurrentSceneChanged( const SceneChangeArgs& args );

    void DocumentModified( const DocumentChangedArgs& args );
    void DocumentClosed( const DocumentChangedArgs& args );

    void ViewToolChanged( const ToolChangeArgs& args );

    void OpenTypeContextMenu( const SelectArgs& args );
    void OpenManifestContextMenu( const SelectArgs& args );

    void OnDrawerPaneModified( const DrawerArgs& args );

  private:
    void AddDrawer( Drawer* drawer );
    void SetupTypeContextMenu( const HM_StrToSceneNodeTypeSmartPtr& sceneNodeTypes,wxMenu& contextMenu,  u32& numMenuItems );
    bool SetupEntityTypeMenus( const Luna::EntityType* entity, wxMenu* entityMenu, u32& numMenuItems );

    static bool SortContextItemsByName( Luna::SceneNode* lhs, Luna::SceneNode* rhs ); 
    static bool SortTypeItemsByName( Luna::SceneNodeType* lhs, Luna::SceneNodeType* rhs );
  };
}
