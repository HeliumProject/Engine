#include "Precompile.h"
#include "ZoneUnloadedItem.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Console/Console.h"
#include "AppUtils/AppUtils.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ZoneUnloadedItem::ZoneUnloadedItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, const tuid& fileID )
: LeafItem( tree, job, TypeUnloadedZone )
, m_FileID( fileID )
{
  m_Label = "Unloaded Zone: Unknown";

  std::string path;

  try
  {
    path = File::GlobalManager().GetPath( fileID );
  }
  catch ( const File::Exception& )
  {
    // Do nothing... just use the default name
  }

  m_Label = "Unloaded Zone: " + (path.empty() ? "Unknown" : FileSystem::GetLeaf( path ));
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ZoneUnloadedItem::~ZoneUnloadedItem()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the label to use in the tree control for this item.
// 
const std::string& ZoneUnloadedItem::GetLabel() const
{
  return m_Label;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the TUID of the zone that this tree item represents.
// 
tuid ZoneUnloadedItem::GetFileID() const
{
  return m_FileID;
}

///////////////////////////////////////////////////////////////////////////////
// Currently returns NULL; you can't remove items that haven't been loaded.
// 
Undo::CommandPtr ZoneUnloadedItem::GetRemoveCommand( const OS_SelectableDumbPtr& selection )
{
#pragma TODO("Remove whole zones of items, even if they aren't loaded?")
  return NULL;
}
