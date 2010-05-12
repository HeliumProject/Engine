#include "Precompile.h"
#include "LightingEnvironmentGroupItem.h"

#include "LightingEnvironment.h"
#include "LightingEnvironmentItem.h"
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
LightingEnvironmentGroupItem::LightingEnvironmentGroupItem( wxTreeCtrlBase* tree, Luna::LightingJob* job )
: GroupItem( tree, job, TypeLightGroup )
{
  job->AddLightingEnvironmentAddedListener( LightingJobMemberChangeSignature::Delegate ( this, &LightingEnvironmentGroupItem::LightingEnvironmentAdded ) );
  job->AddLightingEnvironmentRemovedListener( LightingJobMemberChangeSignature::Delegate ( this, &LightingEnvironmentGroupItem::LightingEnvironmentRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LightingEnvironmentGroupItem::~LightingEnvironmentGroupItem()
{
  Luna::LightingJob* job = GetLightingJob();
  job->RemoveLightingEnvironmentAddedListener( LightingJobMemberChangeSignature::Delegate ( this, &LightingEnvironmentGroupItem::LightingEnvironmentAdded ) );
  job->RemoveLightingEnvironmentRemovedListener( LightingJobMemberChangeSignature::Delegate ( this, &LightingEnvironmentGroupItem::LightingEnvironmentRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the label for a lighting environment group.
// 
const std::string& LightingEnvironmentGroupItem::GetLightingEnvironmentGroupLabel()
{
  static const std::string label( "Lighting Environment" );
  return label;
}

///////////////////////////////////////////////////////////////////////////////
// Non-static version of the above function.
// 
const std::string& LightingEnvironmentGroupItem::GetLabel() const
{
  return GetLightingEnvironmentGroupLabel();
}

///////////////////////////////////////////////////////////////////////////////
// Call after creating an instance of this class to set up child items.
// 
void LightingEnvironmentGroupItem::Load()
{
  __super::Load();

  GetTree()->Freeze();
  S_tuid zoneIDs;
  GetLightingJob()->GetZoneIDs( zoneIDs );
  S_tuid::const_iterator zoneItr = zoneIDs.begin();
  S_tuid::const_iterator zoneEnd = zoneIDs.end();
  for ( ; zoneItr != zoneEnd; ++zoneItr )
  {
    LoadZoneData( &LightingJob::GetLightingEnvironments, *zoneItr );
  }
  GetTree()->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the selection contains other lighting environment groups.
// 
bool LightingEnvironmentGroupItem::IsSelectable( const OS_TreeItemIds& currentSelection ) const
{
  bool isSelectable = false;

  if ( currentSelection.Size() == 0 )
  {
    isSelectable = true;
  }
  else
  {
    LightingTreeItem* firstSelected = static_cast< LightingTreeItem* >( GetTree()->GetItemData( *currentSelection.Begin() ) );
    isSelectable = ( firstSelected->GetType() == TypeLightingEnvironmentGroup );
  }

  return isSelectable;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified object is a lighting environment.  Only lights can be added
// to a lighting environment group.
// 
bool LightingEnvironmentGroupItem::CanAddChild( Object* object ) const
{
  bool canAdd = object->HasType( Reflect::GetType<Luna::LightingEnvironment>() );
  return canAdd;
}

///////////////////////////////////////////////////////////////////////////////
// Adds a child tree item under this one to represent the object, if it can be
// added as a child item.
// 
Undo::CommandPtr LightingEnvironmentGroupItem::AddChild( Object* object )
{
  Undo::Command* command = NULL;

  if ( CanAddChild( object ) )
  {
    Luna::LightingEnvironment* env = Reflect::AssertCast< Luna::LightingEnvironment >( object );

    tuid zoneID = env->GetScene()->GetFileID();
    if ( zoneID != TUID::Null )
    {
      command = new LightingJobCommand( LightingJobCommandActions::Add, LightingJobCommandGroups::LightingEnvironment, GetLightingJob(), env, true );
    }
    else
    {
      // Notify the user that the command could not be completed
      std::ostringstream msg;
      msg << "Zone '" << env->GetScene()->GetFileName() << "'is not in the file resolver yet. Unable to add lighting environment '" << env->GetName() << "' to batch '" << GetLightingJob()->GetName() << "'.";
      wxMessageBox( msg.str().c_str(), "Error", wxOK | wxCENTER | wxICON_ERROR, GetTree() );
    }
  }

  return command;
}

///////////////////////////////////////////////////////////////////////////////
// Required by base class for adding a child tree item.  If the instance is 
// really a lighting environment, it will be added.
// 
void LightingEnvironmentGroupItem::AddChildNode( Luna::SceneNode* instance )
{
  if ( instance->HasType( Reflect::GetType<Luna::LightingEnvironment>() ) )
  {
    AddLightingEnvironment( Reflect::AssertCast< Luna::LightingEnvironment >( instance ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Required by base class for removing a child tree item that corresponds to the
// specified instance.
// 
void LightingEnvironmentGroupItem::RemoveChildNode( Luna::SceneNode* instance )
{
  if ( instance->HasType( Reflect::GetType<Luna::LightingEnvironment>() ) )
  {
    RemoveLightingEnvironment( Reflect::AssertCast< Luna::LightingEnvironment >( instance ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified lighting environment as a child item of this group.
// 
void LightingEnvironmentGroupItem::AddLightingEnvironment( Luna::LightingEnvironment* env )
{
  GetTree()->Freeze();
  LightingEnvironmentItem* itemData = new LightingEnvironmentItem( GetTree(), GetLightingJob(), env );
  wxTreeItemId childItem = GetTree()->AppendItem( GetId(), itemData->GetLabel().c_str(), -1, -1, itemData );
  GetTree()->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Removes the child tree item that represents the specifed lighting environment, if any.
// 
void LightingEnvironmentGroupItem::RemoveLightingEnvironment( Luna::LightingEnvironment* env )
{
  wxTreeItemIdValue cookie;
  wxTreeCtrlBase* tree = GetTree();
  const wxTreeItemId id = GetId();
  wxTreeItemId child = tree->GetFirstChild( id, cookie );
  while ( child.IsOk() )
  {
    LightingEnvironmentItem* data = Cast< LightingEnvironmentItem >( tree->GetItemData( child ), TypeLightingEnvironment );
    if ( data )
    {
      if ( data->GetLightingEnvironment() == env )
      {
        tree->Delete( child );
        break;
      }
    }
    child = tree->GetNextChild( id, cookie );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a lighting environment is added to a lighting job.  Updates the UI.
// 
void LightingEnvironmentGroupItem::LightingEnvironmentAdded( const LightingJobMemberChangeArgs& args )
{
  Luna::LightingEnvironment* env = Reflect::AssertCast< Luna::LightingEnvironment >( args.m_Member );
  if ( env )
  {
    AddLightingEnvironment( env );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a lighting environment is removed from a lighting job.  Updates the UI.
// 
void LightingEnvironmentGroupItem::LightingEnvironmentRemoved( const LightingJobMemberChangeArgs& args )
{
  Luna::LightingEnvironment* env = Reflect::AssertCast< Luna::LightingEnvironment >( args.m_Member );
  if ( env )
  {
    RemoveLightingEnvironment( env );
  }
}
