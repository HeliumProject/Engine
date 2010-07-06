#pragma once

#include "Pipeline/API.h"
#include "Foundation/File/Path.h"

#include "Foundation/Component/Component.h"

namespace Asset
{
  class PIPELINE_API DependenciesComponent : public Component::ComponentBase
  {
  public:

      std::set< Nocturnal::Path > m_Paths;

  public:
    REFLECT_DECLARE_CLASS( DependenciesComponent, ComponentBase );

    static void EnumerateClass( Reflect::Compositor<DependenciesComponent>& comp );

    virtual Component::ComponentUsage GetComponentUsage() const NOC_OVERRIDE { return Component::ComponentUsages::Class; }
  };

  typedef Nocturnal::SmartPtr< DependenciesComponent > DependenciesComponentPtr;
}