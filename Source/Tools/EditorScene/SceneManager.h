#pragma once

#include "Foundation/Event.h"

#include "EditorScene/API.h"
#include "EditorScene/Selection.h"
#include "EditorScene/Scene.h"
#include "EditorScene/Pick.h"
#include "EditorScene/Render.h"

namespace Helium
{
	namespace Editor
	{
		// Forwards
		class HierarchyNode;
		class Viewport;

		struct FilePathChangedArgs;
		struct NodeChangeArgs;

		struct SceneChangeArgs
		{
			Editor::Scene* m_PreviousScene;
			Editor::Scene* m_Scene;

			SceneChangeArgs ( Editor::Scene* previousScene, Editor::Scene* scene )
				: m_PreviousScene( previousScene )
				, m_Scene( scene )
			{
			}
		};

		typedef Helium::Signature< const SceneChangeArgs& > SceneChangeSignature;


		/////////////////////////////////////////////////////////////////////////////
		// Tracks all the scenes and their undo queues.
		// 
		class HELIUM_EDITOR_SCENE_API SceneManager
		{
		private:
			// all loaded scenes by path
			M_SceneSmartPtr m_Scenes;

			// scenes by definition
			typedef std::map< const SceneDefinitionPtr, Editor::Scene* > M_DefinitionToSceneTable;
			M_DefinitionToSceneTable m_DefinitionToSceneTable;

			typedef std::map< Editor::Scene*, SceneDefinition* > M_SceneToDefinitionTable;
			M_SceneToDefinitionTable m_SceneToDefinitionTable;

			// the nested scenes that can be freed
			M_AllocScene m_AllocatedScenes;

			// the current scene
			Editor::Scene* m_CurrentScene;

			Helium::WorldPtr m_World;

		public:
			SceneManager();
			~SceneManager();

			ScenePtr NewScene( Editor::Viewport* viewport, SceneDefinition* definition, bool nested = false );
			ScenePtr OpenScene( Editor::Viewport* viewport, SceneDefinition* definition, std::string& error );

			void AddScene( Editor::Scene* scene );
			Editor::Scene* GetScene( const SceneDefinition &definition ) const;
			Editor::Scene* GetScene( const std::string& path ) const;
			const M_SceneSmartPtr& GetScenes() const;
			void SaveAllScenes( std::string& error );
			void RemoveScene( Editor::Scene* scene );
			void RemoveAllScenes();

			bool IsNestedScene( Editor::Scene* scene ) const;
			void ReleaseNestedScene( Editor::Scene*& scene );

			static std::string GetUniqueFileName();

			bool HasCurrentScene() const;
			bool IsCurrentScene( const Editor::Scene* sceneToCompare ) const;

			Editor::Scene* GetCurrentScene() const;
			void SetCurrentScene( Editor::Scene* scene );

			SceneDefinitionPtr CreateSceneDefinition();

			// Undo/redo support
			bool CanUndo();
			bool CanRedo();
			void Undo();
			void Redo();
			void Push( UndoQueue* queue );

		private:
			Editor::Scene* FindFirstNonNestedScene() const;
			void OnSceneEditing( const SceneEditingArgs& args );

			//void DocumentClosed( const DocumentEventArgs& args );
			//void DocumentPathChanged( const DocumentPathChangedArgs& args );

		public:
			SceneChangeSignature::Event e_SceneAdded;
			SceneChangeSignature::Event e_SceneRemoving;
			SceneChangeSignature::Event e_CurrentSceneChanging;
			SceneChangeSignature::Event e_CurrentSceneChanged;
		};
	}
}
