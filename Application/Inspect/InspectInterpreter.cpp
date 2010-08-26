#include "InspectInterpreter.h"

#include "Application/Inspect/Controls/InspectCanvas.h"
#include "Application/Inspect/Controls/InspectPanel.h"
#include "Application/Inspect/Controls/InspectLabel.h"
#include "Application/Inspect/Controls/InspectButton.h"

using namespace Helium::Inspect;

ST_Container& Interpreter::GetCurrentContainerStack()
{
  Helium::TakeMutex mutex( m_ContainerStackMutex );

  u32 threadId = GetCurrentThreadId();
  
  M_U32ContainerStack::iterator itr = m_ContainerStack.find( threadId );
  if ( itr == m_ContainerStack.end() )
  {
    ST_Container containerStack;
    itr = m_ContainerStack.insert( std::make_pair( threadId, containerStack ) ).first;
  }
  
  return itr->second;
}

void Interpreter::Add(Control* control)
{
  ST_Container& containerStack = GetCurrentContainerStack();
  containerStack.top()->AddChild(control);
}

void Interpreter::Push(Container* container)
{
  ST_Container& containerStack = GetCurrentContainerStack();
  containerStack.push(container);
}

Panel* Interpreter::PushContainer(const tstring& name, bool expanded)
{
  PanelPtr panel = m_Container->GetCanvas()->Create<Panel>( this );

  panel->SetText( name );
  panel->SetInterpreter( this );
  panel->SetExpanded( expanded );

  ST_Container& containerStack = GetCurrentContainerStack();
  containerStack.push(panel);

  return panel;
}

Container* Interpreter::PushContainer()
{
  ContainerPtr container = m_Container->GetCanvas()->Create<Container>( this );
  container->SetInterpreter( this );

  ST_Container& containerStack = GetCurrentContainerStack();
  containerStack.push(container);

  return container;
}

Container* Interpreter::Pop( bool setParent )
{
  ST_Container& containerStack = GetCurrentContainerStack();

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
  ST_Container& containerStack = GetCurrentContainerStack();

  if ( !containerStack.empty() )
  {
    return containerStack.top();
  }
  
  return NULL;
}

Label* Interpreter::AddLabel(const tstring& name)
{
  LabelPtr control = m_Container->GetCanvas()->Create<Label>( this );
  control->SetInterpreter( this );
  control->SetText( name );

  ST_Container& containerStack = GetCurrentContainerStack();
  containerStack.top()->AddChild(control);

  return control;
}

Button* Interpreter::AddButton( const ButtonClickedSignature::Delegate& listener )
{
  ButtonPtr control = m_Container->GetCanvas()->Create<Button>( this );
  control->SetInterpreter( this );
  control->ButtonClickedEvent().Add( listener );

  ST_Container& containerStack = GetCurrentContainerStack();
  containerStack.top()->AddChild(control);

  return control;
}