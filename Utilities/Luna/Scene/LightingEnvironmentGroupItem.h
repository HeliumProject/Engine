#pragma once

// Includes
#include "API.h"
#include "GroupItem.h"
#include "TUID/TUID.h"

namespace Luna
{
  // Forwards
  class LightingEnvironment;
  class LightingJob;
  class Object;
  class SceneNode;
  struct LightingJobMemberChangeArgs;

  /////////////////////////////////////////////////////////////////////////////
  // Manages the lights of a lighting job in the tree (across all zones).
  // 
  class LightingEnvironmentGroupItem : public GroupItem
  {
  public:
    LightingEnvironmentGroupItem( wxTreeCtrlBase* tree, Luna::LightingJob* job );
    virtual ~LightingEnvironmentGroupItem();

    static const std::string& GetLightingEnvironmentGroupLabel();
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
    void AddLightingEnvironment( Luna::LightingEnvironment* env );
    void RemoveLightingEnvironment( Luna::LightingEnvironment* env );

    // Application callbacks
  private:
    void LightingEnvironmentAdded( const LightingJobMemberChangeArgs& args );
    void LightingEnvironmentRemoved( const LightingJobMemberChangeArgs& args );
  };
}