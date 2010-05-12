#include "Precompile.h"
#include "ShadowGroupItem.h"

#include "Entity.h"
#include "LightingJob.h"
#include "LightingJobCommand.h"
#include "Scene.h"
#include "SceneManager.h"

#include "Reflect/Object.h"
#include "Console/Console.h"

// Using
using namespace Luna;


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ShadowGroupItem::ShadowGroupItem( wxTreeCtrlBase* tree, Luna::LightingJob* job )
: LightableGroupItem( tree, job, TypeShadowGroup )
{
  job->AddShadowCasterAddedListener( LightingJobMemberChangeSignature::Delegate ( this, &ShadowGroupItem::ShadowCasterAdded ) );
  job->AddShadowCasterRemovedListener( LightingJobMemberChangeSignature::Delegate ( this, &ShadowGroupItem::ShadowCasterRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ShadowGroupItem::~ShadowGroupItem()
{
  GetLightingJob()->RemoveShadowCasterAddedListener( LightingJobMemberChangeSignature::Delegate ( this, &ShadowGroupItem::ShadowCasterAdded ) );
  GetLightingJob()->RemoveShadowCasterRemovedListener( LightingJobMemberChangeSignature::Delegate ( this, &ShadowGroupItem::ShadowCasterRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the label to use for this tree item.
// 
const std::string& ShadowGroupItem::GetShadowGroupLabel()
{
  static const std::string label = "Excluded Shadow Casters";
  return label;
}

///////////////////////////////////////////////////////////////////////////////
// Non-static version of the above function.
// 
const std::string& ShadowGroupItem::GetLabel() const
{
  return GetShadowGroupLabel();
}

///////////////////////////////////////////////////////////////////////////////
// Prepare this item for use after first creating it.
// 
void ShadowGroupItem::Load() 
{
  __super::Load();

  GetTree()->Freeze();
  S_tuid zoneIDs;
  GetLightingJob()->GetZoneIDs( zoneIDs );
  S_tuid::const_iterator zoneItr = zoneIDs.begin();
  S_tuid::const_iterator zoneEnd = zoneIDs.end();
  for ( ; zoneItr != zoneEnd; ++zoneItr )
  {
    LoadZoneData( &LightingJob::GetShadowCasters, *zoneItr );
  }
  GetTree()->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified object as a child item under this group (if allowed).
// 
Undo::CommandPtr ShadowGroupItem::AddChild( Object* object )
{
  Undo::Command* command = NULL;

  if ( CanAddChild( object ) )
  {
    Luna::Entity* entity = Reflect::AssertCast< Luna::Entity >( object );

    tuid zoneID = entity->GetScene()->GetFileID();
    if ( zoneID != TUID::Null )
    {
      command = new LightingJobCommand( LightingJobCommandActions::Add, LightingJobCommandGroups::Shadow, GetLightingJob(), entity, true );
    }
    else
    {
#pragma TODO("Need better UI feedback for the user")
      Console::Error( "Zone '%s' is not in the file resolver yet. Unable to add '%s' to lighting job '%s'.\n", entity->GetScene()->GetFileName().c_str(), entity->GetName().c_str(), GetLightingJob()->GetName().c_str() );
    }
  }

  return command;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a shadow caster is added to this lighting job.  Updates 
// the UI.
// 
void ShadowGroupItem::ShadowCasterAdded( const LightingJobMemberChangeArgs& args )
{
  Luna::Entity* entity = Reflect::AssertCast< Luna::Entity >( args.m_Member );
  if ( entity )
  {
    AddEntity( entity );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a shadow caster is removed from this lighting job.
// Updates the UI.
// 
void ShadowGroupItem::ShadowCasterRemoved( const LightingJobMemberChangeArgs& args )
{
  Luna::Entity* entity = Reflect::AssertCast< Luna::Entity >( args.m_Member );
  if ( entity )
  {
    RemoveEntity( entity );
  }
}

void ShadowGroupItem::SelectItem( Luna::SceneNode* node )
{
  return;
}

///////////////////////////////////////////////////////////////////////////////
// context menu
//

ContextMenuItemSet ShadowGroupItem::GetContextMenuItems()
{
  ContextMenuItemSet menuItems;

  ContextMenuItemPtr removeAll = new ContextMenuItem( "Remove All", "Remove all Shadow Casters from the lighting job" );
  removeAll->AddCallback( ContextMenuSignature::Delegate( this, &ShadowGroupItem::RemoveAll ) );
  menuItems.AppendItem( removeAll );

  return menuItems;
}

#pragma TODO( "figure out how to make this remove the gui representation of these items from the lighting job tree as well" )
void ShadowGroupItem::RemoveAll( const ContextMenuArgsPtr& args )
{
  Luna::LightingJob* job = GetLightingJob();

  Content::LightingJobPtr contentLightingJob = job->GetPackage< Content::LightingJob >();

  Content::M_LightingJobZoneItems::iterator itor =  contentLightingJob->m_JobElements.begin();
  Content::M_LightingJobZoneItems::iterator end  =  contentLightingJob->m_JobElements.end();

  for( ; itor != end; ++itor )
  {
    itor->second->m_ExcludedShadowCasters.clear();
  }
}