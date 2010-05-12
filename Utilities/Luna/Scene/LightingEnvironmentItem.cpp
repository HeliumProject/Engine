#include "Precompile.h"
#include "LightingEnvironment.h"
#include "LightingEnvironmentItem.h"
#include "LightingJob.h"
#include "LightingJobCommand.h"
#include "Scene.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LightingEnvironmentItem::LightingEnvironmentItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, Luna::LightingEnvironment* env )
: SceneNodeItem( tree, job, env, TypeLightingEnvironment )
, m_LightingEnvironment( env )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LightingEnvironmentItem::~LightingEnvironmentItem()
{
}

///////////////////////////////////////////////////////////////////////////////
// Return the light that this tree item wraps.
// 
Luna::LightingEnvironment* LightingEnvironmentItem::GetLightingEnvironment() const
{
  return m_LightingEnvironment;
}

///////////////////////////////////////////////////////////////////////////////
// Returns a command for removing the light that this tree item wraps from the
// lighting job.
// 
Undo::CommandPtr LightingEnvironmentItem::GetRemoveCommand( const OS_SelectableDumbPtr& selection )
{
  Undo::CommandPtr command;
  if ( selection.Contains( m_LightingEnvironment ) )
  {
    command = new LightingJobCommand( LightingJobCommandActions::Remove, LightingJobCommandGroups::LightingEnvironment, GetLightingJob(), m_LightingEnvironment, false );
  }
  return command;
}
