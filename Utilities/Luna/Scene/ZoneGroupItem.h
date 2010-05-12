#pragma once

// Includes
#include "API.h"
#include "GroupItem.h"
#include "TUID/TUID.h"

namespace Luna
{
  // Forwards
  class Zone;
  class LightingJob;
  class Object;
  class SceneNode;
  struct LightingJobMemberChangeArgs;

  /////////////////////////////////////////////////////////////////////////////
  // Manages the lights of a lighting job in the tree (across all zones).
  // 
  class ZoneGroupItem : public GroupItem
  {
  public:
    ZoneGroupItem( wxTreeCtrlBase* tree, Luna::LightingJob* job );
    virtual ~ZoneGroupItem();

    static const std::string& GetZoneGroupLabel();
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
    void AddZone( Zone* zone );
    void RemoveZone( Zone* zone );

    // Application callbacks
  private:
    void ZoneAdded( const LightingJobMemberChangeArgs& args );
    void ZoneRemoved( const LightingJobMemberChangeArgs& args );
  };
}