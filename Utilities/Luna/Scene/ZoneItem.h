#pragma once

// Includes
#include "API.h"
#include "SceneNodeItem.h"

namespace Luna
{
  class Zone;

  /////////////////////////////////////////////////////////////////////////////
  // Represents lights within the lighting job tree control.
  // 
  class ZoneItem : public SceneNodeItem
  {
  private:
    Zone* m_Zone;

  public:
    ZoneItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, Zone* env );
    virtual ~ZoneItem();

    Zone* GetZone() const;

    // Overrides
    virtual Undo::CommandPtr GetRemoveCommand( const OS_SelectableDumbPtr& selection ) NOC_OVERRIDE;
  };
}