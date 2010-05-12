#include "Precompile.h"
#include "ProbeItem.h"
#include "CubeMapProbe.h"
#include "LightingJob.h"
#include "LightingJobCommand.h"
#include "Scene.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ProbeItem::ProbeItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, Luna::CubeMapProbe* probe )
: SceneNodeItem( tree, job, probe, TypeProbe )
, m_Probe( probe )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ProbeItem::~ProbeItem()
{
}

///////////////////////////////////////////////////////////////////////////////
// Return the cube map probe that this tree item wraps.
// 
Luna::CubeMapProbe* ProbeItem::GetProbe() const
{
  return m_Probe;
}

///////////////////////////////////////////////////////////////////////////////
// Returns a command for removing the light that this tree item wraps from the
// lighting job.
// 
Undo::CommandPtr ProbeItem::GetRemoveCommand( const OS_SelectableDumbPtr& selection )
{
  Undo::CommandPtr command;
  if ( selection.Contains( m_Probe ) )
  {
    command = new LightingJobCommand( LightingJobCommandActions::Remove, LightingJobCommandGroups::Probe, GetLightingJob(), m_Probe, false );
  }
  return command;
}
