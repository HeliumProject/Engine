#include "Precompile.h"
#include "SceneGraph.h"
#include "SceneNode.h"

//#define LUNA_DEBUG_EVALUATE

using namespace Editor;

LUNA_DEFINE_TYPE( SceneGraph );

void SceneGraph::InitializeType()
{
  Reflect::RegisterClass< SceneGraph >( TXT( "SceneGraph" ) );
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
  for each (Editor::SceneNode* node in m_OriginalNodes)
  {
    node->Reset();
  }

  m_OriginalNodes.clear();

  for each (Editor::SceneNode* node in m_IntermediateNodes)
  {
    node->Reset();
  }

  m_IntermediateNodes.clear();

  for each (Editor::SceneNode* node in m_TerminalNodes)
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
  for each (Editor::SceneNode* n in m_OriginalNodes)
  {
    n->SetVisitedID(0);
  }

  for each (Editor::SceneNode* n in m_IntermediateNodes)
  {
    n->SetVisitedID(0);
  }

  for each (Editor::SceneNode* n in m_TerminalNodes)
  {
    n->SetVisitedID(0);
  }
}

void SceneGraph::Classify(Editor::SceneNode* n)
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

void SceneGraph::AddNode(Editor::SceneNode* n)
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

void SceneGraph::RemoveNode(Editor::SceneNode* n)
{
  m_OriginalNodes.erase( n );
  m_IntermediateNodes.erase( n );
  m_TerminalNodes.erase( n );

  n->SetGraph( NULL );
}

u32 SceneGraph::DirtyNode( Editor::SceneNode* node, GraphDirection direction )
{
  u32 count = 0;

  node->SetNodeState(direction, NodeStates::Dirty);
  count++;

  switch (direction)
  {
  case GraphDirections::Downstream:
    {
      std::stack<Editor::SceneNode*> descendantStack;

      for each (Editor::SceneNode* d in node->GetDescendants())
      {
        if ( d->GetNodeState(direction) != NodeStates::Dirty )
        {
          descendantStack.push( d );
        }
      }

      while (!descendantStack.empty())
      {
        Editor::SceneNode* descendant = descendantStack.top();

        descendantStack.pop();

        descendant->SetNodeState(direction, NodeStates::Dirty);
        count++;

        for each (Editor::SceneNode* d in descendant->GetDescendants())
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
      std::stack<Editor::SceneNode*> ancestorStack;

      for each (Editor::SceneNode* d in node->GetAncestors())
      {
        if ( d->GetNodeState(direction) != NodeStates::Dirty )
        {
          ancestorStack.push( d );
        }
      }

      while (!ancestorStack.empty())
      {
        Editor::SceneNode* ancestor = ancestorStack.top();

        ancestorStack.pop();

        ancestor->SetNodeState(direction, NodeStates::Dirty);
        count++;

        for each (Editor::SceneNode* d in ancestor->GetAncestors())
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

  u64 start = Helium::TimerGetClock();

  m_EvaluatedNodes.clear();

  for each (Editor::SceneNode* n in m_TerminalNodes)
  {
    if (n->GetNodeState(GraphDirections::Downstream) == NodeStates::Dirty)
    {
      Evaluate(n, GraphDirections::Downstream);
    }
  }

  for each (Editor::SceneNode* n in m_OriginalNodes)
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

void SceneGraph::Evaluate(Editor::SceneNode* node, GraphDirection direction)
{
  switch (direction)
  {
  case GraphDirections::Downstream:
    {
      for each (Editor::SceneNode* ancestor in node->GetAncestors())
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
      for each (Editor::SceneNode* descendant in node->GetDescendants())
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