#include "BatchCommand.h"

using namespace Helium::Undo;

BatchCommand::BatchCommand()
: m_IsSignificant( false )
{

}

BatchCommand::BatchCommand(const V_CommandSmartPtr& objects)
: m_IsSignificant( false )
{
  Set( objects );
}

BatchCommand::~BatchCommand()
{
}

void BatchCommand::Set(const V_CommandSmartPtr& commands)
{
  m_Commands = commands;

  V_CommandSmartPtr::iterator itr = m_Commands.begin();
  V_CommandSmartPtr::iterator end = m_Commands.end();
  for ( m_IsSignificant = false; itr != end && !m_IsSignificant; ++itr )
  {
    CommandPtr& command = *itr;
    m_IsSignificant |= command->IsSignificant();
  }
}

void BatchCommand::Push(const CommandPtr& command)
{
  if (command.ReferencesObject())
  {
    m_IsSignificant |= command->IsSignificant();
    m_Commands.push_back(command);
  }
}

void BatchCommand::Undo()
{
  V_CommandSmartPtr::reverse_iterator rItr = m_Commands.rbegin();
  V_CommandSmartPtr::reverse_iterator rEnd = m_Commands.rend();
  for ( ; rItr != rEnd; ++rItr )
  {
    CommandPtr& command = *rItr;
    command->Undo();
  }
}

void BatchCommand::Redo()
{
  V_CommandSmartPtr::iterator itr = m_Commands.begin();
  V_CommandSmartPtr::iterator end = m_Commands.end();
  for ( ; itr != end; ++itr )
  {
    CommandPtr& command = *itr;
    command->Redo();
  }
}

bool BatchCommand::IsSignificant() const
{
  return m_IsSignificant;
}

bool BatchCommand::IsEmpty() const
{
  return m_Commands.empty();
}
