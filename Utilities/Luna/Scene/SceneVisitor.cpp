#include "Precompile.h"
#include "SceneVisitor.h"

#include "Pick.h"

#include "Entity.h"
#include "Scene.h"
#include "Transform.h"
#include "HierarchyNodeType.h"

using namespace Luna;

TraversalAction HierarchyChildTraverser::VisitHierarchyNode(Luna::HierarchyNode* node)
{
  S_HierarchyNodeDumbPtr::const_iterator itr = node->GetChildren().begin();
  S_HierarchyNodeDumbPtr::const_iterator end = node->GetChildren().end();
  for ( ; itr != end; ++itr )
  {
    m_Children.Append( *itr );
  }

  return TraversalActions::Continue;
}

HierarchyRenderTraverser::HierarchyRenderTraverser(RenderVisitor* renderVisitor)
: m_RenderVisitor(renderVisitor)
{

}

TraversalAction HierarchyRenderTraverser::VisitHierarchyNode(Luna::HierarchyNode* node)
{
  TraversalAction action;

  Math::Matrix4 matrix = m_RenderVisitor->State().m_Matrix;

  m_RenderVisitor->State().m_Matrix = node->GetTransform()->GetGlobalTransform() * m_RenderVisitor->State().m_Matrix;

  if (node->BoundsCheck(m_RenderVisitor->State().m_Matrix))
  {
    if (node->IsVisible())
    {
      // render this node
      node->Render( m_RenderVisitor );
    }

    // keep traversing
    action = TraversalActions::Continue;
  }
  else
  {
    // not visible, prune traversal
    action = TraversalActions::Prune;
  }

  m_RenderVisitor->State().m_Matrix = matrix;

  return action;
}

HierarchyPickTraverser::HierarchyPickTraverser(PickVisitor* pickVisitor)
: m_PickVisitor (pickVisitor)
{

}

TraversalAction HierarchyPickTraverser::VisitHierarchyNode(Luna::HierarchyNode* node)
{
  TraversalAction action;

  Math::Matrix4 matrix = m_PickVisitor->State().m_Matrix;

  m_PickVisitor->State().m_Matrix = node->GetTransform()->GetGlobalTransform() * m_PickVisitor->State().m_Matrix;
  
  if (node->BoundsCheck(m_PickVisitor->State().m_Matrix))
  {
    if (node->IsVisible())
    {
      m_PickVisitor->SetCurrentObject (node, m_PickVisitor->State().m_Matrix);
      
      if (m_PickVisitor->IntersectsBox(node->GetObjectHierarchyBounds()))
      {
        // pick this node
        node->Pick( m_PickVisitor ); 
      }
    }

    // keep traversin
    action = TraversalActions::Continue;
  }
  else
  {
    // not visible or hierarchy bounding box is not intersected, prune traversal
    action = TraversalActions::Prune;
  }

  m_PickVisitor->State().m_Matrix = matrix;

  return action;
}