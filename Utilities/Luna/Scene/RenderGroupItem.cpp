#include "Precompile.h"
#include "RenderGroupItem.h"

#include "Entity.h"
#include "LightingJob.h"
#include "Scene.h"
#include "SceneManager.h"

#include "Reflect/Object.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
RenderGroupItem::RenderGroupItem( wxTreeCtrlBase* tree, Luna::LightingJob* job )
: LightableGroupItem( tree, job, TypeRenderGroup )
{
  job->AddRenderTargetAddedListener( LightingJobMemberChangeSignature::Delegate ( this, &RenderGroupItem::RenderTargetAdded ) );
  job->AddRenderTargetRemovedListener( LightingJobMemberChangeSignature::Delegate ( this, &RenderGroupItem::RenderTargetRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
RenderGroupItem::~RenderGroupItem()
{
  GetLightingJob()->RemoveRenderTargetAddedListener( LightingJobMemberChangeSignature::Delegate ( this, &RenderGroupItem::RenderTargetAdded ) );
  GetLightingJob()->RemoveRenderTargetRemovedListener( LightingJobMemberChangeSignature::Delegate ( this, &RenderGroupItem::RenderTargetRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the label to display on this tree item.
// 
const std::string& RenderGroupItem::GetRenderGroupLabel()
{
  static const std::string label( "Render Targets" );
  return label;
}

///////////////////////////////////////////////////////////////////////////////
// Non-static version of above function.
// 
const std::string& RenderGroupItem::GetLabel() const
{
  return GetRenderGroupLabel();
}

///////////////////////////////////////////////////////////////////////////////
// Set up this group after creating it.
// 
void RenderGroupItem::Load()
{
  __super::Load();

  GetTree()->Freeze();
  S_tuid zoneIDs;
  GetLightingJob()->GetZoneIDs( zoneIDs );
  S_tuid::const_iterator zoneItr = zoneIDs.begin();
  S_tuid::const_iterator zoneEnd = zoneIDs.end();
  for ( ; zoneItr != zoneEnd; ++zoneItr )
  {
    LoadZoneData( &LightingJob::GetRenderTargets, *zoneItr );
  }
  GetTree()->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a render target is added to a lighting job.  Updates 
// the UI.
// 
void RenderGroupItem::RenderTargetAdded( const LightingJobMemberChangeArgs& args )
{
  Luna::Entity* entity = Reflect::AssertCast< Luna::Entity >( args.m_Member );
  if ( entity )
  {
    AddEntity( entity );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a render target is removed from a lighting job.  Updates 
// the UI.
// 
void RenderGroupItem::RenderTargetRemoved( const LightingJobMemberChangeArgs& args )
{
  Luna::Entity* entity = Reflect::AssertCast< Luna::Entity >( args.m_Member );
  if ( entity )
  {
    RemoveEntity( entity );
  }
}
