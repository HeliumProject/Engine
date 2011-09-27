#include "FoundationPch.h"
#include "ExistenceCommand.h"

using namespace Helium;

ExistenceCommand::ExistenceCommand( ExistenceAction action, FunctionCallerPtr add, FunctionCallerPtr remove, bool redo )
    : m_Action( action )
    , m_Add( add )
    , m_Remove( remove )
{
    if ( redo )
    {
        Redo();
    }
}

ExistenceCommand::~ExistenceCommand()
{
}

void ExistenceCommand::Undo()
{
    switch ( m_Action )
    {
    case ExistenceActions::Add:
        ( *m_Remove )();
        break;

    case ExistenceActions::Remove:
        ( *m_Add )();
        break;
    }
}

void ExistenceCommand::Redo()
{
    switch ( m_Action )
    {
    case ExistenceActions::Add:
        ( *m_Add )();
        break;

    case ExistenceActions::Remove:
        ( *m_Remove )();
        break;
    }
}
