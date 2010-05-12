#include "Precompile.h"
#include "SwitchSceneCommand.h"

#include "Scene.h"
#include "SceneManager.h"

#include "Console/Console.h"
#include "Undo/Exceptions.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LSwitchSceneCommand::LSwitchSceneCommand( Luna::SceneManager* manager, Luna::Scene* newScene )
: Undo::PropertyCommand< Luna::Scene* >( new Nocturnal::MemberProperty< Luna::SceneManager, Luna::Scene* >( manager, &Luna::SceneManager::GetCurrentScene, &Luna::SceneManager::SetCurrentScene ) )
, m_SceneManager( manager )
, m_OldScene( manager->GetCurrentScene() )
, m_NewScene( newScene )
, m_IsValid( true )
{
  m_SceneManager->AddSceneRemovingListener( SceneChangeSignature::Delegate ( this, &LSwitchSceneCommand::SceneRemoving ) );
  
  // Automatically apply the new scene (can't do this as part of LPropertyCommand's constructor
  // because that would invalidate the current scene before we stored it in the m_OldScene variable).
  m_SceneManager->SetCurrentScene( m_NewScene );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LSwitchSceneCommand::~LSwitchSceneCommand()
{
  m_SceneManager->RemoveSceneRemovingListener( SceneChangeSignature::Delegate ( this, &LSwitchSceneCommand::SceneRemoving ) );
}

///////////////////////////////////////////////////////////////////////////////
// Switching scenes is not significant (it shouldn't cause either scene to
// be checked out or modified).
// 
bool LSwitchSceneCommand::IsSignificant() const
{
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to make sure this command is still valid.
// 
void LSwitchSceneCommand::Undo()
{
  if ( m_IsValid )
  {
    __super::Undo();
  }
  else
  {
    Console::Warning( "Undo command to switch scenes is invalid because one of the scenes was unloaded, doing nothing.\n" );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to make sure this command is still valid.
// 
void LSwitchSceneCommand::Redo()
{
  if ( m_IsValid )
  {
    __super::Redo();
  }
  else
  {
    Console::Warning( "Redo command to switch scenes is invalid because one of the scenes was unloaded, doing nothing.\n" );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene is being unloaded.
// 
void LSwitchSceneCommand::SceneRemoving( const SceneChangeArgs& args )
{
  if ( m_OldScene && ( m_OldScene == args.m_Scene ) )
  {
    // The scene that is being unloaded is one of the ones this command dealt with,
    // mark this command as invalid.
    m_OldScene = NULL;
    m_IsValid = false;
  }

  if ( m_NewScene && ( m_NewScene == args.m_Scene ) )
  {
    // The scene that is being unloaded is one of the ones this command dealt with,
    // mark this command as invalid.
    m_NewScene = NULL;
    m_IsValid = false;
  }
}
