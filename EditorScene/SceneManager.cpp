#include "EditorScenePch.h"
#include "EditorScene/SceneManager.h"

#include "Foundation/Log.h"

#include "Framework/WorldManager.h"

#include "EditorScene/SwitchSceneCommand.h"
#include "EditorScene/Viewport.h"

using namespace Helium;
using namespace Helium::Editor;

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
#pragma TODO("Actually pass definition in here when appropriate")
ScenePtr SceneManager::NewScene( Editor::Viewport* viewport, Document* document, bool nested, SceneDefinitionPtr definition )
{
	if (definition.Get() == NULL)
	{
		definition = CreateSceneDefinition();
	}

	document->e_Closed.AddMethod( this, &SceneManager::DocumentClosed );
	document->e_PathChanged.AddMethod( this, &SceneManager::DocumentPathChanged );

	Scene::SceneType type = nested ? Scene::SceneTypes::Slice : Scene::SceneTypes::World;
	ScenePtr scene = new Editor::Scene( viewport, document->GetPath(), definition, type );
	m_DocumentToSceneTable.insert( M_DocumentToSceneTable::value_type( document, scene.Ptr() ) );
	m_SceneToDocumentTable.insert( M_SceneToDocumentTable::value_type( scene.Ptr(), document ) );

	scene->ConnectDocument( document );

	if ( nested )
	{
		// Increment the reference count on the nested scene.
		int32_t& referenceCount = m_AllocatedScenes.insert( M_AllocScene::value_type( scene, 0 ) ).first->second;
		++referenceCount;
	}

	AddScene( scene );

	return scene;
}

///////////////////////////////////////////////////////////////////////////////
// Open a zone that should be under the root.
// 
ScenePtr SceneManager::OpenScene( Editor::Viewport* viewport, Document* document, std::string& error )
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
void SceneManager::AddScene(Editor::Scene* scene)
{
	scene->d_Editing.Set( SceneEditingSignature::Delegate( this, &SceneManager::OnSceneEditing ) );

	std::pair< M_SceneSmartPtr::const_iterator, bool > inserted = m_Scenes.insert( M_SceneSmartPtr::value_type( scene->GetPath().Get(), scene ) );
	HELIUM_ASSERT(inserted.second);

	e_SceneAdded.Raise( SceneChangeArgs( NULL, scene ) );
}

///////////////////////////////////////////////////////////////////////////////
Editor::Scene* SceneManager::GetScene( const Document* document ) const
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
Editor::Scene* SceneManager::GetScene( const std::string& path ) const
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

void SceneManager::SaveAllScenes( std::string& error )
{
	M_SceneSmartPtr::const_iterator sceneItr = m_Scenes.begin();
	M_SceneSmartPtr::const_iterator sceneEnd = m_Scenes.end();
	for ( ; sceneItr != sceneEnd; ++sceneItr )
	{
		Editor::Scene* scene = sceneItr->second;

		M_SceneToDocumentTable::iterator findDocument = m_SceneToDocumentTable.find( scene );
		if ( findDocument != m_SceneToDocumentTable.end() )
		{
			Document* document = findDocument->second;
			std::string saveError;
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
void SceneManager::RemoveScene( Editor::Scene* scene )
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

	typedef std::vector< Editor::Scene* > V_SceneDumbPtr;
	V_SceneDumbPtr topLevelScenes;

	M_SceneSmartPtr::const_iterator sceneItr = m_Scenes.begin();
	M_SceneSmartPtr::const_iterator sceneEnd = m_Scenes.end();
	for ( ; sceneItr != sceneEnd; ++sceneItr )
	{
		Editor::Scene* scene = sceneItr->second;
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
bool SceneManager::IsNestedScene( Editor::Scene* scene ) const
{
	return m_AllocatedScenes.find( scene ) != m_AllocatedScenes.end();
}

///////////////////////////////////////////////////////////////////////////////
// Decrements the reference count on the specified scene.  If the reference 
// count reaches zero, the scene will be removed from the manager.
// 
void SceneManager::ReleaseNestedScene( Editor::Scene*& scene )
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
bool SceneManager::IsCurrentScene( const Editor::Scene* sceneToCompare ) const
{
	return m_CurrentScene == sceneToCompare;
}

///////////////////////////////////////////////////////////////////////////////
// "Gets" the "Current Scene"
// 
Editor::Scene* SceneManager::GetCurrentScene() const
{
	return m_CurrentScene;
}

///////////////////////////////////////////////////////////////////////////////
// Only one scene can be the active, editing one.  Any new items created, selcted,
// etc., will all interact with the current "editing scene." Use this function to 
// change which scene is being edited.
// 
void SceneManager::SetCurrentScene( Editor::Scene* scene )
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
Editor::Scene* SceneManager::FindFirstNonNestedScene() const
{
	Editor::Scene* found = NULL;
	M_SceneSmartPtr::const_iterator sceneItr = m_Scenes.begin();
	M_SceneSmartPtr::const_iterator sceneEnd = m_Scenes.end();
	const M_AllocScene::const_iterator nestedSceneEnd = m_AllocatedScenes.end();
	for ( ; sceneItr != sceneEnd && !found; ++sceneItr )
	{
		Editor::Scene* current = sceneItr->second;
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
	const Document* document = args.m_Document;
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
		std::pair< M_SceneSmartPtr::const_iterator, bool > inserted = m_Scenes.insert( M_SceneSmartPtr::value_type( scene->GetPath().Get(), scene ) );
		HELIUM_ASSERT( inserted.second );
	}
}

SceneDefinitionPtr SceneManager::CreateSceneDefinition()
{
	Package* pRootSceneDefinitionsPackage = WorldManager::GetStaticInstance().GetRootSceneDefinitionsPackage();

	std::string newWorldDefaultNameString( TXT( "NewWorld" ) );
	Name newWorldName( newWorldDefaultNameString.c_str() );
	int attempt = 1;
	do
	{
		if ( ! pRootSceneDefinitionsPackage->FindChild( newWorldName ) )
			break;

		std::stringstream newWorldNameStringStream;
		newWorldNameStringStream << newWorldDefaultNameString << TXT("_") << attempt;
		std::string newWorldNameString = newWorldNameStringStream.str();
		newWorldName = Name( newWorldNameString.c_str() );

		++attempt;
	} while (attempt < 100);

	SceneDefinitionPtr spSceneDefinition;
	bool success = SceneDefinition::Create( spSceneDefinition, newWorldName, pRootSceneDefinitionsPackage );

	if (!success)
		return NULL;

	HELIUM_ASSERT( spSceneDefinition );

	return spSceneDefinition;
}