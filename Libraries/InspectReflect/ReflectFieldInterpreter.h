#pragma once

#include "API.h"

#include "Application/Inspect/Widgets/Canvas.h"
#include "Application/Inspect/Interpreter.h"

namespace Inspect
{
  class INSPECTREFLECT_API ReflectFieldInterpreter : public Interpreter
  {
  public:
    ReflectFieldInterpreter (Container* container);

    virtual void InterpretField(const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent) = 0;

  protected:
    std::vector<Reflect::Element*> m_Instances;
    std::vector<Reflect::SerializerPtr> m_Serializers;
  };

  typedef Nocturnal::SmartPtr<ReflectFieldInterpreter> ReflectFieldInterpreterPtr;
  typedef std::vector< ReflectFieldInterpreterPtr > V_ReflectFieldInterpreter;
}