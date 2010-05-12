#pragma once

// Includes
#include "API.h"
#include "LeafItem.h"

namespace Luna
{
  // Forwards
  class LightingJob;
  class SceneNode;
  struct SceneNodeChangeArgs;

  /////////////////////////////////////////////////////////////////////////////
  // Abstract base class representing scene nodes within the lighting job 
  // tree control.  Override for specific types of nodes.
  // 
  class SceneNodeItem NOC_ABSTRACT: public LeafItem
  {
  private:
    Luna::SceneNode* m_SceneNode;

  public:
    SceneNodeItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, Luna::SceneNode* node, ItemType type );
    virtual ~SceneNodeItem();

    Luna::SceneNode* GetSceneNode() const;
    std::string GetLabel() const;

    virtual void GetSelectableItems( OS_SelectableDumbPtr& selection ) const NOC_OVERRIDE;
    virtual void SelectItem( Luna::SceneNode* node ) NOC_OVERRIDE;
    virtual ContextMenuItemSet GetContextMenuItems() NOC_OVERRIDE;

  protected:
    void NodeNameChanged( const SceneNodeChangeArgs& args );

    // Context menu callbacks
    void Remove( const ContextMenuArgsPtr& args );
  };
}