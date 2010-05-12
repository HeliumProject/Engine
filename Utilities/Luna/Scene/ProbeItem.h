#pragma once

// Includes
#include "API.h"
#include "SceneNodeItem.h"

namespace Luna
{
  // Forwards
  class LightingJob;
  class CubeMapProbe;

  /////////////////////////////////////////////////////////////////////////////
  // Represents cube map probes within the lighting job tree control.
  // 
  class ProbeItem : public SceneNodeItem
  {
  private:
    Luna::CubeMapProbe* m_Probe;

  public:
    ProbeItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, Luna::CubeMapProbe* probe );
    virtual ~ProbeItem();

    Luna::CubeMapProbe* GetProbe() const;

    // Overrides
    virtual Undo::CommandPtr GetRemoveCommand( const OS_SelectableDumbPtr& selection ) NOC_OVERRIDE;
  };
}