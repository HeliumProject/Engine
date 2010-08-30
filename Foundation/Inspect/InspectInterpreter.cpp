#include "InspectInterpreter.h"

#include "Foundation/Inspect/Controls/InspectCanvas.h"
#include "Foundation/Inspect/Controls/InspectContainer.h"
#include "Foundation/Inspect/Controls/InspectLabel.h"
#include "Foundation/Inspect/Controls/InspectButton.h"

using namespace Helium;
using namespace Helium::Inspect;

std::stack< ContainerPtr >& Interpreter::GetCurrentContainerStack()
{
    std::stack< ContainerPtr >* stack = m_ContainerStack;
    return *stack;
}

void Interpreter::Add(Control* control)
{
    std::stack< ContainerPtr >& containerStack = GetCurrentContainerStack();
    containerStack.top()->AddChild(control);
}

void Interpreter::Push(Container* container)
{
    std::stack< ContainerPtr >& containerStack = GetCurrentContainerStack();
    containerStack.push(container);
}

Container* Interpreter::PushContainer( const tstring& name )
{
    ContainerPtr container = new Container ();
    container->a_Name.Set( name );

    std::stack< ContainerPtr >& containerStack = GetCurrentContainerStack();
    containerStack.push( container );

    return container;
}

Container* Interpreter::Pop( bool setParent )
{
    std::stack< ContainerPtr >& containerStack = GetCurrentContainerStack();

    ContainerPtr child = containerStack.top();

    containerStack.pop();

    if ( setParent )
    {
        if (containerStack.empty())
        {
            m_Container->AddChild(child);
        }
        else
        {
            Container* parent = containerStack.top();

            parent->AddChild(child);
        }
    }

    return child;
}

Container* Interpreter::Top()
{
    std::stack< ContainerPtr >& containerStack = GetCurrentContainerStack();

    if ( !containerStack.empty() )
    {
        return containerStack.top();
    }

    return NULL;
}

Label* Interpreter::AddLabel(const tstring& name)
{
    LabelPtr control = new Label ();
    control->Bind( new StringFormatter<tstring>( new tstring( name ), true ) );

    std::stack< ContainerPtr >& containerStack = GetCurrentContainerStack();
    containerStack.top()->AddChild(control);

    return control;
}

Button* Interpreter::AddButton( const ButtonClickedSignature::Delegate& listener )
{
    ButtonPtr control = new Button ();
    control->ButtonClickedEvent().Add( listener );

    std::stack< ContainerPtr >& containerStack = GetCurrentContainerStack();
    containerStack.top()->AddChild(control);

    return control;
}