#pragma once

// Includes
#include "API.h"
#include "SceneNodeItem.h"

namespace Luna
{
  // Forwards
  class Entity;
  class LightingJob;
  class Object;

  /////////////////////////////////////////////////////////////////////////////
  // Wraps a lightable entity in the tree control.
  // 
  class LightableItem : public SceneNodeItem
  {
  private:
    Luna::Entity* m_Entity;

  public:
    LightableItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, Luna::Entity* entity );
    virtual ~LightableItem();

    Luna::Entity* GetEntity() const;

    virtual bool IsSelectable( const OS_TreeItemIds& currentSelection ) const NOC_OVERRIDE;
    virtual Undo::CommandPtr GetRemoveCommand( const OS_SelectableDumbPtr& selection ) NOC_OVERRIDE;
  };
}