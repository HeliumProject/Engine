#pragma once

#include "Luna/API.h"
#include "Core/Selectable.h"

namespace Luna
{
  class Scene;
  class HierarchyNode;

  class PickVisitor;
  class RenderVisitor;


  //
  // What action to take after the traverser visits a node
  //

  namespace TraversalActions
  {
    enum TraversalAction
    {
      Continue,   // keep traversing
      Prune,      // do not visit my children
      Abort,      // abort and unwind traversal
    };
  }

  typedef TraversalActions::TraversalAction TraversalAction;


  //
  // Hierarchy traverser walks the hierarchy of object in the scene
  //

  class HierarchyTraverser
  {
  public:
    virtual TraversalAction VisitHierarchyNode(Luna::HierarchyNode* node) = 0;
  };


  //
  // Hierarchy traverser walks the hierarchy of object in the scene
  //

  class HierarchyChildTraverser : public HierarchyTraverser
  {
  public:
    OS_SelectableDumbPtr m_Children;

    virtual TraversalAction VisitHierarchyNode(Luna::HierarchyNode* node) NOC_OVERRIDE;
  };


  //
  // Render the scene
  //

  class HierarchyRenderTraverser : public HierarchyTraverser
  {
  private:
    RenderVisitor* m_RenderVisitor;

  public:
    HierarchyRenderTraverser(RenderVisitor* renderVisitor);

    virtual TraversalAction VisitHierarchyNode(Luna::HierarchyNode* node) NOC_OVERRIDE;
  };


  //
  // Pick items in the scene
  //

  class HierarchyPickTraverser : public HierarchyTraverser
  {
  private:
    PickVisitor* m_PickVisitor;

  public:
    HierarchyPickTraverser(PickVisitor* pickVisitor);

    virtual TraversalAction VisitHierarchyNode(Luna::HierarchyNode* node) NOC_OVERRIDE;
  };
}