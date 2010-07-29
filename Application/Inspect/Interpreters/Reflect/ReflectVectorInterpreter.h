#pragma once

#include "ReflectFieldInterpreter.h"

#include "Application/Inspect/Controls/Canvas.h"
#include "Application/Inspect/Controls/Container.h"

namespace Inspect
{
  class APPLICATION_API ReflectVectorInterpreter : public ReflectFieldInterpreter
  {
  public:
    ReflectVectorInterpreter( Container* container );

    virtual void InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent ) HELIUM_OVERRIDE;
  };

  typedef Helium::SmartPtr<ReflectVectorInterpreter> ReflectVectorInterpreterPtr;
  typedef std::vector< ReflectVectorInterpreterPtr > V_ReflectVectorInterpreterSmartPtr;
}