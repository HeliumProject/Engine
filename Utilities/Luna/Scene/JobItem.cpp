#include "Precompile.h"
#include "JobItem.h"

#include "CubeMapProbe.h"
#include "Light.h"
#include "LightGroupItem.h"
#include "LightingJob.h"
#include "LightingJobCommand.h"
#include "LightingVolume.h"
#include "ProbeGroupItem.h"
#include "RenderGroupItem.h"
#include "Scene.h"
#include "SceneManager.h"
#include "ShadowGroupItem.h"
#include "ZoneGroupItem.h"
#include "Zone.h"
#include "LightingEnvironment.h"
#include "LightingEnvironmentGroupItem.h"

#include "Core/Object.h"
#include "Common/Container/Insert.h" 

// Using
using Nocturnal::Insert; 
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
JobItem::JobItem( wxTreeCtrlBase* tree, Luna::LightingJob* job )
: LightingTreeItem( tree, job, TypeJob )
, m_LightGroup( NULL )
, m_RenderGroup( NULL )
, m_ShadowGroup( NULL )
, m_ProbeGroup( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
JobItem::~JobItem()
{
}

///////////////////////////////////////////////////////////////////////////////
// Loads all the lights and lightable instances for the specified zone into the
// children of this tree item.
// 
void JobItem::LoadZone( tuid zoneID )
{
  m_LightGroup->LoadZoneData( &LightingJob::GetLights, zoneID );
  m_RenderGroup->LoadZoneData( &LightingJob::GetRenderTargets, zoneID );
  m_ShadowGroup->LoadZoneData( &LightingJob::GetShadowCasters, zoneID );
  m_ProbeGroup->LoadZoneData( &LightingJob::GetCubeMapProbes, zoneID );
  m_EnvironmentGroup->LoadZoneData( &LightingJob::GetLightingEnvironments, zoneID );
  m_ZoneGroup->LoadZoneData( &LightingJob::GetZones, zoneID );


  // Remove Zone Unloaded item if necessary
  RemoveUnloadedZoneItem( zoneID );
}

///////////////////////////////////////////////////////////////////////////////
// Removes any lights and lightable instances that were part of this lighting
// job and in the specified zone.
// 
void JobItem::UnloadZone( tuid zoneID )
{
  m_LightGroup->UnloadZoneData( &LightingJob::GetLights, zoneID );
  m_RenderGroup->UnloadZoneData( &LightingJob::GetRenderTargets, zoneID );
  m_ShadowGroup->UnloadZoneData( &LightingJob::GetShadowCasters, zoneID );
  m_ProbeGroup->UnloadZoneData( &LightingJob::GetCubeMapProbes, zoneID );
  m_EnvironmentGroup->UnloadZoneData( &LightingJob::GetLightingEnvironments, zoneID );
  m_ZoneGroup->UnloadZoneData( &LightingJob::GetZones, zoneID );

  // Add Zone Unloaded item if necessary
  AddUnloadedZoneItem( zoneID );
}

///////////////////////////////////////////////////////////////////////////////
// Adds a tree item that represents a zone that is not currently loaded, only
// if there is not already a tree item for that zone.
// 
void JobItem::AddUnloadedZoneItem( const tuid& zoneID )
{
  Insert<M_UnloadedZone>::Result inserted = m_UnloadedZones.insert( M_UnloadedZone::value_type( zoneID, NULL ) );
  if ( inserted.second )
  {
    ZoneUnloadedItem* itemData = new ZoneUnloadedItem( GetTree(), GetLightingJob(), zoneID );
    GetTree()->AppendItem( GetId(), itemData->GetLabel().c_str(), -1, -1, itemData );
    inserted.first->second = itemData;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Removes a "unloaded zone" item, presumably because that zone has been loaded.
// 
void JobItem::RemoveUnloadedZoneItem( const tuid& zoneID )
{
  M_UnloadedZone::iterator found = m_UnloadedZones.find( zoneID );
  if ( found != m_UnloadedZones.end() )
  {
    ZoneUnloadedItem* itemData = found->second;
    GetTree()->Delete( itemData->GetId() );
    m_UnloadedZones.erase( found );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Call after creating an instance of this class to set up the UI for the first
// time and add sub items.
// 
void JobItem::Load()
{
  GetTree()->Freeze();

  GetTree()->SetItemBold( GetId() );

  m_RenderGroup = new RenderGroupItem( GetTree(), GetLightingJob() );
  GetTree()->AppendItem( GetId(), RenderGroupItem::GetRenderGroupLabel().c_str(), -1, -1, m_RenderGroup );
  m_RenderGroup->Load();

  m_ShadowGroup = new ShadowGroupItem( GetTree(), GetLightingJob() );
  GetTree()->AppendItem( GetId(), ShadowGroupItem::GetShadowGroupLabel().c_str(), -1, -1, m_ShadowGroup );
  m_ShadowGroup->Load();

  m_LightGroup = new LightGroupItem( GetTree(), GetLightingJob() );
  GetTree()->AppendItem( GetId(), LightGroupItem::GetLightGroupLabel().c_str(), -1, -1, m_LightGroup );
  m_LightGroup->Load();

  m_ProbeGroup = new ProbeGroupItem( GetTree(), GetLightingJob() );
  GetTree()->AppendItem( GetId(), ProbeGroupItem::GetProbeGroupLabel().c_str(), -1, -1, m_ProbeGroup );
  m_ProbeGroup->Load();

  m_EnvironmentGroup = new LightingEnvironmentGroupItem( GetTree(), GetLightingJob() );
  GetTree()->AppendItem( GetId(), LightingEnvironmentGroupItem::GetLightingEnvironmentGroupLabel().c_str(), -1, -1, m_EnvironmentGroup );
  m_EnvironmentGroup->Load();

  m_ZoneGroup = new ZoneGroupItem( GetTree(), GetLightingJob() );
  GetTree()->AppendItem( GetId(), ZoneGroupItem::GetZoneGroupLabel().c_str(), -1, -1, m_ZoneGroup );
  m_ZoneGroup->Load();

  Luna::SceneManager* sceneManager = GetLightingJob()->GetScene()->GetManager();
  S_tuid zoneIDs;
  GetLightingJob()->GetZoneIDs( zoneIDs );
  S_tuid::const_iterator zoneItr = zoneIDs.begin();
  S_tuid::const_iterator zoneEnd = zoneIDs.end();
  for ( ; zoneItr != zoneEnd; ++zoneItr )
  {
    // If the scene is not loaded...
    if ( sceneManager->GetScene( *zoneItr ) == NULL )
    {
      AddUnloadedZoneItem( *zoneItr );
    }
  }

  GetTree()->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this item can be selected, in the context of what other items
// are currently selected in the tree.
// 
bool JobItem::IsSelectable( const OS_TreeItemIds& currentSelection ) const
{
  bool isSelectable = false;

  if ( currentSelection.Size() == 0 )
  {
    isSelectable = true;
  }
  else
  {
    LightingTreeItem* firstSelected = reinterpret_cast< LightingTreeItem* >( GetTree()->GetItemData( *currentSelection.Begin() ) );
    isSelectable = ( firstSelected->GetType() == TypeJob );
  }

  return isSelectable;
}

///////////////////////////////////////////////////////////////////////////////
// Adds the lighting job that this tree item wraps to the selection.
// 
void JobItem::GetSelectableItems( OS_SelectableDumbPtr& selection ) const
{
  selection.Append( GetLightingJob() );
  __super::GetSelectableItems( selection );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true.  Jobs can be renamed within the tree.
// 
bool JobItem::CanRename() const
{
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Renames the lighting job node in the scene.
// 
void JobItem::Rename( const std::string& newName ) 
{
  Luna::LightingJob* job = GetLightingJob();
  job->GetScene()->Push( new Undo::PropertyCommand< std::string >( new Nocturnal::MemberProperty< Luna::SceneNode, std::string >( job, &Luna::SceneNode::GetName, &Luna::SceneNode::SetGivenName ), newName ) );
  job->GetScene()->Execute( false );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified child object can be added to the tree. 
// 
bool JobItem::CanAddChild( Object* object ) const
{
  bool canAdd = false;
  // If you want to add a shadow caster or render target, you have to select the 
  // appropriate item in the tree since they are not distinguishable from each 
  // other the way that the other types are.
  if ( !object->HasType( Reflect::GetType<Entity>() ) )
  {
    canAdd = true;
  }
  return canAdd;
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified object as a child of this one in the appropriate spot in
// the tree.
// 
Undo::CommandPtr JobItem::AddChild( Object* object )
{
  Undo::Command* command = NULL;
  if ( CanAddChild( object ) )
  {
    if ( object->HasType( Reflect::GetType<Luna::Light>() ) )
    {
      Luna::Light* light = Reflect::AssertCast< Luna::Light >( object );
      command = new LightingJobCommand( LightingJobCommandActions::Add, LightingJobCommandGroups::Light, GetLightingJob(), light, true );
    }
    else if ( object->HasType( Reflect::GetType<Luna::CubeMapProbe>() ) )
    {
      Luna::CubeMapProbe* probe = Reflect::AssertCast< Luna::CubeMapProbe >( object );
      command = new LightingJobCommand( LightingJobCommandActions::Add, LightingJobCommandGroups::Probe, GetLightingJob(), probe, true );
    }
    else if ( object->HasType( Reflect::GetType<Luna::LightingEnvironment>() ) )
    {
      Luna::LightingEnvironment* item = Reflect::AssertCast< Luna::LightingEnvironment>( object );
      command = new LightingJobCommand( LightingJobCommandActions::Add, LightingJobCommandGroups::LightingEnvironment, GetLightingJob(), item, true );
    }
    else if ( object->HasType( Reflect::GetType<Zone>() ) )
    {
      Zone* item = Reflect::AssertCast< Zone>( object );
      command = new LightingJobCommand( LightingJobCommandActions::Add, LightingJobCommandGroups::Zone, GetLightingJob(), item, true );
    }
  }
  return command;
}

///////////////////////////////////////////////////////////////////////////////
// Returns a command for removing all the items from this lighting job.
// 
Undo::CommandPtr JobItem::GetRemoveCommand( const OS_SelectableDumbPtr& selection )
{
  Undo::BatchCommandPtr batch = new Undo::BatchCommand();

  batch->Push( m_LightGroup->GetRemoveCommand( selection ) );
  batch->Push( m_RenderGroup->GetRemoveCommand( selection ) );
  batch->Push( m_ShadowGroup->GetRemoveCommand( selection ) );
  batch->Push( m_ProbeGroup->GetRemoveCommand( selection ) );
  batch->Push( m_EnvironmentGroup->GetRemoveCommand( selection ) );
  batch->Push( m_ZoneGroup->GetRemoveCommand( selection ) );

  if ( batch->IsEmpty() )
  {
    batch = NULL;
  }

  return batch;
}
