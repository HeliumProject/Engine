#pragma once

// Includes
#include "Luna/API.h"
#include "SceneOutliner.h"

namespace Luna
{
  // Forwards
  class HierarchyNode;
  class HierarchyOutlinerItemData;
  struct ParentChangedArgs;
  struct NodeChangeArgs;

  /////////////////////////////////////////////////////////////////////////////
  // Class to coordinate GUI events on a tree control, and the underlying 
  // hierarchy data.  Items are not loaded in the tree until they are needed
  // when the user expands parent items.  Call Load() to get at least the first
  // item displayed in the tree.  Call Unload() before calling Load() on any
  // other HierarchyOutliner's that are associated with the same tree control.  To 
  // completely free all data associated with this HierarchyOutliner, call Clear().
  // 
  class HierarchyOutliner : public SceneOutliner
  {
  public:
    HierarchyOutliner( Luna::SceneManager* sceneManager );
    virtual ~HierarchyOutliner();

  protected:
    HierarchyOutlinerItemData* GetTreeItemData( const wxTreeItemId& item );
    void AddHierarchyNodes();
    void RecurseAddHierarchyNode( Luna::HierarchyNode* node );
    void AddHierarchyNode( Luna::HierarchyNode* node );

  protected:
    // Overrides from SceneOutliner
    virtual SortTreeCtrl* CreateTreeCtrl( wxWindow* parent, wxWindowID id ) NOC_OVERRIDE;
    virtual void CurrentSceneChanged( Luna::Scene* oldScene ) NOC_OVERRIDE;
    virtual void ConnectSceneListeners() NOC_OVERRIDE;
    virtual void DisconnectSceneListeners() NOC_OVERRIDE;

  private:
    // Tree event callbacks
    void OnBeginDrag( wxTreeEvent& args );
    void OnEndDrag( wxTreeEvent& args );

  private:
    // Event callbacks for other systems in Luna - do not call directly
    void ParentChanged( const ParentChangedArgs& args );
    void NodeAdded( const NodeChangeArgs& args );
    void NodeRemoved( const NodeChangeArgs& args );
  };
}
