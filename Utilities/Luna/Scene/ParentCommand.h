#pragma once

#include "Application/Undo/BatchCommand.h"

namespace Luna
{
  class HierarchyNode;
  typedef Nocturnal::SmartPtr< Luna::HierarchyNode > HierarchyNodePtr;

  class ParentCommand : public Undo::BatchCommand
  {
  private:
    HierarchyNodePtr m_Node;
    HierarchyNodePtr m_NextParent;
    HierarchyNodePtr m_PreviousParent;

  public:
    ParentCommand(const HierarchyNodePtr& child, const HierarchyNodePtr& parent);

    virtual void Undo() NOC_OVERRIDE;

    virtual void Redo() NOC_OVERRIDE;

    void Swap();
  };
}
