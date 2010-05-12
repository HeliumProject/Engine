#pragma once

#include "ReflectFieldInterpreter.h"

#include "Inspect/Canvas.h"
#include "Inspect/Container.h"
#include "Inspect/StringData.h"

namespace Inspect
{
  // Forwards
  class Button;
  class Action;
  class List;
  typedef Nocturnal::SmartPtr<Action> ActionPtr;

  class INSPECTREFLECT_API ReflectArrayInterpreter : public ReflectFieldInterpreter
  {
  public:
    ReflectArrayInterpreter (Container* container);

    virtual void InterpretField(const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent) override;

  protected:
    virtual ActionPtr AddAddButton( List* list );
    virtual ActionPtr AddRemoveButton( List* list );
    virtual ActionPtr AddMoveUpButton( List* list );
    virtual ActionPtr AddMoveDownButton( List* list );

    // callbacks
    static void OnAdd( Button* button );
    static void OnRemove( Button* button );
    static void OnMoveUp( Button* button );
    static void OnMoveDown( Button* button );

    virtual void OnCreateFieldSerializer( Reflect::SerializerPtr & s ){ }
  };

  typedef Nocturnal::SmartPtr<ReflectArrayInterpreter> ReflectArrayInterpreterPtr;
  typedef std::vector< ReflectArrayInterpreterPtr > V_ReflectArrayInterpreterSmartPtr;
}