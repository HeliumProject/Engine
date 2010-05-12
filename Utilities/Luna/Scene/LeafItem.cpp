#include "Precompile.h"
#include "LeafItem.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LeafItem::LeafItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, ItemType type )
: LightingTreeItem( tree, job, type )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LeafItem::~LeafItem()
{
}

///////////////////////////////////////////////////////////////////////////////
// Required by the base class.
// 
void LeafItem::Load()
{
  // Do nothing... for now.
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this item can be selected.
// 
bool LeafItem::IsSelectable( const OS_TreeItemIds& currentSelection ) const
{
  bool isSelectable = false;

  if ( currentSelection.Size() == 0 )
  {
    isSelectable = true;
  }
  else
  {
    // Special-cased for lights and instances so they can both be selected together (for Remove functionality)
    LightingTreeItem* firstSelected = reinterpret_cast< LightingTreeItem* >( GetTree()->GetItemData( *currentSelection.Begin() ) );
    ItemType type = firstSelected->GetType();
    isSelectable = ( type == TypeLight || type == TypeInstance || type == TypeVolume || type == TypeZone );
  }

  return isSelectable;
}

///////////////////////////////////////////////////////////////////////////////
// Returns false.  You cannot rename leaf items since their name is generated
// by combining the item name with the zone name.
// 
bool LeafItem::CanRename() const
{
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Returns false.  Leaf items cannot have children.
// 
bool LeafItem::CanAddChild( Object* object ) const 
{
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// No one should be calling this function.  Leaf items cannot have children.
// 
Undo::CommandPtr LeafItem::AddChild( Object* object ) 
{
  NOC_BREAK();
  return NULL;
}
