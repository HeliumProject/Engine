#pragma once

#include "Application/Inspect/Widgets/InspectPanel.h"
#include "Application/Inspect/Reflect/ReflectInterpreter.h"
#include "Core/Enumerator.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // UI panel containing controls pertaining to lights
  // 
  class LightPanel : public Inspect::Panel
  {
  private:
    Enumerator* m_Enumerator;
    OS_SelectableDumbPtr m_Selection;
    Inspect::ReflectInterpreterPtr  m_ReflectInterpreter;

  public:
    LightPanel( Enumerator* enumerator, const OS_SelectableDumbPtr& selection );
    virtual void Create() NOC_OVERRIDE;
  };
}
