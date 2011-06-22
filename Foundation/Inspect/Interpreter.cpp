#include "FoundationPch.h"
#include "Interpreter.h"

#include "Foundation/Inspect/Canvas.h"
#include "Foundation/Inspect/Container.h"
#include "Foundation/Inspect/Controls/LabelControl.h"
#include "Foundation/Inspect/Controls/ButtonControl.h"

using namespace Helium;
using namespace Helium::Inspect;

std::multimap< uint32_t, std::stack< ContainerPtr >* > ContainerStackPointer::s_Stacks;

ContainerStackPointer::ContainerStackPointer()
{

}

ContainerStackPointer::~ContainerStackPointer()
{
    std::multimap< uint32_t, std::stack< ContainerPtr >* >::const_iterator itr = s_Stacks.lower_bound( m_Key );
    std::multimap< uint32_t, std::stack< ContainerPtr >* >::const_iterator end = s_Stacks.upper_bound( m_Key );
    for ( ; itr != end; ++itr )
    {
        delete itr->second;
    }

    std::multimap< uint32_t, std::stack< ContainerPtr >* >::iterator item;
    while ( ( item = s_Stacks.find( m_Key ) ) != s_Stacks.end() )
    {
        s_Stacks.erase( item );
    }
}                

std::stack< ContainerPtr >& ContainerStackPointer::Get()
{
    std::stack< ContainerPtr >* pointer = (std::stack< ContainerPtr >*)GetPointer();
    
    if ( !pointer )
    {
        static Helium::Mutex mutex;
        Helium::MutexScopeLock lock ( mutex );
        SetPointer( pointer = new std::stack< ContainerPtr > );
        s_Stacks.insert( std::multimap< uint32_t, std::stack< ContainerPtr >* >::value_type( m_Key, pointer ) );
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
    ContainerPtr container = CreateControl<Container>();
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