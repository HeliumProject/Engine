#include "Precompile.h"
#include "LightingJobCommand.h"

#include "CubeMapProbe.h"
#include "Entity.h"
#include "Light.h"
#include "LightingJob.h"
#include "LightingVolume.h"
#include "LightingEnvironment.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Zone.h"

#include "File/Manager.h"
#include "Undo/Exceptions.h"
#include "Console/Console.h"
#include "AppUtils/AppUtils.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LightingJobCommand::LightingJobCommand( LightingJobCommandAction action, LightingJobCommandGroup group, Luna::LightingJob* job, const SceneNodePtr& node, bool execute )
: m_Action( action )
, m_Group( group )
, m_LightingJob( job )
, m_NodeID( node->GetID() )
, m_ZoneID( TUID::Null )
{
  m_ZoneID = node->GetScene()->GetFileID();
  NOC_ASSERT( m_ZoneID != TUID::Null );

  if ( execute )
  {
    Redo();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LightingJobCommand::~LightingJobCommand()
{
}

///////////////////////////////////////////////////////////////////////////////
// Undo the command.
// 
void LightingJobCommand::Undo()
{
  switch ( m_Action )
  {
  case LightingJobCommandActions::Add:
    Remove();
    break;

  case LightingJobCommandActions::Remove:
    Add();
    break;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Redo the command.
// 
void LightingJobCommand::Redo()
{
  switch ( m_Action )
  {
  case LightingJobCommandActions::Add:
    Add();
    break;

  case LightingJobCommandActions::Remove:
    Remove();
    break;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to add the object to the lighting group.  Returns true if
// successful.
// 
void LightingJobCommand::Add()
{
  bool isOk = false;

  Luna::Scene* nodeScene = Verify();
  switch ( m_Group )
  {
  case LightingJobCommandGroups::Light:
    {
      Luna::Light* light = Reflect::ObjectCast< Luna::Light >( nodeScene->FindNode( m_NodeID ) );
      if ( light )
      {
        isOk = m_LightingJob->AddLight( m_ZoneID, light );
      }
    }
    break;

  case LightingJobCommandGroups::Probe:
    {
      Luna::CubeMapProbe* probe = Reflect::ObjectCast< Luna::CubeMapProbe >( nodeScene->FindNode( m_NodeID ) );
      if ( probe )
      {
        isOk = m_LightingJob->AddCubeMapProbe( m_ZoneID, probe );
      }
    }
    break;

  case LightingJobCommandGroups::Render:
    {
      Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( nodeScene->FindNode( m_NodeID ) );
      if ( entity )
      {
        isOk = m_LightingJob->AddRenderTarget( m_ZoneID, entity );
      }
    }
    break;

  case LightingJobCommandGroups::Shadow:
    {
      Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( nodeScene->FindNode( m_NodeID ) );
      if ( entity )
      {
        isOk = m_LightingJob->AddShadowCaster( m_ZoneID, entity );
      }
    }
    break;

    case LightingJobCommandGroups::LightingEnvironment:
    {
      Luna::LightingEnvironment* item = Reflect::ObjectCast< Luna::LightingEnvironment >( nodeScene->FindNode( m_NodeID ) );
      if ( item )
      {
        isOk = m_LightingJob->AddLightingEnvironment( m_ZoneID, item );
      }
    }
    break;

    case LightingJobCommandGroups::Zone:
    {
      Zone* item = Reflect::ObjectCast< Zone >( nodeScene->FindNode( m_NodeID ) );
      if ( item )
      {
        isOk = m_LightingJob->AddZone( m_ZoneID, item );
      }
    }
    break;
  }

  // Do we care if !isOk?
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to remove the object from the lighting group.  Returns true
// if successful.
// 
void LightingJobCommand::Remove()
{
  bool isOk = false;

  // Error checking for this command.  Make sure that the scene has not been unloaded.
  Luna::Scene* nodeScene = Verify();

  switch ( m_Group )
  {
  case LightingJobCommandGroups::Light:
    {
      Luna::Light* light = Reflect::ObjectCast< Luna::Light >( nodeScene->FindNode( m_NodeID ) );
      if ( light )
      {
        isOk = m_LightingJob->RemoveLight( m_ZoneID, light );
      }
    }
    break;

  case LightingJobCommandGroups::Probe:
    {
      Luna::CubeMapProbe* probe = Reflect::ObjectCast< Luna::CubeMapProbe >( nodeScene->FindNode( m_NodeID ) );
      if ( probe )
      {
        isOk = m_LightingJob->RemoveCubeMapProbe( m_ZoneID, probe );
      }
    }
    break;

  case LightingJobCommandGroups::Render:
    {
      Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( nodeScene->FindNode( m_NodeID ) );
      if ( entity )
      {
        isOk = m_LightingJob->RemoveRenderTarget( m_ZoneID, entity );
      }
    }
    break;

  case LightingJobCommandGroups::Shadow:
    {
      Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( nodeScene->FindNode( m_NodeID ) );
      if ( entity )
      {
        isOk = m_LightingJob->RemoveShadowCaster( m_ZoneID, entity );
      }
    }
    break;

    case LightingJobCommandGroups::LightingEnvironment:
    {
      Luna::LightingEnvironment* env = Reflect::ObjectCast< Luna::LightingEnvironment >( nodeScene->FindNode( m_NodeID ) );
      if ( env )
      {
        isOk = m_LightingJob->RemoveLightingEnvironment( m_ZoneID, env );
      }
    }
    break;

    case LightingJobCommandGroups::Zone:
    {
      Zone* item = Reflect::ObjectCast< Zone >( nodeScene->FindNode( m_NodeID ) );
      if ( item )
      {
        isOk = m_LightingJob->RemoveZone( m_ZoneID, item );
      }
    }
    break;
  }

  // Do we care if !isOk?
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to fetch the scene that goes with the node that is being
// added or removed from the lighting job.  If that scene is no longer loaded,
// this function throws an Undo::Exception.
// 
Luna::Scene* LightingJobCommand::Verify()
{
  Luna::Scene* nodeScene = m_LightingJob->GetScene()->GetManager()->GetScene( m_ZoneID );
  if ( !nodeScene )
  {
    std::ostringstream sceneName;
    try
    {
      sceneName << File::GlobalManager().GetPath( m_ZoneID );
    }
    catch ( const File::Exception& e )
    {
      Console::Error( "%s\n", e.what() );
      sceneName << m_ZoneID;
    }

    throw Undo::Exception( "Problem with command for lighting job '%s'. Scene '%s' has been unloaded.", m_LightingJob->GetName().c_str(), sceneName.str().c_str() );
  }
  return nodeScene;
}
