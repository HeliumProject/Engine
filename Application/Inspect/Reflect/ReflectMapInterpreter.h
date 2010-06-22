#pragma once

#include "ReflectFieldInterpreter.h"

#include "Application/Inspect/Widgets/Canvas.h"
#include "Application/Inspect/Widgets/Container.h"

namespace Inspect
{
  class Button;
  class Container;

  class APPLICATION_API ReflectMapInterpreter : public ReflectFieldInterpreter
  {
  public:
    ReflectMapInterpreter( Container* container );
    virtual void InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent ) NOC_OVERRIDE;

  protected:

    void OnAdd( Button* button );
    void OnRemove( Button* button );
    void OnEdit( Button* button );
  };

  typedef Nocturnal::SmartPtr<ReflectMapInterpreter> ReflectMapInterpreterPtr;
  typedef std::vector< ReflectMapInterpreterPtr > V_ReflectMapInterpreterSmartPtr;
}