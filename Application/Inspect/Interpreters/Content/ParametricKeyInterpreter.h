#pragma once

#include "Application/API.h"
#include "ParametricKeyControl.h"
#include "Application/Inspect/Interpreters/Reflect/ReflectFieldInterpreter.h"

namespace Inspect
{
  class APPLICATION_API ParametricKeyInterpreter : public ReflectFieldInterpreter
  {
  public:
    ParametricKeyInterpreter( Container* container );

    virtual void InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent );
  };

  typedef Helium::SmartPtr<ParametricKeyInterpreter> ParametricKeyInterpreterPtr;
  typedef std::vector< ParametricKeyInterpreterPtr > V_ParametricKeyInterpreter;
}