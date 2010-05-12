#pragma once

// Includes
#include "API.h"
#include "SceneNodeItem.h"

namespace Luna
{
  // Forwards
  class LightingJob;
  class LightingEnvironment;

  /////////////////////////////////////////////////////////////////////////////
  // Represents lights within the lighting job tree control.
  // 
  class LightingEnvironmentItem : public SceneNodeItem
  {
  private:
    Luna::LightingEnvironment* m_LightingEnvironment;

  public:
    LightingEnvironmentItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, Luna::LightingEnvironment* env );
    virtual ~LightingEnvironmentItem();

    Luna::LightingEnvironment* GetLightingEnvironment() const;

    // Overrides
    virtual Undo::CommandPtr GetRemoveCommand( const OS_SelectableDumbPtr& selection ) NOC_OVERRIDE;
  };
}