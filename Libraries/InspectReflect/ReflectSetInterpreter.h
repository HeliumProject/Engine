#pragma once

#include "ReflectFieldInterpreter.h"

#include "Application/Inspect/Widgets/Canvas.h"
#include "Application/Inspect/Widgets/Container.h"

namespace Inspect
{
  class Button;
  class Container;

  class INSPECTREFLECT_API ReflectSetInterpreter : public ReflectFieldInterpreter
  {
  public:
    ReflectSetInterpreter( Container* container );

    virtual void InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent ) NOC_OVERRIDE;

  private:
    // callbacks
    void OnAdd( Button* button );
    void OnRemove( Button* button );
  };

  typedef Nocturnal::SmartPtr<ReflectSetInterpreter> ReflectSetInterpreterPtr;
  typedef std::vector< ReflectSetInterpreterPtr > V_ReflectSetInterpreterSmartPtr;
}