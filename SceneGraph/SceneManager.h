#pragma once

#include "Foundation/Event.h"
#include "Application/DocumentManager.h"

#include "SceneGraph/API.h"
#include "SceneGraph/Selection.h"
#include "SceneGraph/Scene.h"
#include "SceneGraph/Pick.h"
#include "SceneGraph/Render.h"

namespace Helium
{
    namespace SceneGraph
    {
        // Forwards
        class HierarchyNode;
        class Viewport;

        struct FilePathChangedArgs;
        struct NodeChangeArgs;

        struct SceneChangeArgs
        {
            SceneGraph::Scene* m_PreviousScene;
            SceneGraph::Scene* m_Scene;

            SceneChangeArgs ( SceneGraph::Scene* previousScene, SceneGraph::Scene* scene )
                : m_PreviousScene( previousScene )
                , m_Scene( scene )
            {
            }
        };

        typedef Helium::Signature< const SceneChangeArgs& > SceneChangeSignature;


        /////////////////////////////////////////////////////////////////////////////
        // Tracks all the scenes and their undo queues.
        // 
        class HELIUM_SCENE_GRAPH_API SceneManager
        {
        private:
            // all loaded scenes by path
            M_SceneSmartPtr m_Scenes;

            // scenes by document
            typedef std::map< const Document*, SceneGraph::Scene* > M_DocumentToSceneTable;
            M_DocumentToSceneTable m_DocumentToSceneTable;

            typedef std::map< SceneGraph::Scene*, Document* > M_SceneToDocumentTable;
            M_SceneToDocumentTable m_SceneToDocumentTable;

            // the nested scenes that can be freed
            M_AllocScene m_AllocatedScenes;

            // the current scene
            SceneGraph::Scene* m_CurrentScene;

        public:
            SceneManager();
            ~SceneManager();

            ScenePtr NewScene( SceneGraph::Viewport* viewport, Document* document, bool nested = false, SceneDefinitionPtr definition = NULL );
            ScenePtr OpenScene( SceneGraph::Viewport* viewport, Document* document, tstring& error );

            void AddScene( SceneGraph::Scene* scene );
            SceneGraph::Scene* GetScene( const Document* document ) const;
            SceneGraph::Scene* GetScene( const tstring& path ) const;
            const M_SceneSmartPtr& GetScenes() const;
            void SaveAllScenes( tstring& error );
            void RemoveScene( SceneGraph::Scene* scene );
            void RemoveAllScenes();

            bool IsNestedScene( SceneGraph::Scene* scene ) const;
            void ReleaseNestedScene( SceneGraph::Scene*& scene );

            static tstring GetUniqueFileName();

            bool HasCurrentScene() const;
            bool IsCurrentScene( const SceneGraph::Scene* sceneToCompare ) const;

            SceneGraph::Scene* GetCurrentScene() const;
            void SetCurrentScene( SceneGraph::Scene* scene );

            SceneDefinitionPtr CreateSceneDefinition();

            // Undo/redo support
            bool CanUndo();
            bool CanRedo();
            void Undo();
            void Redo();
            void Push( UndoQueue* queue );

        private:
            SceneGraph::Scene* FindFirstNonNestedScene() const;
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
