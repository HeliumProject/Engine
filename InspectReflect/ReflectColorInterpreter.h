#pragma once

#include "ReflectFieldInterpreter.h"

#include "Inspect/Canvas.h"
#include "Inspect/Container.h"

namespace Inspect
{
  class INSPECTREFLECT_API ReflectColorInterpreter : public ReflectFieldInterpreter
  {
  public:
    ReflectColorInterpreter( Container* container );

    virtual void InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent ) NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr<ReflectColorInterpreter> ReflectColorInterpreterPtr;
  typedef std::vector< ReflectColorInterpreterPtr > V_ReflectColorInterpreterSmartPtr;
}