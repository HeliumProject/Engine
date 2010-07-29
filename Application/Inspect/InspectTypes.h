#pragma once

#include <string>

#include "Foundation/Automation/Event.h"

#include "Foundation/Reflect/Serializer.h"

namespace Inspect
{
  //
  // Key/Data pair
  //

  struct Item
  {
    tstring m_Key;
    tstring m_Data;

    Item()
    {

    }

    Item(const tstring& key, const tstring& data)
    {
      m_Key = key;
      m_Data = data;
    }
  };

  typedef std::vector<Item> V_Item;


  //
  // Events
  //

  typedef Helium::Signature< void, class Button* > ActionSignature;
  typedef Helium::Signature< void, class Control* > ControlSignature;

  struct ChangeArgs
  {
    class Control* m_Control;

    ChangeArgs(class Control* control)
      : m_Control (control)
    {

    }
  };
  typedef Helium::Signature<void, const ChangeArgs&> ChangedSignature;

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
  typedef Helium::Signature<bool, const ChangingArgs&> ChangingSignature;


  //
  // Forwards
  //

  class Control;
  typedef Helium::SmartPtr<Control> ControlPtr;

  class Container;
  typedef Helium::SmartPtr<Container> ContainerPtr;

  class Group;
  typedef Helium::SmartPtr<Group> GroupPtr;

  class Panel;
  typedef Helium::SmartPtr<Panel> PanelPtr;

  class Canvas;
  typedef Helium::SmartPtr<Canvas> CanvasPtr;

  class Interpreter;
  typedef Helium::SmartPtr<Interpreter> InterpreterPtr;
}