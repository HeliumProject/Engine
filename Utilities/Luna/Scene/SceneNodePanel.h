#pragma once

#include "Luna/API.h"
#include "Inspect/Data.h"
#include "Inspect/Panel.h"
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
