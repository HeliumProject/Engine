#pragma once

#include "API.h"
#include "Zone.h"
#include "Core/Selectable.h"
#include "Inspect/Panel.h"
#include "InspectReflect/ReflectInterpreter.h"
#include "Core/Enumerator.h"

#include "Content/Zone.h"

namespace Luna
{
  class ZonePanel : public Inspect::Panel
  {
  public: 
    ZonePanel(Enumerator* enumerator, const OS_SelectableDumbPtr& selection); 
    virtual ~ZonePanel(); 

    virtual void Create() NOC_OVERRIDE; 

  private: 
    // m_Zones is a vector of elements because the ReflectInterpreter needs it that way. 
    Enumerator*                       m_Enumerator;
    OS_SelectableDumbPtr               m_Selection;
    OS_SelectableDumbPtr               m_RuntimeSelection; 
    std::vector<Reflect::Element*>     m_Zones; 

    Inspect::ReflectInterpreterPtr m_ReflectInterpreter; 
  };
}
