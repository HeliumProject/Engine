#include "SceneGraphPch.h"
#include "ReverseChildrenCommand.h"

using namespace Helium;
using namespace Helium::SceneGraph;

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
