#include "Precompile.h"
#include "ZoneItem.h"
#include "LightingJob.h"
#include "Zone.h"
#include "LightingJobCommand.h"
#include "Scene.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ZoneItem::ZoneItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, Zone* zone )
: SceneNodeItem( tree, job, zone, TypeZone )
, m_Zone( zone )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ZoneItem::~ZoneItem()
{
}

///////////////////////////////////////////////////////////////////////////////
// Return the light that this tree item wraps.
// 
Zone* ZoneItem::GetZone() const
{
  return m_Zone;
}

///////////////////////////////////////////////////////////////////////////////
// Returns a command for removing the light that this tree item wraps from the
// lighting job.
// 
Undo::CommandPtr ZoneItem::GetRemoveCommand( const OS_SelectableDumbPtr& selection )
{
  Undo::CommandPtr command;
  if ( selection.Contains( m_Zone ) )
  {
    command = new LightingJobCommand( LightingJobCommandActions::Remove, LightingJobCommandGroups::Zone, GetLightingJob(), m_Zone, false );
  }
  return command;
}
