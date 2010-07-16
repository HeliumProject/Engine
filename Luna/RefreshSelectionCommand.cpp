#include "Precompile.h"
#include "RefreshSelectionCommand.h"
#include "Selection.h"

using namespace Luna;

RefreshSelectionCommand::RefreshSelectionCommand( Selection* selection ) 
: m_Selection( selection )
{
  NOC_ASSERT( m_Selection );
}

RefreshSelectionCommand::~RefreshSelectionCommand()
{
}

void RefreshSelectionCommand::Undo()
{
  m_Selection->Refresh();
}

void RefreshSelectionCommand::Redo()
{
  m_Selection->Refresh();
}
