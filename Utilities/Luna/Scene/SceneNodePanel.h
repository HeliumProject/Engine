#pragma once

#include "Luna/API.h"
#include "Application/Inspect/Data/Data.h"
#include "Application/Inspect/Widgets/InspectPanel.h"
#include "SceneNode.h"

namespace Luna
{
  class SceneNodePanel : public Inspect::Panel
  {
  public:
    SceneNodePanel(Enumerator* enumerator, const OS_SelectableDumbPtr& selection);
    virtual ~SceneNodePanel();
    virtual void Create() NOC_OVERRIDE;

  protected:
    Enumerator*          m_Enumerator;
    OS_SelectableDumbPtr  m_Selection;
  };
}
