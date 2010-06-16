#pragma once

#include <string>

#include "Foundation/Automation/Event.h"

#include "Reflect/Serializer.h"

namespace Inspect
{
  //
  // Key/Data pair
  //

  struct Item
  {
    std::string m_Key;
    std::string m_Data;

    Item()
    {

    }

    Item(const std::string& key, const std::string& data)
    {
      m_Key = key;
      m_Data = data;
    }
  };

  typedef std::vector<Item> V_Item;


  //
  // Events
  //

  typedef Nocturnal::Signature< void, class Button* > ActionSignature;
  typedef Nocturnal::Signature< void, class Control* > ControlSignature;

  struct ChangeArgs
  {
    class Control* m_Control;

    ChangeArgs(class Control* control)
      : m_Control (control)
    {

    }
  };
  typedef Nocturnal::Signature<void, const ChangeArgs&> ChangedSignature;

  struct ChangingArgs : public ChangeArgs
  {
    Reflect::SerializerPtr m_NewValue;
    bool m_Preview;

    ChangingArgs(class Control* control, const Reflect::SerializerPtr& newValue, bool preview)
      : ChangeArgs (control)
      , m_NewValue (newValue)
      , m_Preview (preview)
    {

    }
  };
  typedef Nocturnal::Signature<bool, const ChangingArgs&> ChangingSignature;


  //
  // Forwards
  //

  class Control;
  typedef Nocturnal::SmartPtr<Control> ControlPtr;

  class Container;
  typedef Nocturnal::SmartPtr<Container> ContainerPtr;

  class Group;
  typedef Nocturnal::SmartPtr<Group> GroupPtr;

  class Panel;
  typedef Nocturnal::SmartPtr<Panel> PanelPtr;

  class Canvas;
  typedef Nocturnal::SmartPtr<Canvas> CanvasPtr;

  class Interpreter;
  typedef Nocturnal::SmartPtr<Interpreter> InterpreterPtr;
}