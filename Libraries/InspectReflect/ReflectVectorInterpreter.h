#pragma once

#include "ReflectFieldInterpreter.h"

#include "Application/Inspect/Widgets/Canvas.h"
#include "Application/Inspect/Widgets/Container.h"

namespace Inspect
{
  class INSPECTREFLECT_API ReflectVectorInterpreter : public ReflectFieldInterpreter
  {
  public:
    ReflectVectorInterpreter( Container* container );

    virtual void InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent ) NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr<ReflectVectorInterpreter> ReflectVectorInterpreterPtr;
  typedef std::vector< ReflectVectorInterpreterPtr > V_ReflectVectorInterpreterSmartPtr;
}