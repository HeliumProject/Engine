#pragma once

// Includes
#include "API.h"
#include "SceneNodeItem.h"

namespace Luna
{
  // Forwards
  class LightingJob;
  class Light;

  /////////////////////////////////////////////////////////////////////////////
  // Represents lights within the lighting job tree control.
  // 
  class LightItem : public SceneNodeItem
  {
  private:
    Luna::Light* m_Light;

  public:
    LightItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, Luna::Light* light );
    virtual ~LightItem();

    Luna::Light* GetLight() const;

    // Overrides
    virtual Undo::CommandPtr GetRemoveCommand( const OS_SelectableDumbPtr& selection ) NOC_OVERRIDE;
  };
}