#include "Precompile.h"
#include "LightItem.h"
#include "Light.h"
#include "LightingJob.h"
#include "LightingJobCommand.h"
#include "Scene.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LightItem::LightItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, Luna::Light* light )
: SceneNodeItem( tree, job, light, TypeLight )
, m_Light( light )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LightItem::~LightItem()
{
}

///////////////////////////////////////////////////////////////////////////////
// Return the light that this tree item wraps.
// 
Luna::Light* LightItem::GetLight() const
{
  return m_Light;
}

///////////////////////////////////////////////////////////////////////////////
// Returns a command for removing the light that this tree item wraps from the
// lighting job.
// 
Undo::CommandPtr LightItem::GetRemoveCommand( const OS_SelectableDumbPtr& selection )
{
  Undo::CommandPtr command;
  if ( selection.Contains( m_Light ) )
  {
    command = new LightingJobCommand( LightingJobCommandActions::Remove, LightingJobCommandGroups::Light, GetLightingJob(), m_Light, false );
  }
  return command;
}
