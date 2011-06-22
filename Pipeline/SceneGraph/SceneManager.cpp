#include "PipelinePch.h"
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
// 
// 
SceneManager::SceneManager()
: m_CurrentScene( NULL )
{
}

SceneManager::~SceneManager()
{
    m_Scenes.clear();
    m_DocumentToSceneTable.clear();
    m_SceneToDocumentTable.clear();
    m_AllocatedScenes.clear();
    m_CurrentScene = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Create a new scene.  Pass in true if this should be the root scene.
// 
ScenePtr SceneManager::NewScene( SceneGraph::Viewport* viewport, Document* document, bool nested )
{
    document->e_Closed.AddMethod( this, &SceneManager::DocumentClosed );
    document->e_PathChanged.AddMethod( this, &SceneManager::DocumentPathChanged );

    ScenePtr scene = new SceneGraph::Scene( viewport, document->GetPath() );
    m_DocumentToSceneTable.insert( M_DocumentToSceneTable::value_type( document, scene.Ptr() ) );
    m_SceneToDocumentTable.insert( M_SceneToDocumentTable::value_type( scene.Ptr(), document ) );

    scene->ConnectDocument( document );

    AddScene( scene );

    if ( nested )
    {
        // Increment the reference count on the nested scene.
        int32_t& referenceCount = m_AllocatedScenes.insert( M_AllocScene::value_type( scene, 0 ) ).first->second;
        ++referenceCount;
    }

    return scene;
}

///////////////////////////////////////////////////////////////////////////////
// Open a zone that should be under the root.
// 
ScenePtr SceneManager::OpenScene( SceneGraph::Viewport* viewport, Document* document, tstring& error )
{
    ScenePtr scene = NewScene( viewport, document );
    if ( !scene->Load( document->GetPath() ) )
    {
        error = TXT( "Failed to load scene from " ) + document->GetPath().Get() + TXT( "." );
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
 
    Helium::StdInsert<M_SceneSmartPtr>::Result inserted = m_Scenes.insert( M_SceneSmartPtr::value_type( scene->GetPath().Get(), scene ) );
    HELIUM_ASSERT(inserted.second);

    e_SceneAdded.Raise( SceneChangeArgs( NULL, scene ) );
}

///////////////////////////////////////////////////////////////////////////////
SceneGraph::Scene* SceneManager::GetScene( const Document* document ) const
{
    M_DocumentToSceneTable::const_iterator foundDocument = m_DocumentToSceneTable.find( document );
    if ( foundDocument != m_DocumentToSceneTable.end() )
    {
        return foundDocument->second;
    }

    return NULL;
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
// Returns the full list of scenes (includes nested scenes).
// 
const M_SceneSmartPtr& SceneManager::GetScenes() const 
{
    return m_Scenes;
}

void SceneManager::SaveAllScenes( tstring& error )
{
    M_SceneSmartPtr::const_iterator sceneItr = m_Scenes.begin();
    M_SceneSmartPtr::const_iterator sceneEnd = m_Scenes.end();
    for ( ; sceneItr != sceneEnd; ++sceneItr )
    {
        SceneGraph::Scene* scene = sceneItr->second;

        M_SceneToDocumentTable::iterator findDocument = m_SceneToDocumentTable.find( scene );
        if ( findDocument != m_SceneToDocumentTable.end() )
        {
            Document* document = findDocument->second;
            tstring saveError;
            document->Save( saveError );

            if ( !saveError.empty() )
            {
                error += saveError;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Removes a scene from this manager
// 
void SceneManager::RemoveScene( SceneGraph::Scene* scene )
{
    // There is a problem in the code.  You should not be unloading a scene that
    // someone still has allocated.
    HELIUM_ASSERT( m_AllocatedScenes.find( scene ) == m_AllocatedScenes.end() );

    M_SceneToDocumentTable::iterator findDocument = m_SceneToDocumentTable.find( scene );
    if ( findDocument != m_SceneToDocumentTable.end() )
    {
        Document* document = findDocument->second;

        scene->DisconnectDocument( document );

        document->e_Closed.RemoveMethod( this, &SceneManager::DocumentClosed );
        document->e_PathChanged.RemoveMethod( this, &SceneManager::DocumentPathChanged );

        m_DocumentToSceneTable.erase( document );
        m_SceneToDocumentTable.erase( findDocument );
    }

    e_SceneRemoving.Raise( SceneChangeArgs( NULL, scene ) );

    scene->d_Editing.Clear();

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
// Returns true if the specified scene is a nested (allocated) scene.  Otherwise
// the scene is a world or zone.
// 
bool SceneManager::IsNestedScene( SceneGraph::Scene* scene ) const
{
    return m_AllocatedScenes.find( scene ) != m_AllocatedScenes.end();
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

    e_CurrentSceneChanging.Raise( SceneChangeArgs( m_CurrentScene, scene ) );

    Scene* previousScene = m_CurrentScene;
    m_CurrentScene = scene;

    e_CurrentSceneChanged.Raise( SceneChangeArgs( previousScene, m_CurrentScene ) );
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
    M_SceneToDocumentTable::iterator findDocument = m_SceneToDocumentTable.find( args.m_Scene );
    if ( findDocument != m_SceneToDocumentTable.end() )
    {
        const Document* document = findDocument->second;
        if ( document )
        {
            args.m_Veto = !document->IsCheckedOut();
            return;
        }
    }

    args.m_Veto = true;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a document is closed.  Closes the associated scene.
// 
void SceneManager::DocumentClosed( const DocumentEventArgs& args )
{
    const Document* document = static_cast< const Document* >( args.m_Document );
    HELIUM_ASSERT( document );

    if ( document )
    {
        Scene* scene = GetScene( document );

        scene->DisconnectDocument( document );

        // If the current scene is the one that is being closed, we need to set it
        // to no longer be the current scene.
        if ( HasCurrentScene() && GetCurrentScene() == scene )
        {
            SetCurrentScene( NULL );
        }

#pragma TODO( "Is this sane?" )
        while( IsNestedScene( scene ) )
        {
            ReleaseNestedScene( scene );
        }

        if ( scene )
        {
            RemoveScene( scene );
        }

        // Select the next scene in the list, if there is one
        if ( !HasCurrentScene() )
        {
            SetCurrentScene( FindFirstNonNestedScene() );
        }

        document->e_Closed.RemoveMethod( this, &SceneManager::DocumentClosed );
        document->e_PathChanged.RemoveMethod( this, &SceneManager::DocumentPathChanged );
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
        Helium::StdInsert<M_SceneSmartPtr>::Result inserted = m_Scenes.insert( M_SceneSmartPtr::value_type( scene->GetPath().Get(), scene ) );
        HELIUM_ASSERT( inserted.second );
    }
}
