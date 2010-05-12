#include "Precompile.h"
#include "LightingTreeItem.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// You have to specify the tree control that this item is part of, the lighting
// job that is the root of this item's tree, and the type of the item.
// 
LightingTreeItem::LightingTreeItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, LightingTreeItem::ItemType type )
: m_Tree( tree )
, m_LightingJob( job )
, m_Type( type )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LightingTreeItem::~LightingTreeItem()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the tree control that this item is part of.
// 
wxTreeCtrlBase* LightingTreeItem::GetTree() const
{
  return m_Tree;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the Luna::LightingJob associated with this class.
// 
Luna::LightingJob* LightingTreeItem::GetLightingJob() const
{
  return m_LightingJob;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the type of this tree item.
// 
LightingTreeItem::ItemType LightingTreeItem::GetType() const
{
  return m_Type;
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over all the children of this item and calls their GetSelectableItems
// function.  Any tree item that actually wraps a selectable scene node should
// add that node to the selection.
// 
void LightingTreeItem::GetSelectableItems( OS_SelectableDumbPtr& selection ) const
{
  wxTreeCtrlBase* tree = GetTree();
  const wxTreeItemId id = GetId();
  wxTreeItemIdValue cookie;
  wxTreeItemId child = tree->GetFirstChild( id, cookie );
  while ( child.IsOk() )
  {
    LightingTreeItem* childData = static_cast< LightingTreeItem* >( tree->GetItemData( child ) );
    if ( childData )
    {
      childData->GetSelectableItems( selection );
    }

    child = tree->GetNextChild( id, cookie );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over all the children of this item and calls their GetSelectableItems
// function.  Any tree item that actually wraps a selectable scene node should
// add that node to the selection.
// 
void LightingTreeItem::SelectItem( Luna::SceneNode* node )
{
  wxTreeCtrlBase* tree = GetTree();
  const wxTreeItemId id = GetId();
  wxTreeItemIdValue cookie;
  wxTreeItemId child = tree->GetFirstChild( id, cookie );
  while ( child.IsOk() )
  {
    LightingTreeItem* childData = static_cast< LightingTreeItem* >( tree->GetItemData( child ) );
    if ( childData )
    {
      childData->SelectItem( node );
    }

    child = tree->GetNextChild( id, cookie );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns an empty list.  Derived classes should NOC_OVERRIDE this function and
// provide their own context menu items.
// 
ContextMenuItemSet LightingTreeItem::GetContextMenuItems()
{ 
  return ContextMenuItemSet(); 
}
