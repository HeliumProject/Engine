#include "Precompile.h"
#include "GroupItem.h"
#include "LightingJob.h"
#include "LightingJobCommand.h"
#include "Scene.h"
#include "SceneManager.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
GroupItem::GroupItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, ItemType type )
: LightingTreeItem( tree, job, type )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
GroupItem::~GroupItem()
{
}

///////////////////////////////////////////////////////////////////////////////
// Uses a pointer to member function to fetch the list of items that belong in 
// this group.  Adds all the appropriate child tree items to this group.
// 
void GroupItem::LoadZoneData( const UniqueID::S_TUID& ( LightingJob::* getter )( const tuid& ) const, tuid zoneID )
{
  Luna::Scene* scene = GetLightingJob()->GetScene()->GetManager()->GetScene( zoneID );
  const UniqueID::S_TUID& instanceIDs = ( GetLightingJob()->*getter )( zoneID );

  if ( scene && instanceIDs.size() > 0 )
  {
    GetTree()->Freeze();
    UniqueID::S_TUID::const_iterator instItr = instanceIDs.begin();
    UniqueID::S_TUID::const_iterator instEnd = instanceIDs.end();
    for ( ; instItr != instEnd; ++instItr )
    {
      Luna::SceneNode* node = scene->FindNode( *instItr );
      if ( node )
      {
        Luna::SceneNode* instance = Reflect::ObjectCast< Luna::SceneNode >( node );
        if ( instance )
        {
          AddChildNode( instance );
        }
      }
    }
    GetTree()->Thaw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Uses a pointer to member function to fetch all the items that belong to this
// group.  Removes all the child items that belong to the specified zone.
// 
void GroupItem::UnloadZoneData( const UniqueID::S_TUID& ( LightingJob::* getter )( const tuid& ) const, tuid zoneID )
{
  Luna::Scene* scene = GetLightingJob()->GetScene()->GetManager()->GetScene( zoneID );
  NOC_ASSERT( scene ); // If this assert causes problems, it can probably be removed.
  const UniqueID::S_TUID& instanceIDs = ( GetLightingJob()->*getter )( zoneID );

  if ( scene )
  {
    GetTree()->Freeze();
    UniqueID::S_TUID::const_iterator instItr = instanceIDs.begin();
    UniqueID::S_TUID::const_iterator instEnd = instanceIDs.end();
    for ( ; instItr != instEnd; ++instItr )
    {
      Luna::SceneNode* node = scene->FindNode( *instItr );
      if ( node )
      {
        Luna::Instance* instance = Reflect::ObjectCast< Luna::Instance >( node );
        if ( instance )
        {
          RemoveChildNode( instance );
        }
      }
    }
    GetTree()->Thaw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Call after creating an item to set up its initial state.
// 
void GroupItem::Load()
{
  GetTree()->SetItemBold( GetId() );
}

///////////////////////////////////////////////////////////////////////////////
// Group items cannot be renamed.
// 
bool GroupItem::CanRename() const
{
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Returns a batch command that removes all the sub items of this group from
// the lighting job.
// 
Undo::CommandPtr GroupItem::GetRemoveCommand( const OS_SelectableDumbPtr& selection )
{
  Undo::BatchCommandPtr batch = new Undo::BatchCommand();

  wxTreeCtrlBase* tree = GetTree();
  const wxTreeItemId id = GetId();
  wxTreeItemIdValue cookie;
  wxTreeItemId child = tree->GetFirstChild( id, cookie );
  while ( child.IsOk() )
  {
    LightingTreeItem* childData = static_cast< LightingTreeItem* >( tree->GetItemData( child ) );
    if ( childData )
    {
      batch->Push( childData->GetRemoveCommand( selection ) );
    }
    child = tree->GetNextChild( id, cookie );
  }

  // If the batch is empty, return a NULL command
  if ( batch->IsEmpty() )
  {
    batch = NULL;
  }

  return batch;
}

void GroupItem::RemoveAllChildren()
{
  wxTreeItemIdValue cookie;
  wxTreeCtrlBase* tree = GetTree();
  const wxTreeItemId id = GetId();
  wxTreeItemId child = tree->GetFirstChild( id, cookie );
  while ( child.IsOk() )
  {
    tree->Delete( child );
    child = tree->GetNextChild( id, cookie );
  }
}

void GroupItem::RemoveAll( const ContextMenuArgsPtr& args )
{

}