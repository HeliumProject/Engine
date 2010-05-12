#pragma once

// Includes
#include "API.h"
#include "LightableGroupItem.h"

namespace Luna
{
  // Forwards
  class LightingJob;
  class Object;
  struct LightingJobMemberChangeArgs;

  /////////////////////////////////////////////////////////////////////////////
  // Class representing a group of shadow casters for a given lighting job.
  // 
  class ShadowGroupItem : public LightableGroupItem
  {
  private:
    static const std::string s_ShadowCasterName;

  public:
    ShadowGroupItem( wxTreeCtrlBase* tree, Luna::LightingJob* job );
    virtual ~ShadowGroupItem();

    static const std::string& GetShadowGroupLabel();
    virtual void SelectItem( Luna::SceneNode* node );

    // Overrides
  protected:
    virtual const std::string& GetLabel() const NOC_OVERRIDE;
    void RemoveAll( const ContextMenuArgsPtr& args ) NOC_OVERRIDE;
  public:
    virtual void Load() NOC_OVERRIDE;
    Undo::CommandPtr AddChild( Object* object ) NOC_OVERRIDE;
    virtual ContextMenuItemSet GetContextMenuItems() NOC_OVERRIDE;

    // Application event callbacks
  private:
    void ShadowCasterAdded( const LightingJobMemberChangeArgs& args );
    void ShadowCasterRemoved( const LightingJobMemberChangeArgs& args );
  };
}