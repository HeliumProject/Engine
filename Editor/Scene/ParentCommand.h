#pragma once

#include "Application/Undo/BatchCommand.h"

namespace Editor
{
  class HierarchyNode;
  typedef Helium::SmartPtr< Editor::HierarchyNode > HierarchyNodePtr;

  class ParentCommand : public Undo::BatchCommand
  {
  private:
    HierarchyNodePtr m_Node;
    HierarchyNodePtr m_NextParent;
    HierarchyNodePtr m_PreviousParent;

  public:
    ParentCommand(const HierarchyNodePtr& child, const HierarchyNodePtr& parent);

    virtual void Undo() HELIUM_OVERRIDE;

    virtual void Redo() HELIUM_OVERRIDE;

    void Swap();
  };
}
