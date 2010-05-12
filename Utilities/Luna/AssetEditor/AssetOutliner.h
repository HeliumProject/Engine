#pragma once

#include <map>
#include "Inspect/DropTarget.h"
#include "Core/Selectable.h"
#include "UIToolKit/SortTreeCtrl.h"

#include <wx/dnd.h>

namespace Luna
{
  // Forwards
  struct ChildChangeArgs;
  struct NodeIconChangeArgs;
  struct NodeNameChangeArgs;
  struct NodeStyleChangeArgs;
  struct HierarchyChangeArgs;
  class AssetEditor;
  class AssetManager;
  class AssetNode;
  typedef Nocturnal::SmartPtr< Luna::AssetNode > AssetNodePtr;
  typedef std::set< Luna::AssetNode* > S_AssetNodeDumbPtr;
  class AssetOutlineItemData;

  /////////////////////////////////////////////////////////////////////////////
  // Manages the tree UI that is displayed for all asset nodes.
  // 
  class AssetOutliner : public wxEvtHandler
  {
  private:
    typedef std::map< Luna::AssetNode*, wxTreeItemId > M_TreeItems;

  private:
    UIToolKit::SortTreeCtrl* m_TreeControl;
    AssetEditor* m_Editor;
    Luna::AssetManager* m_AssetManager;
    M_TreeItems m_Items;
    bool m_FreezeSelection;
    wxTreeItemId m_DragOverItem;
    bool m_DragOriginatedHere;
    S_AssetNodeDumbPtr m_ParentsOfDraggedNodes;

  public:
    AssetOutliner( AssetEditor* editor, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize );
    virtual ~AssetOutliner();
    wxWindow* const GetWindow() const;
    void Sort( const wxTreeItemId& itemId = UIToolKit::SortTreeCtrl::InvalidItemId, bool recursive = true );

    bool AddItem( Luna::AssetNode* node );
    void DeleteItem( Luna::AssetNode* node );

    bool ExpandAll( bool expand );
    bool ExpandNodes( const S_AssetNodeDumbPtr& nodes, bool expand );
    bool ExpandNode( Luna::AssetNode* node, bool expand );

  private:
    void ExpandTreeItem( wxTreeItemId item, bool expand );

    wxTreeItemId DragHitTest( wxPoint point );
    void SetLabelStyle( const wxTreeItemId& treeItem, u32 style );
    AssetOutlineItemData* GetItemData( const wxTreeItemId& id );
    void DebugDumpTree( const wxTreeItemId& treeItem, std::string prefix );

    // callback for when an item has been deleted
    void ItemDeleted( AssetOutlineItemData* data );
    friend class AssetOutlineItemData;

    // 
    // Data change callbacks
    // 
  private:
    void NodeChildAdded( const ChildChangeArgs& args );
    void NodeChildRemoved( const ChildChangeArgs& args );
    void NodeChildRearranged( const ChildChangeArgs& args );
    void NodeRenamed( const NodeNameChangeArgs& args );
    void NodeIconChanged( const NodeIconChangeArgs& args );
    void NodeStyleChanged( const NodeStyleChangeArgs& args );
    void HierarchyChangeStarting( const HierarchyChangeArgs& args );
    void HierarchyChangeFinished( const HierarchyChangeArgs& args );
    void SelectionChanged( const OS_SelectableDumbPtr& selection );
    wxDragResult DragOver( const Inspect::DragArgs& args );
    wxDragResult Drop( const Inspect::DragArgs& args );
    void DragLeave( Nocturnal::Void );

    // 
    // UI callbacks
    // 
  private:
    void OnSelectionChanged( wxTreeEvent& args );
    void OnTreeItemRightClick( wxTreeEvent& args );
    void OnTreeItemActivated( wxTreeEvent& args );
    void OnBeginDrag( wxTreeEvent& args );
    void OnEndDrag( wxTreeEvent& args );

  private:
    DECLARE_EVENT_TABLE();
  };
}
