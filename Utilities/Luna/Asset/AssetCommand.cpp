#include "Precompile.h"
#include "AssetCommand.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AssetCommand::AssetCommand()
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AssetCommand::~AssetCommand()
{
}

///////////////////////////////////////////////////////////////////////////////
// Sets the undoable command that this object wraps.
// 
void AssetCommand::SetCommand( const Undo::CommandPtr& command )
{
  m_Command = command;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the undoable command that this object wraps.
// 
Undo::CommandPtr AssetCommand::GetCommand() const
{
  return m_Command;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the list of assets that are effected by this command.
// 
void AssetCommand::SetAssets( const S_AssetClassDumbPtr& assets )
{
  m_Assets = assets;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of assets that are effected by this command.
// 
const S_AssetClassDumbPtr& AssetCommand::GetAssets() const
{
  return m_Assets;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified asset is in the list of assets effected by
// this command.
// 
bool AssetCommand::IsAssetEffected( Luna::AssetClass* asset )
{
  return ( m_Assets.find( asset ) != m_Assets.end() );
}
