/*#include "Precompile.h"*/
#include "Pipeline/SceneGraph/SceneManager.h"

#include "Foundation/Component/ComponentHandle.h"
#include "Foundation/Container/Insert.h" 
#include "Foundation/Log.h"
#include "Pipeline/Asset/Classes/SceneAsset.h"

#include "Pipeline/SceneGraph/SwitchSceneCommand.h"
#include "Pipeline/SceneGraph/Viewport.h"

using namespace Helium;
using namespace Helium::SceneGraph;

///////////////////////////////////////////////////////////////////////////////
// Returns a different name each time this function is called so that scenes
// can be uniquely named.
// 
static tstring GetUniqueSceneName()
{
    static int32_t number = 1;

    tostringstream str;
    str << "Scene" << number++;
    return str.str();
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
SceneManager::SceneManager( MessageSignature::Delegate message, FileDialogSignature::Delegate fileDialog )
: m_DocumentManager( message, fileDialog )
, m_CurrentScene( NULL )
{

}

///////////////////////////////////////////////////////////////////////////////
// Create a new scene.  Pass in true if this should be the root scene.
// 
ScenePtr SceneManager::NewScene( SceneGraph::Viewport* viewport, Path path )
{
    if ( path.empty() )
    {
        path = GetUniqueSceneName();
    }

    SceneDocumentPtr document = new SceneDocument( path );
    document->AddDocumentClosedListener( DocumentChangedSignature::Delegate( this, &SceneManager::DocumentClosed ) );
    document->AddDocumentPathChangedListener( DocumentPathChangedSignature::Delegate ( this, &SceneManager::DocumentPathChanged ) );

    ScenePtr scene = new SceneGraph::Scene( viewport, path );
    document->SetScene( scene );

    tstring error;
    bool result = m_DocumentManager.OpenDocument( document, error );
    HELIUM_ASSERT( result );

    AddScene( scene );
    return scene;
}

///////////////////////////////////////////////////////////////////////////////
// Open a zone that should be under the root.
// 
ScenePtr SceneManager::OpenScene( SceneGraph::Viewport* viewport, const tstring& path, tstring& error )
{
    ScenePtr scene = NewScene( viewport, path );
    if ( !scene->Load( path ) )
    {
        error = TXT( "Failed to load scene from " ) + path + TXT( "." );
        RemoveScene( scene );
        scene = NULL;
    }

    return scene;
}

///////////////////////////////////////////////////////////////////////////////
// Adds a scene to this manager
// 
void SceneManager::AddScene(SceneGraph::Scene* scene)
{
    scene->d_Editing.Set( SceneEditingSignature::Delegate( this, &SceneManager::OnSceneEditing ) );
    scene->d_ResolveScene.Set( ResolveSceneSignature::Delegate( this, &SceneManager::AllocateNestedScene ) );

    Helium::Insert<M_SceneSmartPtr>::Result inserted = m_Scenes.insert( M_SceneSmartPtr::value_type( scene->GetPath().Get(), scene ) );
    HELIUM_ASSERT(inserted.second);

    m_SceneAdded.Raise( scene );
}

///////////////////////////////////////////////////////////////////////////////
// Removes a scene from this manager
// 
void SceneManager::RemoveScene(SceneGraph::Scene* scene)
{
    // There is a problem in the code.  You should not be unloading a scene that
    // someone still has allocated.
    HELIUM_ASSERT( m_AllocatedScenes.find( scene ) == m_AllocatedScenes.end() );

    m_DocumentManager.FindDocument( scene->GetPath() )->RemoveDocumentPathChangedListener( DocumentPathChangedSignature::Delegate ( this, &SceneManager::DocumentPathChanged ) );
    m_SceneRemoving.Raise( scene );

    scene->d_Editing.Clear();
    scene->d_ResolveScene.Clear();

    M_SceneSmartPtr::iterator found = m_Scenes.find( scene->GetPath().Get() );
    HELIUM_ASSERT( found != m_Scenes.end() );

    if (found->second.Ptr() == m_CurrentScene)
    {
        if (m_Scenes.size() <= 1)
        {
            SetCurrentScene(NULL);
        }
        else
        {
            SetCurrentScene( FindFirstNonNestedScene() ); 
        }
    }

    m_Scenes.erase( found );
}

///////////////////////////////////////////////////////////////////////////////
// Removes all the scenes from the scene manager.
// 
void SceneManager::RemoveAllScenes()
{
    SetCurrentScene( NULL );

    typedef std::vector< SceneGraph::Scene* > V_SceneDumbPtr;
    V_SceneDumbPtr topLevelScenes;

    M_SceneSmartPtr::const_iterator sceneItr = m_Scenes.begin();
    M_SceneSmartPtr::const_iterator sceneEnd = m_Scenes.end();
    for ( ; sceneItr != sceneEnd; ++sceneItr )
    {
        SceneGraph::Scene* scene = sceneItr->second;
        if ( m_AllocatedScenes.find( scene ) == m_AllocatedScenes.end() )
        {
            topLevelScenes.push_back( scene );
        }
    }

    V_SceneDumbPtr::const_iterator removeItr = topLevelScenes.begin();
    V_SceneDumbPtr::const_iterator removeEnd = topLevelScenes.end();
    for ( ; removeItr != removeEnd; ++removeItr )
    {
        RemoveScene( *removeItr );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the full list of scenes (includes nested scenes).
// 
const M_SceneSmartPtr& SceneManager::GetScenes() const 
{
    return m_Scenes;
}

///////////////////////////////////////////////////////////////////////////////
// Finds a scene by full path in this manager
// 
SceneGraph::Scene* SceneManager::GetScene( const tstring& path ) const
{
    M_SceneSmartPtr::const_iterator found = m_Scenes.find( path );

    if (found != m_Scenes.end())
    {
        return found->second.Ptr();
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified scene is a nested (allocated) scene.  Otherwise
// the scene is a world or zone.
// 
bool SceneManager::IsNestedScene( SceneGraph::Scene* scene ) const
{
    return m_AllocatedScenes.find( scene ) != m_AllocatedScenes.end();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the specified scene and increases the reference count on that scene.
// If the scene was not yet created, this function will create a new scene.  If
// there was a problem loading the scene, it will be empty.  If you allocate a
// scene, you must call ReleaseNestedScene to free it.
// 
void SceneManager::AllocateNestedScene( const ResolveSceneArgs& args )
{
    args.m_Scene = GetScene( args.m_Path );

    if ( !args.m_Scene )
    {
        // Try to load nested scene.
        //parent->ChangeStatus( TXT("Loading ") + path + TXT( "..." ) );

        ScenePtr scenePtr = NewScene( args.m_Viewport, args.m_Path );
        if ( !scenePtr->Load( args.m_Path ) )
        {
            Log::Error( TXT( "Failed to load scene from %s\n" ), args.m_Path.c_str() );
        }

        //parent->ChangeStatus( TXT( "Ready" ) );
        args.m_Scene = scenePtr;
    }

    if ( args.m_Scene )
    {
        // Increment the reference count on the nested scene.
        int32_t& referenceCount = m_AllocatedScenes.insert( M_AllocScene::value_type( args.m_Scene, 0 ) ).first->second;
        ++referenceCount;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Decrements the reference count on the specified scene.  If the reference 
// count reaches zero, the scene will be removed from the manager.
// 
void SceneManager::ReleaseNestedScene( SceneGraph::Scene*& scene )
{
    M_AllocScene::iterator found = m_AllocatedScenes.find( scene );
    if ( found != m_AllocatedScenes.end() )
    {
        int32_t& referenceCount = found->second;
        if ( --referenceCount == 0 )
        {
            m_AllocatedScenes.erase( found );
            RemoveScene( scene );
        }
    }
    else
    {
        // You tried to release a scene that was not allocated
        HELIUM_BREAK();
    }

    scene = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if there is currently a valid scene that has focus for any
// edits.
// 
bool SceneManager::HasCurrentScene() const
{
    return m_CurrentScene != NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the scene that is passed in is the same as the current
// "editing scene".
// 
bool SceneManager::IsCurrentScene( const SceneGraph::Scene* sceneToCompare ) const
{
    return m_CurrentScene == sceneToCompare;
}

///////////////////////////////////////////////////////////////////////////////
// "Gets" the "Current Scene"
// 
SceneGraph::Scene* SceneManager::GetCurrentScene() const
{
    return m_CurrentScene;
}

///////////////////////////////////////////////////////////////////////////////
// Only one scene can be the active, editing one.  Any new items created, selcted,
// etc., will all interact with the current "editing scene." Use this function to 
// change which scene is being edited.
// 
void SceneManager::SetCurrentScene( SceneGraph::Scene* scene )
{
    if ( m_CurrentScene == scene )
    {
        return;
    }

    m_CurrentSceneChanging.Raise( m_CurrentScene );

    m_CurrentScene = scene;

    m_CurrentSceneChanged.Raise( m_CurrentScene );
}


///////////////////////////////////////////////////////////////////////////////
// Iterates over the scenes and returns the first one that is not allocated
// (i.e. not a nested scene).
// 
SceneGraph::Scene* SceneManager::FindFirstNonNestedScene() const
{
    SceneGraph::Scene* found = NULL;
    M_SceneSmartPtr::const_iterator sceneItr = m_Scenes.begin();
    M_SceneSmartPtr::const_iterator sceneEnd = m_Scenes.end();
    const M_AllocScene::const_iterator nestedSceneEnd = m_AllocatedScenes.end();
    for ( ; sceneItr != sceneEnd && !found; ++sceneItr )
    {
        SceneGraph::Scene* current = sceneItr->second;
        if ( m_AllocatedScenes.find( current ) == nestedSceneEnd )
        {
            found = current; // breaks out of loop
        }
    }

    return found;
}

void SceneManager::OnSceneEditing( const SceneEditingArgs& args )
{
    SceneDocument* document = (SceneDocument*)m_DocumentManager.FindDocument( args.m_Scene->GetPath() );
    if ( document )
    {
        args.m_Veto = !m_DocumentManager.IsCheckedOut( document );
    }
    else
    {
        args.m_Veto = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the path of a scene changes.  Since the scene manager 
// stores the scenes by their paths, the internal list has to be updated.
// 
void SceneManager::DocumentPathChanged( const DocumentPathChangedArgs& args )
{
    M_SceneSmartPtr::iterator found = m_Scenes.find( args.m_OldPath );
    if ( found != m_Scenes.end() )
    {
        // Hold a reference to the scene while we re-add it to the list, otherwise
        // it will get deleted.
        ScenePtr scene = found->second;

        // remove the scene
        m_Scenes.erase( found );

        // change the path of the scene
        scene->SetPath( args.m_Document->GetPath() );

        // re-insert w/ new path
        Helium::Insert<M_SceneSmartPtr>::Result inserted = m_Scenes.insert( M_SceneSmartPtr::value_type( scene->GetPath().Get(), scene ) );
        HELIUM_ASSERT( inserted.second );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a document is closed.  Closes the associated scene.
// 
void SceneManager::DocumentClosed( const DocumentChangedArgs& args )
{
    const SceneDocument* document = static_cast< const SceneDocument* >( args.m_Document );
    HELIUM_ASSERT( document );

    if ( document )
    {
        ScenePtr scene = document->GetScene();

        // If the current scene is the one that is being closed, we need to set it
        // to no longer be the current scene.
        if ( HasCurrentScene() && GetCurrentScene() == scene )
        {
            SetCurrentScene( NULL );
        }

        RemoveScene( scene );

        // Select the next scene in the list, if there is one
        if ( !HasCurrentScene() )
        {
            SetCurrentScene( FindFirstNonNestedScene() );
        }

        document->RemoveDocumentClosedListener( DocumentChangedSignature::Delegate( this, &SceneManager::DocumentClosed ) );
    }
}
