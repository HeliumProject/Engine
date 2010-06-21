#include "Precompile.h"
#include "SceneManager.h"

#include "Scene.h"
#include "SceneEditor.h"
#include "SceneDocument.h"
#include "SwitchSceneCommand.h"
#include "Zone.h"

#include "Pipeline/Asset/Classes/SceneAsset.h"
#include "Pipeline/Component/ComponentHandle.h"
#include "Foundation/Container/Insert.h" 
#include "Foundation/Log.h"
#include "Application/UI/FileDialog.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Returns a different name each time this function is called so that scenes
// can be uniquely named.
// 
static std::string GetUniqueSceneName()
{
    static i32 number = 1;

    std::ostringstream str;
    str << "Scene" << number++;
    return str.str();
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
SceneManager::SceneManager(SceneEditor* editor)
: DocumentManager( editor )
, m_CurrentScene( NULL )
, m_Editor (editor)
{

}

///////////////////////////////////////////////////////////////////////////////
// 
// 
SceneManager::~SceneManager()
{

}

///////////////////////////////////////////////////////////////////////////////
// The editor that owns us
// 
SceneEditor* SceneManager::GetEditor()
{
    return m_Editor;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the currently open level (if any).
// 
Asset::SceneAsset* SceneManager::GetCurrentLevel() const
{
    return m_CurrentLevel; 
}

///////////////////////////////////////////////////////////////////////////////
// Create a new scene.  Pass in true if this should be the root scene.
// 
ScenePtr SceneManager::NewScene( bool isRoot, std::string path, bool addDoc )
{
    std::string name;
    if ( path.empty() )
    {
        name = GetUniqueSceneName();
    }

    SceneDocumentPtr document = new SceneDocument( path, name );
    document->AddDocumentClosedListener( DocumentChangedSignature::Delegate( this, &SceneManager::OnDocumentClosed ) );
    ScenePtr scene = new Luna::Scene( this, document );
    if ( isRoot )
    {
        SetRootScene( scene );
    }
    AddScene( scene );

    if ( addDoc && !AddDocument( document ) )
    {
        // Shouldn't happen
        NOC_BREAK();
    }
    return scene;
}

///////////////////////////////////////////////////////////////////////////////
// Open a top-level scene (world).
// 
DocumentPtr SceneManager::OpenPath( const std::string& path, std::string& error ) 
{
    if ( !CloseAll() )
    {
        return NULL;
    }

    // Create a batch command to toggle between the previous scene, the new scene,
    // and no scene (while we are loading the file).  This batch command will be
    // committed to the undo stack only if this is not the first scene that we
    // are opening.
    Undo::BatchCommandPtr batch = new Undo::BatchCommand();
    Luna::Scene* previousScene = GetCurrentScene();
    if ( previousScene )
    {
        batch->Push( new LSwitchSceneCommand( this, NULL ) );
    }

    std::string scenePath = path;
    SceneDocumentPtr document;
    Nocturnal::Path filePath( path );

    ScenePtr scene = NewScene( m_Root == NULL, scenePath, true );
    if ( !scene->LoadFile( scenePath ) )
    {
        error = "Failed to load scene from " + path + ".";
        RemoveScene( scene );
        scene = NULL;
    }

    if ( scene.ReferencesObject() )
    {
        document = scene->GetSceneDocument();
        if ( !m_CurrentScene )
        {
            SetCurrentScene( scene );
        }

        batch->Push( new LSwitchSceneCommand( this, scene ) );
        if ( previousScene && GetRootScene() )
        {
            GetRootScene()->Push( batch );
        }
        // else: Throw away the batch;  it's already done the work of switching scenes
        // and we didn't start with a valid scene, so this command is meaningless to put
        // on the undo queue.
    }
    else
    {
        // If we switched from a valid scene, restore that scene as the current one
        if ( previousScene )
        {
            batch->Push( new LSwitchSceneCommand( this, previousScene ) );
        }
    }
    return document;
}

///////////////////////////////////////////////////////////////////////////////
// Open a zone that should be under the root.
// 
ScenePtr SceneManager::OpenZone( const std::string& path, std::string& error )
{
    ScenePtr scene = NewScene( false, path, true );
    if ( !scene->LoadFile( path ) )
    {
        error = "Failed to load scene from " + path + ".";
        RemoveScene( scene );
        scene = NULL;
    }
    else
    {
        Nocturnal::Path zonePath( path );

        S_ZoneDumbPtr::const_iterator zoneItr = m_Root->GetZones().begin();
        S_ZoneDumbPtr::const_iterator zoneEnd = m_Root->GetZones().end();
        for ( ; zoneItr != zoneEnd; ++zoneItr )
        {
            Zone* zone = *zoneItr;
            if ( zone->GetPathObject().Hash() == zonePath.Hash() )
            {
                scene->SetColor( zone->GetColor() );
                break;
            }
        }
    }
    return scene;
}

///////////////////////////////////////////////////////////////////////////////
// Prompt the user to save a file to a new location.  Returns the path to the
// new file location, or an empty string if the user cancels the operation.
// 
static std::string PromptSaveAs( const DocumentPtr& file, wxWindow* window )
{
    std::string path;
    std::string defaultDir = Nocturnal::Path( file->GetFilePath() ).Directory();
    std::string defaultFile = file->GetFilePath();

    Nocturnal::FileDialog saveDlg( window, "Save As...", defaultDir.c_str(), defaultFile.c_str(), "", Nocturnal::FileDialogStyles::DefaultSave );
    
    S_string extensions;
    Reflect::Archive::GetExtensions( extensions );
    for ( S_string::const_iterator itr = extensions.begin(), end = extensions.end(); itr != end; ++itr )
    {
        saveDlg.AddFilter( std::string( "Scene (*.scene." ) + *itr + ")|*.scene." + *itr );
    }

    if ( saveDlg.ShowModal() == wxID_OK )
    {
        path = saveDlg.GetPath();
    }

    return path;
}

///////////////////////////////////////////////////////////////////////////////
// Called when the "save all" option is chosen in the UI.  Iterates over all
// the open scenes and asks the session manager to save them.
// 
bool SceneManager::Save( DocumentPtr document, std::string& error )
{
    SceneDocument* sceneDocument = Reflect::ObjectCast< SceneDocument >( document );
    if ( !sceneDocument )
    {
        NOC_BREAK();
        error = document->GetFilePath() + " is not a valid scene file.";
        return false;
    }

    Luna::Scene* scene = sceneDocument->GetScene();
    if ( !scene )
    {
        NOC_BREAK();
        error = scene->GetFullPath() + " does not contain a valid scene to save.";
        return false;
    }

    // Check for "save as"
    if ( document->GetFilePath().empty() )
    {
        std::string savePath = PromptSaveAs( sceneDocument, m_Editor );
        if ( !savePath.empty() )
        {
            document->SetFilePath( savePath );
        }
        else
        {
            // No error, operation cancelled
            return true;
        }
    }

    if ( scene->Save() )
    {
        return __super::Save( document, error );
    }

    error = "Failed to save " + scene->GetFullPath();
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the root scene.  There can only be one root scene.  All zones that are
// created should be added to the root scene.
// 
void SceneManager::SetRootScene( Luna::Scene* root )
{
    if ( m_Root.Ptr() != root )
    {
        if ( m_Root.Ptr() )
        {
            // Do something?  Close all open scenes?
        }

        m_Root = root;

        if ( m_Root.Ptr() )
        {
            // Do something?  Notify listeners?
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the root scene.
// 
Luna::Scene* SceneManager::GetRootScene()
{
    return m_Root;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified scene is the root scene.
// 
bool SceneManager::IsRoot( Luna::Scene* scene ) const
{
    return ( ( scene != NULL ) && ( scene == m_Root ) );
}

///////////////////////////////////////////////////////////////////////////////
// Adds a scene to this manager
// 
void SceneManager::AddScene(Luna::Scene* scene)
{
    scene->GetSceneDocument()->AddDocumentPathChangedListener( DocumentPathChangedSignature::Delegate ( this, &SceneManager::DocumentPathChanged ) );
    scene->AddNodeRemovedListener( NodeChangeSignature::Delegate( this, &SceneManager::SceneNodeDeleting ) );

    const std::string& path = scene->GetFullPath();
    Nocturnal::Insert<M_SceneSmartPtr>::Result inserted = m_Scenes.insert( M_SceneSmartPtr::value_type( path, scene ) );
    NOC_ASSERT(inserted.second);

    m_SceneAdded.Raise( scene );
}

///////////////////////////////////////////////////////////////////////////////
// Removes a scene from this manager
// 
void SceneManager::RemoveScene(Luna::Scene* scene)
{
    // There is a problem in the code.  You should not be unloading a scene that
    // someone still has allocated.
    NOC_ASSERT( m_AllocatedScenes.find( scene ) == m_AllocatedScenes.end() );

    // RemoveScene is called for nested scenes and non-nested
    // we don't SaveVisibility in remove scene because i don't want to save it for nested scenes
    // and because we remove from m_AllocatedScenes before calling RemoveScene, 
    // we have no way to test (in this function) that it was a nested scene we're unloading. 
    // 

    scene->GetSceneDocument()->RemoveDocumentPathChangedListener( DocumentPathChangedSignature::Delegate ( this, &SceneManager::DocumentPathChanged ) );
    scene->RemoveNodeRemovedListener( NodeChangeSignature::Delegate( this, &SceneManager::SceneNodeDeleting ) );
    m_SceneRemoving.Raise( scene );

    M_SceneSmartPtr::iterator found = m_Scenes.find( scene->GetFullPath() );
    NOC_ASSERT( found != m_Scenes.end() );

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

    if ( IsRoot( scene ) )
    {
        SetRootScene( NULL );
    }

    RemoveDocument( scene->GetSceneDocument() );
    m_Scenes.erase( found );
}

///////////////////////////////////////////////////////////////////////////////
// Removes all the scenes from the scene manager.
// 
void SceneManager::RemoveAllScenes()
{
    SetCurrentScene( NULL );

#pragma TODO("This is a hack to support our current unwinding of all the allocated scenes, which is unordered.  We need to redesign the scene manager so that the root scene is deallocated last")
    ScenePtr root = m_Root; // hold a reference to the root while we close all its nested scenes

    typedef std::vector< Luna::Scene* > V_SceneDumbPtr;
    V_SceneDumbPtr topLevelScenes;

    M_SceneSmartPtr::const_iterator sceneItr = m_Scenes.begin();
    M_SceneSmartPtr::const_iterator sceneEnd = m_Scenes.end();
    for ( ; sceneItr != sceneEnd; ++sceneItr )
    {
        Luna::Scene* scene = sceneItr->second;
        if ( m_AllocatedScenes.find( scene ) == m_AllocatedScenes.end() )
        {
            topLevelScenes.push_back( scene );
        }
    }

    V_SceneDumbPtr::const_iterator removeItr = topLevelScenes.begin();
    V_SceneDumbPtr::const_iterator removeEnd = topLevelScenes.end();
    for ( ; removeItr != removeEnd; ++removeItr )
    {
        (*removeItr)->SaveVisibility(); 
        RemoveScene( *removeItr );
    }

    root = NULL;
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
Luna::Scene* SceneManager::GetScene( const std::string& path ) const
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
bool SceneManager::IsNestedScene( Luna::Scene* scene ) const
{
    return m_AllocatedScenes.find( scene ) != m_AllocatedScenes.end();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the specified scene and increases the reference count on that scene.
// If the scene was not yet created, this function will create a new scene.  If
// there was a problem loading the scene, it will be empty.  If you allocate a
// scene, you must call ReleaseNestedScene to free it.
// 
Luna::Scene* SceneManager::AllocateNestedScene( const std::string& path, Luna::Scene* parent )
{
    Luna::Scene* scene = GetScene( path );

    if ( !scene )
    {
        // Try to load nested scene.
        parent->ChangeStatus( std::string ("Loading ") + path + "..." );

        ScenePtr scenePtr = NewScene( false, path, false );
        if ( !scenePtr->LoadFile( path ) )
        {
            Log::Error( "Failed to load scene from %s\n", path.c_str() );
        }

        parent->ChangeStatus( "Ready" );
        scene = scenePtr;
    }

    if ( scene )
    {
        // Increment the reference count on the nested scene.
        i32& referenceCount = m_AllocatedScenes.insert( M_AllocScene::value_type( scene, 0 ) ).first->second;
        ++referenceCount;
    }

    return scene;
}

///////////////////////////////////////////////////////////////////////////////
// Decrements the reference count on the specified scene.  If the reference 
// count reaches zero, the scene will be removed from the manager.
// 
void SceneManager::ReleaseNestedScene( Luna::Scene*& scene )
{
    M_AllocScene::iterator found = m_AllocatedScenes.find( scene );
    if ( found != m_AllocatedScenes.end() )
    {
        i32& referenceCount = found->second;
        if ( --referenceCount == 0 )
        {
            m_AllocatedScenes.erase( found );
            RemoveScene( scene );
        }
    }
    else
    {
        // You tried to release a scene that was not allocated
        NOC_BREAK();
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
bool SceneManager::IsCurrentScene( const Luna::Scene* sceneToCompare ) const
{
    return m_CurrentScene == sceneToCompare;
}

///////////////////////////////////////////////////////////////////////////////
// "Gets" the "Current Scene"
// 
Luna::Scene* SceneManager::GetCurrentScene() const
{
    return m_CurrentScene;
}

///////////////////////////////////////////////////////////////////////////////
// Freezes sorting in all the outliners (tree controls).
// 
void SceneManager::FreezeTreeSorting()
{
    m_Editor->GetTreeMonitor().FreezeSorting();
}

///////////////////////////////////////////////////////////////////////////////
// Resumes sorting (and sorts) all the outliners.
// 
void SceneManager::ThawTreeSorting()
{
    m_Editor->GetTreeMonitor().ThawSorting();
}

///////////////////////////////////////////////////////////////////////////////
// Only one scene can be the active, editing one.  Any new items created, selcted,
// etc., will all interact with the current "editing scene." Use this function to 
// change which scene is being edited.
// 
void SceneManager::SetCurrentScene( Luna::Scene* scene )
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
// Returns true if there is a command on the undo stack.
// 
bool SceneManager::CanUndo()
{
    return m_UndoManager.CanUndo();
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if there is a command on the redo stack.
// 
bool SceneManager::CanRedo()
{
    return m_UndoManager.CanRedo();
}

///////////////////////////////////////////////////////////////////////////////
// Undo the previous command.
// 
void SceneManager::Undo()
{
    m_UndoManager.Undo();
}

///////////////////////////////////////////////////////////////////////////////
// Redo the previously undone command.
// 
void SceneManager::Redo()
{
    m_UndoManager.Redo();
}

///////////////////////////////////////////////////////////////////////////////
// The scene manager keeps track of which scene's undo queue contains the last
// command that was done.  For every command done on a scene, this Push call
// should be made to keep the scene manager up to date.  Adds the specified 
// undo queue to be the next undoable operation.
// 
void SceneManager::Push( Undo::Queue* queue )
{
    m_UndoManager.Push( queue );
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over the scenes and returns the first one that is not allocated
// (i.e. not a nested scene).
// 
Luna::Scene* SceneManager::FindFirstNonNestedScene() const
{
    Luna::Scene* found = NULL;
    M_SceneSmartPtr::const_iterator sceneItr = m_Scenes.begin();
    M_SceneSmartPtr::const_iterator sceneEnd = m_Scenes.end();
    const M_AllocScene::const_iterator nestedSceneEnd = m_AllocatedScenes.end();
    for ( ; sceneItr != sceneEnd && !found; ++sceneItr )
    {
        Luna::Scene* current = sceneItr->second;
        if ( m_AllocatedScenes.find( current ) == nestedSceneEnd )
        {
            found = current; // breaks out of loop
        }
    }

    return found;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the path of a scene changes.  Since the scene manager 
// stores the scenes by their paths, the internal list has to be updated.
// 
void SceneManager::DocumentPathChanged( const DocumentPathChangedArgs& args )
{
    const std::string pathOrName = !args.m_OldFilePath.empty() ? args.m_OldFilePath : args.m_OldFileName;
    M_SceneSmartPtr::iterator found = m_Scenes.find( pathOrName );
    if ( found != m_Scenes.end() )
    {
        // Hold a reference to the scene while we re-add it to the list, otherwise
        // it will get deleted.
        ScenePtr scene = found->second;

        m_Scenes.erase( found );
        Nocturnal::Insert<M_SceneSmartPtr>::Result inserted = 
            m_Scenes.insert( M_SceneSmartPtr::value_type( scene->GetFullPath(), scene ) );
        NOC_ASSERT( inserted.second );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a node is being deleted from a scene.  If the node is a 
// zone, and that zone is loaded, we will automatically unload it.
// 
void SceneManager::SceneNodeDeleting( const NodeChangeArgs& args )
{
    if ( args.m_Node->HasType( Reflect::GetType< Zone >() ) )
    {
        Document* doc = FindDocument( Reflect::TryCast< Zone >( args.m_Node )->GetPath() );
        if ( doc )
        {
            CloseDocument( doc );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a document is closed.  Closes the associated scene.
// 
void SceneManager::OnDocumentClosed( const DocumentChangedArgs& args )
{
    const SceneDocument* document = Reflect::ConstObjectCast< SceneDocument >( args.m_Document );
    NOC_ASSERT( document );

    m_Editor->SyncPropertyThread();

    if ( document )
    {
        ScenePtr scene = document->GetScene();

        // If the current scene is the one that is being closed, we need to set it
        // to no longer be the current scene.
        if ( HasCurrentScene() && GetCurrentScene() == scene )
        {
            SetCurrentScene( NULL );
        }

        if ( IsRoot( scene ) )
        {
            RemoveAllScenes();
            m_CurrentLevel = NULL;
        }
        else
        {
            scene->SaveVisibility(); 
            RemoveScene( scene );
        }

        // Select the next scene in the list, if there is one
        if ( !HasCurrentScene() )
        {
            SetCurrentScene( FindFirstNonNestedScene() );
        }

        document->RemoveDocumentClosedListener( DocumentChangedSignature::Delegate( this, &SceneManager::OnDocumentClosed ) );
    }
}
