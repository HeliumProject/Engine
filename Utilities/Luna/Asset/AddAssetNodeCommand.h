#pragma once

#include "Application/Undo/ExistenceCommand.h"

namespace Luna
{
  // Forwards
  class AssetNode;
  typedef Nocturnal::SmartPtr< Luna::AssetNode > AssetNodePtr;

  /////////////////////////////////////////////////////////////////////////////
  // Command for adding an node to the hierarchy.
  // 
  class AddAssetNodeCommand : public Undo::Command
  {
  private:
    Luna::AssetNode* m_Parent;
    Luna::AssetNodePtr m_Child;
    Luna::AssetNode* m_BeforeSibling;

  public:
    AddAssetNodeCommand( Luna::AssetNode* parent, Luna::AssetNode* child, Luna::AssetNode* beforeSibling = NULL, bool redo = true );
    virtual void Undo() NOC_OVERRIDE;
    virtual void Redo() NOC_OVERRIDE;
  };
}
