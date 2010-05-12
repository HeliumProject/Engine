#pragma once

#include "AttributeGroup.h"
#include "Inspect/Panel.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // UI panel containing all controls pertianing to lighting volumes.
  // 
  class LightingVolumePanel : public Inspect::Panel
  {
  private:
    Enumerator* m_Enumerator;
    OS_SelectableDumbPtr m_Selection;
    Inspect::ReflectInterpreterPtr  m_ReflectInterpreter;

  public:
    LightingVolumePanel( Enumerator* enumerator, const OS_SelectableDumbPtr& selection );
    virtual void Create() NOC_OVERRIDE;
  };
}
