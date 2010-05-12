#pragma once

#include "API.h"
#include "ParametricKeyControl.h"
#include "InspectReflect/ReflectFieldInterpreter.h"

namespace Inspect
{
  class INSPECTCONTENT_API ParametricKeyInterpreter : public ReflectFieldInterpreter
  {
  public:
    ParametricKeyInterpreter( Container* container );

    virtual void InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent );
  };

  typedef Nocturnal::SmartPtr<ParametricKeyInterpreter> ParametricKeyInterpreterPtr;
  typedef std::vector< ParametricKeyInterpreterPtr > V_ParametricKeyInterpreter;
}