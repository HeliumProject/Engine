#include "Precompile.h"
#include "ZoneGroupItem.h"

#include "Zone.h"
#include "ZoneItem.h"
#include "LightingJob.h"
#include "LightingJobCommand.h"
#include "Scene.h"
#include "SceneManager.h"

#include "File/Manager.h"
#include "Console/Console.h"
#include "Core/Object.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ZoneGroupItem::ZoneGroupItem( wxTreeCtrlBase* tree, Luna::LightingJob* job )
: GroupItem( tree, job, TypeZoneGroup )
{
  job->AddZoneAddedListener( LightingJobMemberChangeSignature::Delegate ( this, &ZoneGroupItem::ZoneAdded ) );
  job->AddZoneRemovedListener( LightingJobMemberChangeSignature::Delegate ( this, &ZoneGroupItem::ZoneRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ZoneGroupItem::~ZoneGroupItem()
{
  Luna::LightingJob* job = GetLightingJob();
  job->RemoveZoneAddedListener( LightingJobMemberChangeSignature::Delegate ( this, &ZoneGroupItem::ZoneAdded ) );
  job->RemoveZoneRemovedListener( LightingJobMemberChangeSignature::Delegate ( this, &ZoneGroupItem::ZoneRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the label for a zone group.
// 
const std::string& ZoneGroupItem::GetZoneGroupLabel()
{
  static const std::string label( "Excluded Zones" );
  return label;
}

///////////////////////////////////////////////////////////////////////////////
// Non-static version of the above function.
// 
const std::string& ZoneGroupItem::GetLabel() const
{
  return GetZoneGroupLabel();
}

///////////////////////////////////////////////////////////////////////////////
// Call after creating an instance of this class to set up child items.
// 
void ZoneGroupItem::Load()
{
  __super::Load();

  GetTree()->Freeze();
   
  LoadZoneData( &LightingJob::GetZones,  GetLightingJob()->GetScene()->GetManager()->GetRootScene()->GetFileID() );

  GetTree()->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the selection contains other zone groups.
// 
bool ZoneGroupItem::IsSelectable( const OS_TreeItemIds& currentSelection ) const
{
  bool isSelectable = false;

  if ( currentSelection.Size() == 0 )
  {
    isSelectable = true;
  }
  else
  {
    LightingTreeItem* firstSelected = static_cast< LightingTreeItem* >( GetTree()->GetItemData( *currentSelection.Begin() ) );
    isSelectable = ( firstSelected->GetType() == TypeZoneGroup );
  }

  return isSelectable;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified object is a zone.  Only lights can be added
// to a zone group.
// 
bool ZoneGroupItem::CanAddChild( Object* object ) const
{
  bool canAdd = object->HasType( Reflect::GetType<Zone>() );
  return canAdd;
}

///////////////////////////////////////////////////////////////////////////////
// Adds a child tree item under this one to represent the object, if it can be
// added as a child item.
// 
Undo::CommandPtr ZoneGroupItem::AddChild( Object* object )
{
  Undo::Command* command = NULL;

  if ( CanAddChild( object ) )
  {
    Zone* zone = Reflect::AssertCast< Zone >( object );

    tuid zoneID = zone->GetScene()->GetFileID();
    if ( zoneID != TUID::Null )
    {
      command = new LightingJobCommand( LightingJobCommandActions::Add, LightingJobCommandGroups::Zone, GetLightingJob(), zone, true );
    }
    else
    {
      // Notify the user that the command could not be completed
      std::ostringstream msg;
      msg << "Zone '" << zone->GetScene()->GetFileName() << "'is not in the file resolver yet. Unable to add zone '" << zone->GetName() << "' to batch '" << GetLightingJob()->GetName() << "'.";
      wxMessageBox( msg.str().c_str(), "Error", wxOK | wxCENTER | wxICON_ERROR, GetTree() );
    }
  }

  return command;
}

///////////////////////////////////////////////////////////////////////////////
// Required by base class for adding a child tree item.  If the instance is 
// really a zone, it will be added.
// 
void ZoneGroupItem::AddChildNode( Luna::SceneNode* instance )
{
  if ( instance->HasType( Reflect::GetType<Zone>() ) )
  {
    AddZone( Reflect::AssertCast< Zone >( instance ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Required by base class for removing a child tree item that corresponds to the
// specified instance.
// 
void ZoneGroupItem::RemoveChildNode( Luna::SceneNode* instance )
{
  if ( instance->HasType( Reflect::GetType<Zone>() ) )
  {
    RemoveZone( Reflect::AssertCast< Zone >( instance ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified zone as a child item of this group.
// 
void ZoneGroupItem::AddZone( Zone* zone )
{
  GetTree()->Freeze();
  ZoneItem* itemData = new ZoneItem( GetTree(), GetLightingJob(), zone );
  wxTreeItemId childItem = GetTree()->AppendItem( GetId(), itemData->GetLabel().c_str(), -1, -1, itemData );
  GetTree()->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Removes the child tree item that represents the specifed zone, if any.
// 
void ZoneGroupItem::RemoveZone( Zone* zone )
{
  wxTreeItemIdValue cookie;
  wxTreeCtrlBase* tree = GetTree();
  const wxTreeItemId id = GetId();
  wxTreeItemId child = tree->GetFirstChild( id, cookie );
  while ( child.IsOk() )
  {
    ZoneItem* data = Cast< ZoneItem >( tree->GetItemData( child ), TypeZone );
    if ( data )
    {
      if ( data->GetZone() == zone )
      {
        tree->Delete( child );
        break;
      }
    }
    child = tree->GetNextChild( id, cookie );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a zone is added to a lighting job.  Updates the UI.
// 
void ZoneGroupItem::ZoneAdded( const LightingJobMemberChangeArgs& args )
{
  Zone* zone = Reflect::AssertCast< Zone >( args.m_Member );
  if ( zone )
  {
    AddZone( zone );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a zone is removed from a lighting job.  Updates the UI.
// 
void ZoneGroupItem::ZoneRemoved( const LightingJobMemberChangeArgs& args )
{
  Zone* zone = Reflect::AssertCast< Zone >( args.m_Member );
  if ( zone )
  {
    RemoveZone( zone );
  }
}
