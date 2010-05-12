#include "Precompile.h"
#include "SceneNodeItem.h"

#include "Scene.h"
#include "SceneNode.h"

#include "Editor/ContextMenu.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
SceneNodeItem::SceneNodeItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, Luna::SceneNode* node, ItemType type )
: LeafItem( tree, job, type )
, m_SceneNode( node )
{
  NOC_ASSERT( m_SceneNode );
  m_SceneNode->AddNameChangedListener( SceneNodeChangeSignature::Delegate ( this, &SceneNodeItem::NodeNameChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
SceneNodeItem::~SceneNodeItem()
{
  m_SceneNode->RemoveNameChangedListener( SceneNodeChangeSignature::Delegate ( this, &SceneNodeItem::NodeNameChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Return the scene node that this tree item wraps.
// 
Luna::SceneNode* SceneNodeItem::GetSceneNode() const
{
  return m_SceneNode;
}

///////////////////////////////////////////////////////////////////////////////
// Gets the label to display on this tree item (combination of the zone name
// and node name).
// 
std::string SceneNodeItem::GetLabel() const
{
  // Build the name of the item to include the zone that it's in
  std::string zone = "Unknown Zone";
  if ( m_SceneNode->GetScene() )
  {
    const std::string& fileName = m_SceneNode->GetScene()->GetFileName();
    if ( !fileName.empty() )
    {
      zone = fileName;
    }
  }
  
  const std::string label = zone + ":" + m_SceneNode->GetName();
  return label;
}

///////////////////////////////////////////////////////////////////////////////
// Adds the scene node that this item wraps to the selection.
// 
void SceneNodeItem::GetSelectableItems( OS_SelectableDumbPtr& selection ) const
{
  selection.Append( m_SceneNode );
  __super::GetSelectableItems( selection );
}

void SceneNodeItem::SelectItem( Luna::SceneNode* node )
{
  if( m_SceneNode == node )
    GetTree()->SelectItem( GetId() );
}
///////////////////////////////////////////////////////////////////////////////
// Returns the context menu set to use for this tree item.
// 
ContextMenuItemSet SceneNodeItem::GetContextMenuItems()
{
  ContextMenuItemSet menuItems;

  ContextMenuItemPtr remove = new ContextMenuItem( "Remove", "Remove highlighed objects from the lighting job" );
  remove->AddCallback( ContextMenuSignature::Delegate( this, &SceneNodeItem::Remove ) );
  menuItems.AppendItem( remove );

  return menuItems;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the name of the scene node changes.  Updates the tree
// control with the new label.
// 
void SceneNodeItem::NodeNameChanged( const SceneNodeChangeArgs& args )
{
  NOC_ASSERT( args.m_Node == m_SceneNode );
  if ( args.m_Node == m_SceneNode )
  {
    GetTree()->SetItemText( GetId(), GetLabel().c_str() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user chooses the menu item to remove this item.  Removes
// this item from the lighting job.
// 
void SceneNodeItem::Remove( const ContextMenuArgsPtr& args )
{
  OS_SelectableDumbPtr itemsToRemove;
  itemsToRemove.Append( m_SceneNode );

  args->GetBatch()->Push( GetRemoveCommand( itemsToRemove ) );
}
