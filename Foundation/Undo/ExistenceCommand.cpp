#include "ExistenceCommand.h"

using namespace Helium::Undo;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
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

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ExistenceCommand::~ExistenceCommand()
{
}

///////////////////////////////////////////////////////////////////////////////
// Undoes the previous action.
// 
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

///////////////////////////////////////////////////////////////////////////////
// Performs the action again.
// 
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
