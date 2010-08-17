#pragma once

#include <string>

#include "Foundation/Automation/Event.h"

#include "Foundation/Reflect/Serializer.h"

namespace Helium
{
    namespace Inspect
    {
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
}