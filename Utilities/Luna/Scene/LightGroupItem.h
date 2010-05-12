#pragma once

// Includes
#include "API.h"
#include "GroupItem.h"
#include "TUID/TUID.h"

namespace Luna
{
  // Forwards
  class Light;
  class LightingJob;
  class Object;
  class SceneNode;
  struct LightingJobMemberChangeArgs;

  /////////////////////////////////////////////////////////////////////////////
  // Manages the lights of a lighting job in the tree (across all zones).
  // 
  class LightGroupItem : public GroupItem
  {
  public:
    LightGroupItem( wxTreeCtrlBase* tree, Luna::LightingJob* job );
    virtual ~LightGroupItem();

    static const std::string& GetLightGroupLabel();
    virtual const std::string& GetLabel() const NOC_OVERRIDE;

    // Overrides
    virtual void Load() NOC_OVERRIDE;
    virtual bool IsSelectable( const OS_TreeItemIds& currentSelection ) const NOC_OVERRIDE;
    virtual bool CanAddChild( Object* object ) const NOC_OVERRIDE;
    virtual Undo::CommandPtr AddChild( Object* object ) NOC_OVERRIDE;

  protected:
    virtual void AddChildNode( Luna::SceneNode* instance ) NOC_OVERRIDE;
    virtual void RemoveChildNode( Luna::SceneNode* instance ) NOC_OVERRIDE;

  private:
    void AddLight( Luna::Light* light );
    void RemoveLight( Luna::Light* light );

    // Application callbacks
  private:
    void LightAdded( const LightingJobMemberChangeArgs& args );
    void LightRemoved( const LightingJobMemberChangeArgs& args );
  };
}