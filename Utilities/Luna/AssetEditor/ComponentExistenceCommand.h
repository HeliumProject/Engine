#pragma once

#include "Application/Undo/BatchCommand.h"
#include "Application/Undo/ExistenceCommand.h"

namespace Luna
{
  // Forwards
  class AssetClass;
  class ComponentWrapper;
  typedef Nocturnal::SmartPtr< Luna::ComponentWrapper > ComponentWrapperPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Command for adding and removing an Luna::ComponentWrapper from an Luna::AssetClass.  Also
  // handles all the changes that might be necessary to the Luna::AssetNode hierarchy.
  // 
  class ComponentExistenceCommand : public Undo::ExistenceCommand
  {
  private:
    Undo::ExistenceAction m_Action;
    Luna::AssetClass* m_Asset;
    Luna::ComponentWrapperPtr m_Component;
    Undo::BatchCommandPtr m_References;
    
  public:
    ComponentExistenceCommand( Undo::ExistenceAction action, Luna::AssetClass* asset, Luna::ComponentWrapper* attribute );
    virtual void Undo() NOC_OVERRIDE;
    virtual void Redo() NOC_OVERRIDE;
  };
}
