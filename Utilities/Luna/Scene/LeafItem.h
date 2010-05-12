#pragma once

// Includes
#include "API.h"
#include "LightingTreeItem.h"

namespace Luna
{
  // Forwards
  class LightingJob;
  class Object;

  /////////////////////////////////////////////////////////////////////////////
  // Leaf items in the tree cannot have any children.
  // 
  class LeafItem NOC_ABSTRACT : public LightingTreeItem
  {
  public:
    LeafItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, ItemType type );
    virtual ~LeafItem();

    virtual void Load() NOC_OVERRIDE;
    virtual bool IsSelectable( const OS_TreeItemIds& currentSelection ) const NOC_OVERRIDE;
    virtual bool CanRename() const NOC_OVERRIDE;
    virtual bool CanAddChild( Object* object ) const NOC_OVERRIDE;
    virtual Undo::CommandPtr AddChild( Object* object ) NOC_OVERRIDE;
  };
}