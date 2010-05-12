#pragma once

#include "Undo/ExistenceCommand.h"

namespace Luna
{
  // Forwards
  class AssetNode;
  typedef Nocturnal::SmartPtr< Luna::AssetNode > AssetNodePtr;

  /////////////////////////////////////////////////////////////////////////////
  // Command for removing a node from the hierarchy.
  // 
  class RemoveAssetNodeCommand : public Undo::Command
  {
  private:
    Luna::AssetNode* m_Parent;
    Luna::AssetNodePtr m_Child;
    Luna::AssetNode* m_BeforeSibling;

  public:
    RemoveAssetNodeCommand( Luna::AssetNode* child, bool redo = true );
    virtual void Undo() NOC_OVERRIDE;
    virtual void Redo() NOC_OVERRIDE;
  };
}
