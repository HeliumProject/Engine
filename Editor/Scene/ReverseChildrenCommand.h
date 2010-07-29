#pragma once

#include "HierarchyNode.h"
#include "Application/Undo/Command.h"

namespace Editor
{
  class ReverseChildrenCommand : public Undo::Command
  {
  private:
    HierarchyNodePtr m_Node;

  public:
    ReverseChildrenCommand( const HierarchyNodePtr& node );
    virtual ~ReverseChildrenCommand();
    virtual void Undo() HELIUM_OVERRIDE;
    virtual void Redo() HELIUM_OVERRIDE;
  };
}
