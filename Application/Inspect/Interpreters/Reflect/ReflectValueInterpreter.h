#pragma once

#include "Application/API.h"

#include "ReflectFieldInterpreter.h"

namespace Inspect
{
  class APPLICATION_API ReflectValueInterpreter : public ReflectFieldInterpreter
  {
  public:
    ReflectValueInterpreter (Container* container);

    virtual void InterpretField(const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent);
  };

  typedef Helium::SmartPtr<ReflectValueInterpreter> ReflectValueInterpreterPtr;
  typedef std::vector< ReflectValueInterpreterPtr > V_ReflectValueInterpreter;
}