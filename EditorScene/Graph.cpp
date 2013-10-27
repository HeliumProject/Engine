#include "EditorScenePch.h"
#include "Graph.h"
#include "EditorScene/SceneNode.h"

#include <stack>

//#define SCENE_DEBUG_EVALUATE

HELIUM_DEFINE_CLASS( Helium::Editor::Graph );

using namespace Helium;
using namespace Helium::Editor;

Graph::Graph()
	: m_NextID (1)
	, m_CurrentID (0)
{

}

void Graph::Reset()
{
	for ( S_SceneNodeDumbPtr::const_iterator itr = m_OriginalNodes.begin(), end = m_OriginalNodes.end(); itr != end; ++itr )
	{
		(*itr)->Reset();
	}

	m_OriginalNodes.clear();

	for ( S_SceneNodeDumbPtr::const_iterator itr = m_IntermediateNodes.begin(), end = m_IntermediateNodes.end(); itr != end; ++itr )
	{
		(*itr)->Reset();
	}

	m_IntermediateNodes.clear();

	for ( S_SceneNodeDumbPtr::const_iterator itr = m_TerminalNodes.begin(), end = m_TerminalNodes.end(); itr != end; ++itr )
	{
		(*itr)->Reset();
	}

	m_TerminalNodes.clear();

	m_CurrentID = 0;
	m_NextID = 1;
}

uint32_t Graph::AssignVisitedID()
{
	m_NextID++;

	if (m_NextID == 0)
	{
		ResetVisitedIDs();
	}

	return m_NextID;
}

void Graph::ResetVisitedIDs()
{
	for ( S_SceneNodeDumbPtr::const_iterator itr = m_OriginalNodes.begin(), end = m_OriginalNodes.end(); itr != end; ++itr )
	{
		(*itr)->SetVisitedID(0);
	}

	for ( S_SceneNodeDumbPtr::const_iterator itr = m_IntermediateNodes.begin(), end = m_IntermediateNodes.end(); itr != end; ++itr )
	{
		(*itr)->SetVisitedID(0);
	}

	for ( S_SceneNodeDumbPtr::const_iterator itr = m_TerminalNodes.begin(), end = m_TerminalNodes.end(); itr != end; ++itr )
	{
		(*itr)->SetVisitedID(0);
	}
}

void Graph::Classify(Editor::SceneNode* n)
{
	if (n->GetAncestors().empty())
	{
		m_OriginalNodes.insert( n );
		m_IntermediateNodes.erase( n );
		m_TerminalNodes.erase( n );
	}
	else if (n->GetDescendants().empty())
	{
		m_OriginalNodes.erase( n );
		m_IntermediateNodes.erase( n );
		m_TerminalNodes.insert( n );
	}
	else
	{
		m_OriginalNodes.erase( n );
		m_IntermediateNodes.insert( n );
		m_TerminalNodes.erase( n );
	}
}

void Graph::AddNode(Editor::SceneNode* n)
{
	// Track this node
	Classify(n);

	// Make the node aware of the graph
	n->SetGraph( this );

	// Force Evaluation
	n->Dirty();

	// Reset visited status (just in case)
	n->SetVisitedID(0);
}

void Graph::RemoveNode(Editor::SceneNode* n)
{
	m_OriginalNodes.erase( n );
	m_IntermediateNodes.erase( n );
	m_TerminalNodes.erase( n );

	n->SetGraph( NULL );
}

uint32_t Graph::DirtyNode( Editor::SceneNode* node, GraphDirection direction )
{
	uint32_t count = 0;

	node->SetNodeState(direction, NodeStates::Dirty);
	count++;

	switch (direction)
	{
	case GraphDirections::Downstream:
		{
			std::stack<Editor::SceneNode*> descendantStack;

			for ( S_SceneNodeSmartPtr::const_iterator itr = node->GetDescendants().begin(), end = node->GetDescendants().end(); itr != end; ++itr )
			{
				if ( (*itr)->GetNodeState(direction) != NodeStates::Dirty )
				{
					descendantStack.push( *itr );
				}
			}

			while (!descendantStack.empty())
			{
				Editor::SceneNode* descendant = descendantStack.top();

				descendantStack.pop();

				descendant->SetNodeState(direction, NodeStates::Dirty);
				count++;

				for ( S_SceneNodeSmartPtr::const_iterator itr = descendant->GetDescendants().begin(), end = descendant->GetDescendants().end(); itr != end; ++itr )
				{
					if ( (*itr)->GetNodeState(direction) != NodeStates::Dirty )
					{
						descendantStack.push( *itr );
					}
				}
			}

			break;
		}

	case GraphDirections::Upstream:
		{
			std::stack<Editor::SceneNode*> ancestorStack;

			for ( S_SceneNodeDumbPtr::const_iterator itr = node->GetAncestors().begin(), end = node->GetAncestors().end(); itr != end; ++itr )
			{
				if ( (*itr)->GetNodeState(direction) != NodeStates::Dirty )
				{
					ancestorStack.push( *itr );
				}
			}

			while (!ancestorStack.empty())
			{
				Editor::SceneNode* ancestor = ancestorStack.top();

				ancestorStack.pop();

				ancestor->SetNodeState(direction, NodeStates::Dirty);
				count++;

				for ( S_SceneNodeDumbPtr::const_iterator itr = ancestor->GetAncestors().begin(), end = ancestor->GetAncestors().end(); itr != end; ++itr )
				{
					if ( (*itr)->GetNodeState(direction) != NodeStates::Dirty )
					{
						ancestorStack.push( *itr );
					}
				}
			}

			break;
		}
	}

	return count;
}

EvaluateResult Graph::EvaluateGraph(bool silent)
{
	EvaluateResult result;

	EDITOR_SCENE_EVALUATE_SCOPE_TIMER( ("") );

	uint64_t start = Helium::TimerGetClock();

	m_EvaluatedNodes.clear();

	for ( S_SceneNodeDumbPtr::const_iterator itr = m_TerminalNodes.begin(), end = m_TerminalNodes.end(); itr != end; ++itr )
	{
		if ((*itr)->GetNodeState(GraphDirections::Downstream) == NodeStates::Dirty)
		{
			Evaluate(*itr, GraphDirections::Downstream);
		}
	}

	for ( S_SceneNodeDumbPtr::const_iterator itr = m_OriginalNodes.begin(), end = m_OriginalNodes.end(); itr != end; ++itr )
	{
		if ((*itr)->GetNodeState(GraphDirections::Upstream) == NodeStates::Dirty)
		{
			Evaluate(*itr, GraphDirections::Upstream);
		}
	}

	result.m_NodeCount = (int)m_EvaluatedNodes.size();

	m_EvaluatedEvent.Raise( m_EvaluatedNodes );

	m_CleanupRoots.clear();

	result.m_TotalTime = Helium::CyclesToMillis(Helium::TimerGetClock() - start);

	return result;
}

void Graph::Evaluate(Editor::SceneNode* node, GraphDirection direction)
{
	switch (direction)
	{
	case GraphDirections::Downstream:
		{
			for ( S_SceneNodeDumbPtr::const_iterator itr = node->GetAncestors().begin(), end = node->GetAncestors().end(); itr != end; ++itr )
			{
				if ((*itr)->GetNodeState(direction) == NodeStates::Dirty)
				{
					Evaluate(*itr, direction);
				}
			}

			break;
		}

	case GraphDirections::Upstream:
		{
			for ( S_SceneNodeSmartPtr::const_iterator itr = node->GetDescendants().begin(), end = node->GetDescendants().end(); itr != end; ++itr )
			{
				if ((*itr)->GetNodeState(direction) == NodeStates::Dirty)
				{
					Evaluate(*itr, direction);
				}
			}

			break;
		}
	}

	// perform evaluate
	node->DoEvaluate(direction);

	m_EvaluatedNodes.insert( node );
}