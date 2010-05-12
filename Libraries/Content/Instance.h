#pragma once

#include "API.h"
#include "PivotTransform.h"

namespace Symbol
{
  class UDTInstance; 
  typedef Nocturnal::SmartPtr<UDTInstance> UDTInstancePtr; 
}

namespace Content
{
  class CONTENT_API Instance NOC_ABSTRACT : public PivotTransform
  {
  private:
    REFLECT_DECLARE_ABSTRACT(Instance, PivotTransform);
    static void EnumerateClass( Reflect::Compositor<Instance>& comp );

  public:
    Instance ();
    Instance (const UniqueID::TUID& id);

  public:
    // The overidden type of the node
    std::string m_ConfiguredType;

    // Visibility preferences for this instance
    bool  m_Solid;
    bool  m_SolidOverride;
    bool  m_Transparent;
    bool  m_TransparentOverride;
  };

  typedef Nocturnal::SmartPtr<Instance> InstancePtr;
  typedef std::vector<InstancePtr> V_Instance;
}