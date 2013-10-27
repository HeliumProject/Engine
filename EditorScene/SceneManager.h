#pragma once

#include "Foundation/Event.h"
#include "Application/DocumentManager.h"

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

			// scenes by document
			typedef std::map< const Document*, Editor::Scene* > M_DocumentToSceneTable;
			M_DocumentToSceneTable m_DocumentToSceneTable;

			typedef std::map< Editor::Scene*, Document* > M_SceneToDocumentTable;
			M_SceneToDocumentTable m_SceneToDocumentTable;

			// the nested scenes that can be freed
			M_AllocScene m_AllocatedScenes;

			// the current scene
			Editor::Scene* m_CurrentScene;

		public:
			SceneManager();
			~SceneManager();

			ScenePtr NewScene( Editor::Viewport* viewport, Document* document, bool nested = false, SceneDefinitionPtr definition = NULL );
			ScenePtr OpenScene( Editor::Viewport* viewport, Document* document, std::string& error );

			void AddScene( Editor::Scene* scene );
			Editor::Scene* GetScene( const Document* document ) const;
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

			void DocumentClosed( const DocumentEventArgs& args );
			void DocumentPathChanged( const DocumentPathChangedArgs& args );

		public:
			SceneChangeSignature::Event e_SceneAdded;
			SceneChangeSignature::Event e_SceneRemoving;
			SceneChangeSignature::Event e_CurrentSceneChanging;
			SceneChangeSignature::Event e_CurrentSceneChanged;
		};
	}
}
