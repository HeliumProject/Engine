#include "SceneGraphPch.h"
#include "ParentCommand.h"
#include "SceneGraph/Transform.h"
#include "Reflect/Object.h"

using namespace Helium;
using namespace Helium::SceneGraph;

ParentCommand::ParentCommand(const HierarchyNodePtr& child, const HierarchyNodePtr& parent)
{
	m_Node = child;
	m_NextParent = parent;
	m_PreviousParent = child->GetParent();

	m_Node->SetParent( m_NextParent.Ptr() );

	SceneGraph::Transform* transform = Reflect::SafeCast< SceneGraph::Transform >( m_Node );
	if ( transform )
	{
		Push( transform->ComputeObjectComponents() );
	}
}

void ParentCommand::Undo()
{
	Swap();

	Base::Undo();
}

void ParentCommand::Redo()
{
	Swap();

	Base::Redo();
}

void ParentCommand::Swap()
{
	// swap parents
	HierarchyNodePtr n = m_NextParent;
	m_NextParent = m_PreviousParent;
	m_PreviousParent = n;

	m_Node->SetParent( m_NextParent.Ptr() );
}