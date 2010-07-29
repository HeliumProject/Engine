#pragma once

// Includes
#include "Editor/API.h"
#include "SceneOutlinerState.h"
#include "Selection.h"
#include "Platform/Types.h"
#include "UI/Controls/Tree/SortTreeCtrl.h"

namespace Editor
{
  // Forwards
  class Object;
  class Scene;
  class SceneManager;
  class SceneOutlinerItemData;
  struct SceneNodeChangeArgs;
  struct SceneChangeArgs;

  /////////////////////////////////////////////////////////////////////////////
  // Abstract base class for GUIs that display trees of scene nodes.
  // 
  class SceneOutliner HELIUM_ABSTRACT : public wxEvtHandler
  {
  protected:
    // Typedefs
    typedef std::map< Object*, wxTreeItemId > M_TreeItems;

  protected:
    // Member variables
    Editor::SceneManager* m_SceneManager;
    Editor::Scene* m_CurrentScene;
    SortTreeCtrl* m_TreeCtrl;
    M_TreeItems m_Items;
    SceneOutlinerState m_StateInfo;
    bool m_IgnoreSelectionChange;
    bool m_DisplayCounts; 

  public:
    // Functions
    SceneOutliner( Editor::SceneManager* sceneManager );
    virtual ~SceneOutliner();
    SortTreeCtrl* InitTreeCtrl( wxWindow* parent, wxWindowID id );
    void SaveState( SceneOutlinerState& state );
    void RestoreState( const SceneOutlinerState& state );
    void DisableSorting();
    void EnableSorting();
    virtual void Sort( const wxTreeItemId& root = SortTreeCtrl::InvalidItemId );

  protected:
    SceneOutlinerItemData* GetTreeItemData( const wxTreeItemId& item );
    void UpdateCurrentScene( Editor::Scene* scene );
    void DoRestoreState();

  protected:
    // Derived classes can optionally HELIUM_OVERRIDE these functions
    virtual void Clear();
    virtual wxTreeItemId AddItem( const wxTreeItemId& parent, const tstring& name, i32 image, SceneOutlinerItemData* data, bool isSelected, bool countable = true); 
    virtual void DeleteItem( Object* object );
    void UpdateItemCounts( const wxTreeItemId& node, int delta );
    void UpdateItemVisibility( const wxTreeItemId& item, bool visible );

    virtual void ConnectSceneListeners();
    virtual void DisconnectSceneListeners();
    virtual void CurrentSceneChanging( Editor::Scene* newScene );
    virtual void CurrentSceneChanged( Editor::Scene* oldScene );

  protected:
    // Application event callbacks
    virtual void CurrentSceneChanged( const SceneChangeArgs& args );
    virtual void SelectionChanged( const OS_SelectableDumbPtr& selection );
    virtual void SceneNodeNameChanged( const SceneNodeChangeArgs& args );
    void SceneNodeVisibilityChanged( const SceneNodeChangeArgs& args );

  protected:
    // Derived classes must HELIUM_OVERRIDE these functions
    virtual SortTreeCtrl* CreateTreeCtrl( wxWindow* parent, wxWindowID id ) = 0;

  private:
    // Tree event callbacks
    virtual void OnEndLabelEdit( wxTreeEvent& args );
    virtual void OnSelectionChanging( wxTreeEvent& args );
    virtual void OnSelectionChanged( wxTreeEvent& args );
    virtual void OnExpanded( wxTreeEvent& args );
    virtual void OnCollapsed( wxTreeEvent& args );
    virtual void OnDeleted( wxTreeEvent& args );
    virtual void OnChar( wxKeyEvent& args );

  private:
    // Connect/disconnect dynamic event table for GUI callbacks
    void ConnectDynamicEventTable();
    void DisconnectDynamicEventTable();
  };
}
