#include "Precompile.h"
#include "SceneGraph.h"
#include "SceneNode.h"

//#define LUNA_DEBUG_EVALUATE

using namespace Luna;

LUNA_DEFINE_TYPE( SceneGraph );

void SceneGraph::InitializeType()
{
  Reflect::RegisterClass< SceneGraph >( "SceneGraph" );
}

void SceneGraph::CleanupType()
{
  Reflect::UnregisterClass< SceneGraph >();
}

SceneGraph::SceneGraph()
: m_NextID (1)
, m_CurrentID (0)
{

}

void SceneGraph::Reset()
{
  for each (Luna::SceneNode* node in m_OriginalNodes)
  {
    node->Reset();
  }

  m_OriginalNodes.clear();

  for each (Luna::SceneNode* node in m_IntermediateNodes)
  {
    node->Reset();
  }

  m_IntermediateNodes.clear();

  for each (Luna::SceneNode* node in m_TerminalNodes)
  {
    node->Reset();
  }

  m_TerminalNodes.clear();

  m_CurrentID = 0;
  m_NextID = 1;
}

u32 SceneGraph::AssignVisitedID()
{
  m_NextID++;

  if (m_NextID == 0)
  {
    ResetVisitedIDs();
  }
  
  return m_NextID;
}

void SceneGraph::ResetVisitedIDs()
{
  for each (Luna::SceneNode* n in m_OriginalNodes)
  {
    n->SetVisitedID(0);
  }

  for each (Luna::SceneNode* n in m_IntermediateNodes)
  {
    n->SetVisitedID(0);
  }

  for each (Luna::SceneNode* n in m_TerminalNodes)
  {
    n->SetVisitedID(0);
  }
}

void SceneGraph::Classify(Luna::SceneNode* n)
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

void SceneGraph::AddNode(Luna::SceneNode* n)
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

void SceneGraph::RemoveNode(Luna::SceneNode* n)
{
  m_OriginalNodes.erase( n );
  m_IntermediateNodes.erase( n );
  m_TerminalNodes.erase( n );

  n->SetGraph( NULL );
}

u32 SceneGraph::DirtyNode( Luna::SceneNode* node, GraphDirection direction )
{
  u32 count = 0;

  node->SetNodeState(direction, NodeStates::Dirty);
  count++;

  switch (direction)
  {
  case GraphDirections::Downstream:
    {
      std::stack<Luna::SceneNode*> descendantStack;

      for each (Luna::SceneNode* d in node->GetDescendants())
      {
        if ( d->GetNodeState(direction) != NodeStates::Dirty )
        {
          descendantStack.push( d );
        }
      }

      while (!descendantStack.empty())
      {
        Luna::SceneNode* descendant = descendantStack.top();

        descendantStack.pop();

        descendant->SetNodeState(direction, NodeStates::Dirty);
        count++;

        for each (Luna::SceneNode* d in descendant->GetDescendants())
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
      std::stack<Luna::SceneNode*> ancestorStack;

      for each (Luna::SceneNode* d in node->GetAncestors())
      {
        if ( d->GetNodeState(direction) != NodeStates::Dirty )
        {
          ancestorStack.push( d );
        }
      }

      while (!ancestorStack.empty())
      {
        Luna::SceneNode* ancestor = ancestorStack.top();

        ancestorStack.pop();

        ancestor->SetNodeState(direction, NodeStates::Dirty);
        count++;

        for each (Luna::SceneNode* d in ancestor->GetAncestors())
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

EvaluateResult SceneGraph::EvaluateGraph(bool silent)
{
  EvaluateResult result;

  LUNA_SCENE_EVALUATE_SCOPE_TIMER( ("") );

  u64 start = Platform::TimerGetClock();

  m_EvaluatedNodes.clear();

  for each (Luna::SceneNode* n in m_TerminalNodes)
  {
    if (n->GetNodeState(GraphDirections::Downstream) == NodeStates::Dirty)
    {
      Evaluate(n, GraphDirections::Downstream);
    }
  }

  for each (Luna::SceneNode* n in m_OriginalNodes)
  {
    if (n->GetNodeState(GraphDirections::Upstream) == NodeStates::Dirty)
    {
      Evaluate(n, GraphDirections::Upstream);
    }
  }

  result.m_NodeCount = (int)m_EvaluatedNodes.size();

  m_EvaluatedEvent.Raise( m_EvaluatedNodes );

  m_CleanupRoots.clear();

  result.m_TotalTime = Platform::CyclesToMillis(Platform::TimerGetClock() - start);

  return result;
}

void SceneGraph::Evaluate(Luna::SceneNode* node, GraphDirection direction)
{
  switch (direction)
  {
  case GraphDirections::Downstream:
    {
      for each (Luna::SceneNode* ancestor in node->GetAncestors())
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
      for each (Luna::SceneNode* descendant in node->GetDescendants())
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