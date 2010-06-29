#pragma once

#include "HierarchyNode.h"
#include "Application/Undo/Command.h"

namespace Luna
{
  class ReverseChildrenCommand : public Undo::Command
  {
  private:
    HierarchyNodePtr m_Node;

  public:
    ReverseChildrenCommand( const HierarchyNodePtr& node );
    virtual ~ReverseChildrenCommand();
    virtual void Undo() NOC_OVERRIDE;
    virtual void Redo() NOC_OVERRIDE;
  };
}
