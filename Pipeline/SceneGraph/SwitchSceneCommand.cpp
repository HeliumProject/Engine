/*#include "Precompile.h"*/
#include "SwitchSceneCommand.h"

#include "Pipeline/SceneGraph/Scene.h"
#include "Pipeline/SceneGraph/SceneManager.h"

#include "Foundation/Log.h"
#include "Foundation/Undo/Exceptions.h"

using namespace Helium;
using namespace Helium::SceneGraph;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
SwitchSceneCommand::SwitchSceneCommand( SceneGraph::SceneManager* manager, SceneGraph::Scene* newScene )
: Undo::PropertyCommand< SceneGraph::Scene* >( new Helium::MemberProperty< SceneGraph::SceneManager, SceneGraph::Scene* >( manager, &SceneGraph::SceneManager::GetCurrentScene, &SceneGraph::SceneManager::SetCurrentScene ) )
, m_SceneManager( manager )
, m_OldScene( manager->GetCurrentScene() )
, m_NewScene( newScene )
, m_IsValid( true )
{
    m_SceneManager->e_SceneRemoving.AddMethod( this, &SwitchSceneCommand::SceneRemoving );

    // Automatically apply the new scene (can't do this as part of LPropertyCommand's constructor
    // because that would invalidate the current scene before we stored it in the m_OldScene variable).
    m_SceneManager->SetCurrentScene( m_NewScene );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
SwitchSceneCommand::~SwitchSceneCommand()
{
    m_SceneManager->e_SceneRemoving.RemoveMethod( this, &SwitchSceneCommand::SceneRemoving );
}

///////////////////////////////////////////////////////////////////////////////
// Switching scenes is not significant (it shouldn't cause either scene to
// be checked out or modified).
// 
bool SwitchSceneCommand::IsSignificant() const
{
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to make sure this command is still valid.
// 
void SwitchSceneCommand::Undo()
{
    if ( m_IsValid )
    {
        Base::Undo();
    }
    else
    {
        Log::Warning( TXT( "Undo command to switch scenes is invalid because one of the scenes was unloaded, doing nothing.\n" ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to make sure this command is still valid.
// 
void SwitchSceneCommand::Redo()
{
    if ( m_IsValid )
    {
        Base::Redo();
    }
    else
    {
        Log::Warning( TXT( "Redo command to switch scenes is invalid because one of the scenes was unloaded, doing nothing.\n" ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene is being unloaded.
// 
void SwitchSceneCommand::SceneRemoving( const SceneChangeArgs& args )
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
