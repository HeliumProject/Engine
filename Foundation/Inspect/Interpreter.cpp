#include "Interpreter.h"

#include "Foundation/Inspect/Canvas.h"
#include "Foundation/Inspect/Container.h"
#include "Foundation/Inspect/Controls/LabelControl.h"
#include "Foundation/Inspect/Controls/ButtonControl.h"

using namespace Helium;
using namespace Helium::Inspect;

std::set< std::stack< ContainerPtr >* > ContainerStackPointer::s_Stacks;

ContainerStackPointer::ContainerStackPointer()
{

}

ContainerStackPointer::~ContainerStackPointer()
{
    while ( !s_Stacks.empty() )
    {
        delete *s_Stacks.begin();
        s_Stacks.erase( *s_Stacks.begin() );
    }
}                

std::stack< ContainerPtr >& ContainerStackPointer::Get()
{
    std::stack< ContainerPtr >* pointer = (std::stack< ContainerPtr >*)GetPointer();
    
    if ( !pointer )
    {
        static Helium::Mutex mutex;
        Helium::TakeMutex lock ( mutex );
        SetPointer( pointer = new std::stack< ContainerPtr > );
        s_Stacks.insert( pointer );
    }

    return *pointer;
}

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