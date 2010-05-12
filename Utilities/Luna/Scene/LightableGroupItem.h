#pragma once

// Includes
#include "API.h"
#include "GroupItem.h"
#include "TUID/TUID.h"

namespace Luna
{
  // Forwards
  class Entity;
  class LightingJob;
  class SceneNode;

  /////////////////////////////////////////////////////////////////////////////
  // Base class for shadow caster and render target groups in the tree.
  // 
  class LightableGroupItem NOC_ABSTRACT : public GroupItem
  {
  public:
    LightableGroupItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, ItemType type );
    virtual ~LightableGroupItem();

  protected:
    void AddEntity( Luna::Entity* entity );
    void RemoveEntity( Luna::Entity* entity );

    // Overrides
    virtual bool IsSelectable( const OS_TreeItemIds& currentSelection ) const NOC_OVERRIDE;
    virtual bool CanAddChild( Object* object ) const NOC_OVERRIDE;
    virtual Undo::CommandPtr AddChild( Object* object ) NOC_OVERRIDE;

  protected:
    virtual void AddChildNode( Luna::SceneNode* instance ) NOC_OVERRIDE;
    virtual void RemoveChildNode( Luna::SceneNode* instance ) NOC_OVERRIDE;
  };
}