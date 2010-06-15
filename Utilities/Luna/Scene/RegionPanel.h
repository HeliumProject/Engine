#pragma once

#include "Luna/API.h"
#include "Region.h"
#include "Core/Selectable.h"
#include "Inspect/Panel.h"
#include "InspectReflect/ReflectInterpreter.h"
#include "Core/Enumerator.h"

#include "Content/Region.h"

namespace Luna
{
  class RegionPanel : public Inspect::Panel
  {
  public: 
    RegionPanel(Enumerator* enumerator, const OS_SelectableDumbPtr& selection); 
    virtual ~RegionPanel(); 

    virtual void Create() NOC_OVERRIDE; 

  private: 
    // m_Regions is a vector of elements because the ReflectInterpreter
    // needs it that way. 
    Enumerator*                       m_Enumerator;
    OS_SelectableDumbPtr               m_Selection;
    OS_SelectableDumbPtr               m_RuntimeSelection; 
    std::vector<Reflect::Element*>     m_Regions; 

    Inspect::ReflectInterpreterPtr m_ReflectInterpreter; 
  };
}
