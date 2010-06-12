#pragma once

#include "Application/Undo/BatchCommand.h"
#include "Application/Undo/ExistenceCommand.h"

namespace Luna
{
  // Forwards
  class AssetClass;
  class AttributeWrapper;
  typedef Nocturnal::SmartPtr< Luna::AttributeWrapper > AttributeWrapperPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Command for adding and removing an Luna::AttributeWrapper from an Luna::AssetClass.  Also
  // handles all the changes that might be necessary to the Luna::AssetNode hierarchy.
  // 
  class AttributeExistenceCommand : public Undo::ExistenceCommand
  {
  private:
    Undo::ExistenceAction m_Action;
    Luna::AssetClass* m_Asset;
    Luna::AttributeWrapperPtr m_Attribute;
    Undo::BatchCommandPtr m_References;
    
  public:
    AttributeExistenceCommand( Undo::ExistenceAction action, Luna::AssetClass* asset, Luna::AttributeWrapper* attribute );
    virtual void Undo() NOC_OVERRIDE;
    virtual void Redo() NOC_OVERRIDE;
  };
}
