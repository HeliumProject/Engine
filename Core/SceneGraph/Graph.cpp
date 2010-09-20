/*#include "Precompile.h"*/
#include "Graph.h"
#include "Core/SceneGraph/SceneNode.h"

//#define SCENE_DEBUG_EVALUATE

using namespace Helium;
using namespace Helium::Core;

REFLECT_DEFINE_ABSTRACT( Graph );

void Graph::InitializeType()
{
  Reflect::RegisterClassType< Graph >( TXT( "Graph" ) );
}

void Graph::CleanupType()
{
  Reflect::UnregisterClassType< Graph >();
}

Graph::Graph()
: m_NextID (1)
, m_CurrentID (0)
{

}

void Graph::Reset()
{
  for each (Core::SceneNode* node in m_OriginalNodes)
  {
    node->Reset();
  }

  m_OriginalNodes.clear();

  for each (Core::SceneNode* node in m_IntermediateNodes)
  {
    node->Reset();
  }

  m_IntermediateNodes.clear();

  for each (Core::SceneNode* node in m_TerminalNodes)
  {
    node->Reset();
  }

  m_TerminalNodes.clear();

  m_CurrentID = 0;
  m_NextID = 1;
}

u32 Graph::AssignVisitedID()
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
  for each (Core::SceneNode* n in m_OriginalNodes)
  {
    n->SetVisitedID(0);
  }

  for each (Core::SceneNode* n in m_IntermediateNodes)
  {
    n->SetVisitedID(0);
  }

  for each (Core::SceneNode* n in m_TerminalNodes)
  {
    n->SetVisitedID(0);
  }
}

void Graph::Classify(Core::SceneNode* n)
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

void Graph::AddNode(Core::SceneNode* n)
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

void Graph::RemoveNode(Core::SceneNode* n)
{
  m_OriginalNodes.erase( n );
  m_IntermediateNodes.erase( n );
  m_TerminalNodes.erase( n );

  n->SetGraph( NULL );
}

u32 Graph::DirtyNode( Core::SceneNode* node, GraphDirection direction )
{
  u32 count = 0;

  node->SetNodeState(direction, NodeStates::Dirty);
  count++;

  switch (direction)
  {
  case GraphDirections::Downstream:
    {
      std::stack<Core::SceneNode*> descendantStack;

      for each (Core::SceneNode* d in node->GetDescendants())
      {
        if ( d->GetNodeState(direction) != NodeStates::Dirty )
        {
          descendantStack.push( d );
        }
      }

      while (!descendantStack.empty())
      {
        Core::SceneNode* descendant = descendantStack.top();

        descendantStack.pop();

        descendant->SetNodeState(direction, NodeStates::Dirty);
        count++;

        for each (Core::SceneNode* d in descendant->GetDescendants())
        {
          if ( d->GetNodeState(direction) != NodeStates::Dirty )
          {
            descendantStack.push( d );
          }
        }
      }

      break;
    }

  case GraphDirections::Upstream:
    {
      std::stack<Core::SceneNode*> ancestorStack;

      for each (Core::SceneNode* d in node->GetAncestors())
      {
        if ( d->GetNodeState(direction) != NodeStates::Dirty )
        {
          ancestorStack.push( d );
        }
      }

      while (!ancestorStack.empty())
      {
        Core::SceneNode* ancestor = ancestorStack.top();

        ancestorStack.pop();

        ancestor->SetNodeState(direction, NodeStates::Dirty);
        count++;

        for each (Core::SceneNode* d in ancestor->GetAncestors())
        {
          if ( d->GetNodeState(direction) != NodeStates::Dirty )
          {
            ancestorStack.push( d );
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

  CORE_EVALUATE_SCOPE_TIMER( ("") );

  u64 start = Helium::TimerGetClock();

  m_EvaluatedNodes.clear();

  for each (Core::SceneNode* n in m_TerminalNodes)
  {
    if (n->GetNodeState(GraphDirections::Downstream) == NodeStates::Dirty)
    {
      Evaluate(n, GraphDirections::Downstream);
    }
  }

  for each (Core::SceneNode* n in m_OriginalNodes)
  {
    if (n->GetNodeState(GraphDirections::Upstream) == NodeStates::Dirty)
    {
      Evaluate(n, GraphDirections::Upstream);
    }
  }

  result.m_NodeCount = (int)m_EvaluatedNodes.size();

  m_EvaluatedEvent.Raise( m_EvaluatedNodes );

  m_CleanupRoots.clear();

  result.m_TotalTime = Helium::CyclesToMillis(Helium::TimerGetClock() - start);

  return result;
}

void Graph::Evaluate(Core::SceneNode* node, GraphDirection direction)
{
  switch (direction)
  {
  case GraphDirections::Downstream:
    {
      for each (Core::SceneNode* ancestor in node->GetAncestors())
      {
        if (ancestor->GetNodeState(direction) == NodeStates::Dirty)
        {
          Evaluate(ancestor, direction);
        }
      }

      break;
    }

  case GraphDirections::Upstream:
    {
      for each (Core::SceneNode* descendant in node->GetDescendants())
      {
        if (descendant->GetNodeState(direction) == NodeStates::Dirty)
        {
          Evaluate(descendant, direction);
        }
      }

      break;
    }
  }

  // perform evaluate
  node->DoEvaluate(direction);

  m_EvaluatedNodes.insert( node );
}