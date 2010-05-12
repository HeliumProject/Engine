#pragma once

// Includes
#include "API.h"
#include "GroupItem.h"
#include "TUID/TUID.h"

namespace Luna
{
  // Forwards
  class CubeMapProbe;
  struct LightingJobMemberChangeArgs;

  /////////////////////////////////////////////////////////////////////////////
  // Group that contains cube map probes for this lighting job.
  // 
  class ProbeGroupItem : public GroupItem
  {
  public:
    ProbeGroupItem( wxTreeCtrlBase* tree, Luna::LightingJob* job );
    virtual ~ProbeGroupItem();

    static const std::string& GetProbeGroupLabel();

    // Overrides
    virtual const std::string& GetLabel() const NOC_OVERRIDE;
    virtual void Load() NOC_OVERRIDE;
    virtual bool IsSelectable( const OS_TreeItemIds& currentSelection ) const NOC_OVERRIDE;
    virtual bool CanAddChild( Object* object ) const NOC_OVERRIDE;
    virtual Undo::CommandPtr AddChild( Object* object ) NOC_OVERRIDE;
  protected:
    virtual void AddChildNode( Luna::SceneNode* instance ) NOC_OVERRIDE;
    virtual void RemoveChildNode( Luna::SceneNode* instance ) NOC_OVERRIDE;

  protected:
    void AddProbe( Luna::CubeMapProbe* probe );
    void RemoveProbe( Luna::CubeMapProbe* probe );

    // Callbacks
    void ProbeAdded( const LightingJobMemberChangeArgs& args );
    void ProbeRemoved( const LightingJobMemberChangeArgs& args );
  };
}