#include "Precompile.h"
#include "LightGroupItem.h"

#include "Light.h"
#include "LightItem.h"
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
LightGroupItem::LightGroupItem( wxTreeCtrlBase* tree, Luna::LightingJob* job )
: GroupItem( tree, job, TypeLightGroup )
{
  job->AddLightAddedListener( LightingJobMemberChangeSignature::Delegate ( this, &LightGroupItem::LightAdded ) );
  job->AddLightRemovedListener( LightingJobMemberChangeSignature::Delegate ( this, &LightGroupItem::LightRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LightGroupItem::~LightGroupItem()
{
  Luna::LightingJob* job = GetLightingJob();
  job->RemoveLightAddedListener( LightingJobMemberChangeSignature::Delegate ( this, &LightGroupItem::LightAdded ) );
  job->RemoveLightRemovedListener( LightingJobMemberChangeSignature::Delegate ( this, &LightGroupItem::LightRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the label for a light group.
// 
const std::string& LightGroupItem::GetLightGroupLabel()
{
  static const std::string label( "Lights" );
  return label;
}

///////////////////////////////////////////////////////////////////////////////
// Non-static version of the above function.
// 
const std::string& LightGroupItem::GetLabel() const
{
  return GetLightGroupLabel();
}

///////////////////////////////////////////////////////////////////////////////
// Call after creating an instance of this class to set up child items.
// 
void LightGroupItem::Load()
{
  __super::Load();

  GetTree()->Freeze();
  S_tuid zoneIDs;
  GetLightingJob()->GetZoneIDs( zoneIDs );
  S_tuid::const_iterator zoneItr = zoneIDs.begin();
  S_tuid::const_iterator zoneEnd = zoneIDs.end();
  for ( ; zoneItr != zoneEnd; ++zoneItr )
  {
    LoadZoneData( &LightingJob::GetLights, *zoneItr );
  }
  GetTree()->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the selection contains other light groups.
// 
bool LightGroupItem::IsSelectable( const OS_TreeItemIds& currentSelection ) const
{
  bool isSelectable = false;

  if ( currentSelection.Size() == 0 )
  {
    isSelectable = true;
  }
  else
  {
    LightingTreeItem* firstSelected = static_cast< LightingTreeItem* >( GetTree()->GetItemData( *currentSelection.Begin() ) );
    isSelectable = ( firstSelected->GetType() == TypeLightGroup );
  }

  return isSelectable;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified object is a light.  Only lights can be added
// to a light group.
// 
bool LightGroupItem::CanAddChild( Object* object ) const
{
  bool canAdd = object->HasType( Reflect::GetType<Luna::Light>() );
  return canAdd;
}

///////////////////////////////////////////////////////////////////////////////
// Adds a child tree item under this one to represent the object, if it can be
// added as a child item.
// 
Undo::CommandPtr LightGroupItem::AddChild( Object* object )
{
  Undo::Command* command = NULL;

  if ( CanAddChild( object ) )
  {
    Luna::Light* light = Reflect::AssertCast< Luna::Light >( object );

    tuid zoneID = light->GetScene()->GetFileID();
    if ( zoneID != TUID::Null )
    {
      command = new LightingJobCommand( LightingJobCommandActions::Add, LightingJobCommandGroups::Light, GetLightingJob(), light, true );
    }
    else
    {
      // Notify the user that the command could not be completed
      std::ostringstream msg;
      msg << "Zone '" << light->GetScene()->GetFileName() << "'is not in the file resolver yet. Unable to add light '" << light->GetName() << "' to batch '" << GetLightingJob()->GetName() << "'.";
      wxMessageBox( msg.str().c_str(), "Error", wxOK | wxCENTER | wxICON_ERROR, GetTree() );
    }
  }

  return command;
}

///////////////////////////////////////////////////////////////////////////////
// Required by base class for adding a child tree item.  If the instance is 
// really a light, it will be added.
// 
void LightGroupItem::AddChildNode( Luna::SceneNode* instance )
{
  if ( instance->HasType( Reflect::GetType<Luna::Light>() ) )
  {
    AddLight( Reflect::AssertCast< Luna::Light >( instance ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Required by base class for removing a child tree item that corresponds to the
// specified instance.
// 
void LightGroupItem::RemoveChildNode( Luna::SceneNode* instance )
{
  if ( instance->HasType( Reflect::GetType<Luna::Light>() ) )
  {
    RemoveLight( Reflect::AssertCast< Luna::Light >( instance ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified light as a child item of this group.
// 
void LightGroupItem::AddLight( Luna::Light* light )
{
  GetTree()->Freeze();
  LightItem* itemData = new LightItem( GetTree(), GetLightingJob(), light );
  wxTreeItemId childItem = GetTree()->AppendItem( GetId(), itemData->GetLabel().c_str(), -1, -1, itemData );
  GetTree()->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Removes the child tree item that represents the specifed light, if any.
// 
void LightGroupItem::RemoveLight( Luna::Light* light )
{
  wxTreeItemIdValue cookie;
  wxTreeCtrlBase* tree = GetTree();
  const wxTreeItemId id = GetId();
  wxTreeItemId child = tree->GetFirstChild( id, cookie );
  while ( child.IsOk() )
  {
    LightItem* data = Cast< LightItem >( tree->GetItemData( child ), TypeLight );
    if ( data )
    {
      if ( data->GetLight() == light )
      {
        tree->Delete( child );
        break;
      }
    }
    child = tree->GetNextChild( id, cookie );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a light is added to a lighting job.  Updates the UI.
// 
void LightGroupItem::LightAdded( const LightingJobMemberChangeArgs& args )
{
  Luna::Light* light = Reflect::AssertCast< Luna::Light >( args.m_Member );
  if ( light )
  {
    AddLight( light );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a light is removed from a lighting job.  Updates the UI.
// 
void LightGroupItem::LightRemoved( const LightingJobMemberChangeArgs& args )
{
  Luna::Light* light = Reflect::AssertCast< Luna::Light >( args.m_Member );
  if ( light )
  {
    RemoveLight( light );
  }
}
