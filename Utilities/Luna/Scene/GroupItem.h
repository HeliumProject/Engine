#pragma once

// Includes
#include "API.h"
#include "LightingTreeItem.h"
#include "UniqueID/TUID.h"
#include "TUID/TUID.h"

namespace Luna
{
  // Forwards
  class SceneNode;
  class LightingJob;

  /////////////////////////////////////////////////////////////////////////////
  // Base class for item data pertaining to a tree item that groups sub items
  // together.
  // 
  class GroupItem : public LightingTreeItem
  {
  public:
    GroupItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, ItemType type );
    virtual ~GroupItem();

    void LoadZoneData( const UniqueID::S_TUID& ( LightingJob::* getter )( const tuid& ) const, tuid zoneID );
    void UnloadZoneData( const UniqueID::S_TUID& ( LightingJob::* getter )( const tuid& ) const, tuid zoneID );

  protected:
    // Derived classes must implement the following:
    virtual const std::string& GetLabel() const = 0;
    virtual void AddChildNode( Luna::SceneNode* instance ) = 0;
    virtual void RemoveChildNode( Luna::SceneNode* instance ) = 0;
    virtual void RemoveAllChildren();
    
    // Context menu callbacks
    virtual void RemoveAll( const ContextMenuArgsPtr& args );

  public:
    // Overrides
    virtual void Load() NOC_OVERRIDE;
    virtual bool CanRename() const NOC_OVERRIDE;
    virtual Undo::CommandPtr GetRemoveCommand( const OS_SelectableDumbPtr& selection ) NOC_OVERRIDE;
  };
}