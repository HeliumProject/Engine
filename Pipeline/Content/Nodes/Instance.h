#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Content/Nodes/PivotTransform.h"

namespace Symbol
{
  class UDTInstance; 
  typedef Nocturnal::SmartPtr<UDTInstance> UDTInstancePtr; 
}

namespace Content
{
  class PIPELINE_API Instance NOC_ABSTRACT : public PivotTransform
  {
  private:
    REFLECT_DECLARE_ABSTRACT(Instance, PivotTransform);
    static void EnumerateClass( Reflect::Compositor<Instance>& comp );

  public:
    Instance ();
    Instance (const Nocturnal::TUID& id);

  public:
    // Visibility preferences for this instance
    bool  m_Solid;
    bool  m_SolidOverride;
    bool  m_Transparent;
    bool  m_TransparentOverride;
  };

  typedef Nocturnal::SmartPtr<Instance> InstancePtr;
  typedef std::vector<InstancePtr> V_Instance;
}