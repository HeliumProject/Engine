#include "Precompile.h"
#include "ReverseChildrenCommand.h"

using namespace Luna;

ReverseChildrenCommand::ReverseChildrenCommand( const HierarchyNodePtr& node )
: m_Node( node )
{
  m_Node->ReverseChildren();
}

ReverseChildrenCommand::~ReverseChildrenCommand()
{
}

void ReverseChildrenCommand::Undo()
{
  m_Node->ReverseChildren();
}

void ReverseChildrenCommand::Redo()
{
  m_Node->ReverseChildren();
}
