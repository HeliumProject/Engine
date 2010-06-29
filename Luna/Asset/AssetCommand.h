#pragma once

#include "AssetClass.h"
#include "Application/Undo/Command.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Class for managing an undo command and the list of assets effected by that
  // command.
  // 
  class AssetCommand : public Nocturnal::RefCountBase<Luna::AssetCommand>
  {
  protected:
    Undo::CommandPtr m_Command;
    S_AssetClassDumbPtr m_Assets;

  public:
    AssetCommand();
    virtual ~AssetCommand();

    void SetCommand( const Undo::CommandPtr& command );
    Undo::CommandPtr GetCommand() const;

    void SetAssets( const S_AssetClassDumbPtr& assets );
    const S_AssetClassDumbPtr& GetAssets() const;
    bool IsAssetEffected( Luna::AssetClass* asset );
  };

  typedef Nocturnal::SmartPtr< Luna::AssetCommand > AssetCommandPtr;
}
