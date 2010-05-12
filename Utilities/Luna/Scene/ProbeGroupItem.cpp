#include "Precompile.h"
#include "ProbeGroupItem.h"

#include "CubeMapProbe.h"
#include "LightingJob.h"
#include "LightingJobCommand.h"
#include "ProbeItem.h"
#include "Scene.h"

#include "Core/Object.h"


// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ProbeGroupItem::ProbeGroupItem( wxTreeCtrlBase* tree, Luna::LightingJob* job )
: GroupItem( tree, job, TypeProbeGroup )
{
  job->AddProbeAddedListener( LightingJobMemberChangeSignature::Delegate ( this, &ProbeGroupItem::ProbeAdded ) );
  job->AddProbeRemovedListener( LightingJobMemberChangeSignature::Delegate ( this, &ProbeGroupItem::ProbeRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ProbeGroupItem::~ProbeGroupItem()
{
  GetLightingJob()->RemoveProbeAddedListener( LightingJobMemberChangeSignature::Delegate ( this, &ProbeGroupItem::ProbeAdded ) );
  GetLightingJob()->RemoveProbeRemovedListener( LightingJobMemberChangeSignature::Delegate ( this, &ProbeGroupItem::ProbeRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the label to use for this tree item.
// 
const std::string& ProbeGroupItem::GetProbeGroupLabel()
{
  static const std::string label( "Cube Map Probes" );
  return label;
}

///////////////////////////////////////////////////////////////////////////////
// Non-static version of above function.
// 
const std::string& ProbeGroupItem::GetLabel() const
{
  return GetProbeGroupLabel();
}

///////////////////////////////////////////////////////////////////////////////
// Call after creating an item to set up its initial state.
// 
void ProbeGroupItem::Load()
{
  __super::Load();

  GetTree()->Freeze();
  S_tuid zoneIDs;
  GetLightingJob()->GetZoneIDs( zoneIDs );
  S_tuid::const_iterator zoneItr = zoneIDs.begin();
  S_tuid::const_iterator zoneEnd = zoneIDs.end();
  for ( ; zoneItr != zoneEnd; ++zoneItr )
  {
    LoadZoneData( &LightingJob::GetCubeMapProbes, *zoneItr );
  }
  GetTree()->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this item is able to be selected, based on what is in the
// current selection list.
// 
bool ProbeGroupItem::IsSelectable( const OS_TreeItemIds& currentSelection ) const
{
  bool isSelectable = false;

  if ( currentSelection.Size() == 0 )
  {
    isSelectable = true;
  }
  else
  {
    LightingTreeItem* firstSelected = static_cast< LightingTreeItem* >( GetTree()->GetItemData( *currentSelection.Begin() ) );
    isSelectable = ( firstSelected->GetType() == TypeProbeGroup );
  }

  return isSelectable;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the object can be added to this group.
// 
bool ProbeGroupItem::CanAddChild( Object* object ) const
{
  return ( object->HasType( Reflect::GetType<Luna::CubeMapProbe>() ) );
}

///////////////////////////////////////////////////////////////////////////////
// Creates a new command to add the specified object to this group.  The returned
// command will not reference anything if the object could not be added.
// 
Undo::CommandPtr ProbeGroupItem::AddChild( Object* object )
{
  Undo::Command* command = NULL;
  if ( CanAddChild( object ) )
  {
    Luna::CubeMapProbe* probe = Reflect::AssertCast< Luna::CubeMapProbe >( object );
    if ( probe->GetScene()->GetFileID() != TUID::Null )
    {
      command = new LightingJobCommand( LightingJobCommandActions::Add, LightingJobCommandGroups::Probe, GetLightingJob(), probe, true );
    }
    else
    {
      // Notify the user that the command could not be completed
      std::ostringstream msg;
      msg << "Zone '" << probe->GetScene()->GetFileName() << "'is not in the file resolver yet. Unable to add cube map probe '" << probe->GetName() << "' to batch '" << GetLightingJob()->GetName() << "'.";
      wxMessageBox( msg.str().c_str(), "Error", wxOK | wxCENTER | wxICON_ERROR, GetTree() );
    }
  }
  return command;
}

///////////////////////////////////////////////////////////////////////////////
// Required by base class to actually add a child item to the tree.
// 
void ProbeGroupItem::AddChildNode( Luna::SceneNode* instance )
{
  if ( instance->HasType( Reflect::GetType<Luna::CubeMapProbe>() ) )
  {
    AddProbe( Reflect::AssertCast< Luna::CubeMapProbe >( instance ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Required by base class to remove the child tree item.
// 
void ProbeGroupItem::RemoveChildNode( Luna::SceneNode* instance )
{
  if ( instance->HasType( Reflect::GetType<Luna::CubeMapProbe>() ) )
  {
    RemoveProbe( Reflect::AssertCast< Luna::CubeMapProbe >( instance ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Adds a tree item (as a child of this group) wrapping the specified cube map probe.
// 
void ProbeGroupItem::AddProbe( Luna::CubeMapProbe* probe )
{
  GetTree()->Freeze();
  ProbeItem* itemData = new ProbeItem( GetTree(), GetLightingJob(), probe );
  wxTreeItemId childItem = GetTree()->AppendItem( GetId(), itemData->GetLabel().c_str(), -1, -1, itemData );
  GetTree()->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Searches child items of this group, looking for the one that is linked to
// the specified probe.  Removes that child item if found.
// 
void ProbeGroupItem::RemoveProbe( Luna::CubeMapProbe* probe )
{
  wxTreeItemIdValue cookie;
  wxTreeCtrlBase* tree = GetTree();
  const wxTreeItemId id = GetId();
  wxTreeItemId child = tree->GetFirstChild( id, cookie );
  while ( child.IsOk() )
  {
    ProbeItem* data = Cast< ProbeItem >( tree->GetItemData( child ), TypeProbe );
    if ( data )
    {
      if ( data->GetProbe() == probe )
      {
        tree->Delete( child );
        break;
      }
    }
    child = tree->GetNextChild( id, cookie );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a cube map probe was added to this lighting job.  Updates
// the UI by adding the appropriate tree item as a child of this one.
// 
void ProbeGroupItem::ProbeAdded( const LightingJobMemberChangeArgs& args )
{
  AddProbe( Reflect::AssertCast< Luna::CubeMapProbe >( args.m_Member ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a cube map probe is removed from this lighting job.  Updates
// the tree UI.
// 
void ProbeGroupItem::ProbeRemoved( const LightingJobMemberChangeArgs& args )
{
  RemoveProbe( Reflect::AssertCast< Luna::CubeMapProbe >( args.m_Member ) );
}
