#include "FoundationPch.h"
#include "BatchCommand.h"

using namespace Helium;

BatchCommand::BatchCommand()
    : m_IsSignificant( false )
{

}

BatchCommand::BatchCommand(const V_UndoCommandSmartPtr& objects)
    : m_IsSignificant( false )
{
    Set( objects );
}

BatchCommand::~BatchCommand()
{
}

void BatchCommand::Set(const V_UndoCommandSmartPtr& commands)
{
    m_Commands = commands;

    V_UndoCommandSmartPtr::iterator itr = m_Commands.begin();
    V_UndoCommandSmartPtr::iterator end = m_Commands.end();
    for ( m_IsSignificant = false; itr != end && !m_IsSignificant; ++itr )
    {
        UndoCommandPtr& command = *itr;
        m_IsSignificant |= command->IsSignificant();
    }
}

void BatchCommand::Push(const UndoCommandPtr& command)
{
    if (command.ReferencesObject())
    {
        m_IsSignificant |= command->IsSignificant();
        m_Commands.push_back(command);
    }
}

void BatchCommand::Undo()
{
    V_UndoCommandSmartPtr::reverse_iterator rItr = m_Commands.rbegin();
    V_UndoCommandSmartPtr::reverse_iterator rEnd = m_Commands.rend();
    for ( ; rItr != rEnd; ++rItr )
    {
        UndoCommandPtr& command = *rItr;
        command->Undo();
    }
}

void BatchCommand::Redo()
{
    V_UndoCommandSmartPtr::iterator itr = m_Commands.begin();
    V_UndoCommandSmartPtr::iterator end = m_Commands.end();
    for ( ; itr != end; ++itr )
    {
        UndoCommandPtr& command = *itr;
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
