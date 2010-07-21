#pragma once

#include "Luna/API.h"
#include "Application/Inspect/Data/Data.h"
#include "Application/Inspect/Controls/InspectPanel.h"
#include "SceneNode.h"

namespace Luna
{
  class SceneNodePanel : public Inspect::Panel
  {
  public:
    SceneNodePanel(PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection);
    virtual ~SceneNodePanel();
    virtual void Create() NOC_OVERRIDE;

  protected:
    PropertiesGenerator*          m_Generator;
    OS_SelectableDumbPtr  m_Selection;
  };
}
