#include "InspectInterpreter.h"

#include "Foundation/Inspect/Controls/CanvasControl.h"
#include "Foundation/Inspect/Controls/ContainerControl.h"
#include "Foundation/Inspect/Controls/LabelControl.h"
#include "Foundation/Inspect/Controls/ButtonControl.h"

using namespace Helium;
using namespace Helium::Inspect;

void Interpreter::Add(Control* control)
{
    m_ContainerStack.Get().top()->AddChild(control);
}

void Interpreter::Push(Container* container)
{
    m_ContainerStack.Get().push(container);
}

Container* Interpreter::PushContainer( const tstring& name )
{
    ContainerPtr container = new Container ();
    container->a_Name.Set( name );

    m_ContainerStack.Get().push( container );

    return container;
}

Container* Interpreter::Pop( bool setParent )
{
    ContainerPtr child = m_ContainerStack.Get().top();

    m_ContainerStack.Get().pop();

    if ( setParent )
    {
        if ( m_ContainerStack.Get().empty() )
        {
            m_Container->AddChild(child);
        }
        else
        {
            Container* parent = m_ContainerStack.Get().top();

            parent->AddChild(child);
        }
    }

    return child;
}

Container* Interpreter::Top()
{
    if ( !m_ContainerStack.Get().empty() )
    {
        return m_ContainerStack.Get().top();
    }

    return NULL;
}

Label* Interpreter::AddLabel(const tstring& name)
{
    LabelPtr control = new Label ();
    control->Bind( new StringFormatter<tstring>( new tstring( name ), true ) );

    m_ContainerStack.Get().top()->AddChild(control);

    return control;
}

Button* Interpreter::AddButton( const ButtonClickedSignature::Delegate& listener )
{
    ButtonPtr control = new Button ();
    control->ButtonClickedEvent().Add( listener );

    m_ContainerStack.Get().top()->AddChild(control);

    return control;
}