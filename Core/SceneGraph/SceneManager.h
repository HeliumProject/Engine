#pragma once

#include "Foundation/Automation/Event.h"

#include "Foundation/Document/Document.h"
#include "Foundation/Document/DocumentManager.h"

#include "Core/API.h"
#include "Core/Asset/Classes/SceneAsset.h"
#include "Core/SceneGraph/Selection.h"
#include "Core/SceneGraph/Scene.h"
#include "Core/SceneGraph/Pick.h"
#include "Core/SceneGraph/Render.h"

namespace Helium
{
    namespace Asset
    {
        class SceneAsset;
        typedef Helium::SmartPtr< SceneAsset > SceneAssetPtr;
    }

    namespace SceneGraph
    {
        // Forwards
        class HierarchyNode;
        class Viewport;

        struct FilePathChangedArgs;
        struct NodeChangeArgs;

        struct SceneChangeArgs
        {
            SceneGraph::Scene* m_Scene;

            SceneChangeArgs (SceneGraph::Scene* scene)
                : m_Scene (scene)
            {
            }
        };

        typedef Helium::Signature< const SceneChangeArgs& > SceneChangeSignature;


        /////////////////////////////////////////////////////////////////////////////
        // Tracks all the scenes and their undo queues.
        // 
        class CORE_API SceneManager
        {
        private:
            // manages the documents (scenes)
            DocumentManager m_DocumentManager;

            // all loaded scenes by path
            M_SceneSmartPtr m_Scenes;

            // scenes by document
            typedef std::map< const Document*, SceneGraph::Scene* > M_DocumentSceneTable;
            M_DocumentSceneTable m_DocumentSceneTable;

            // the nested scenes that can be freed
            M_AllocScene m_AllocatedScenes;

            // the current scene
            SceneGraph::Scene* m_CurrentScene;

        public:
            SceneManager( MessageSignature::Delegate message, FileDialogSignature::Delegate fileDialog );

            ScenePtr NewScene( SceneGraph::Viewport* viewport, Path path = TXT( "" ) );
            ScenePtr OpenScene( SceneGraph::Viewport* viewport, const tstring& path, tstring& error );

            DocumentManager& GetDocumentManager()
            {
                return m_DocumentManager;
            }

            bool AllowChanges( Document* document )
            {
                return m_DocumentManager.AllowChanges( document );
            }

            void AddScene( SceneGraph::Scene* scene );
            SceneGraph::Scene* GetScene( const Document* document ) const;
            SceneGraph::Scene* GetScene( const tstring& path ) const;
            const M_SceneSmartPtr& GetScenes() const;
            void RemoveScene( SceneGraph::Scene* scene );
            void RemoveAllScenes();

            bool IsNestedScene( SceneGraph::Scene* scene ) const;
            void AllocateNestedScene( const ResolveSceneArgs& args );
            void ReleaseNestedScene( SceneGraph::Scene*& scene );

            static tstring GetUniqueFileName();

            bool HasCurrentScene() const;
            bool IsCurrentScene( const SceneGraph::Scene* sceneToCompare ) const;

            SceneGraph::Scene* GetCurrentScene() const;
            void SetCurrentScene( SceneGraph::Scene* scene );

            // Undo/redo support
            bool CanUndo();
            bool CanRedo();
            void Undo();
            void Redo();
            void Push( Undo::Queue* queue );

        private:
            SceneGraph::Scene* FindFirstNonNestedScene() const;
            void OnSceneEditing( const SceneEditingArgs& args );
            
            void DocumentSave( const DocumentEventArgs& args );
            void DocumentClosed( const DocumentEventArgs& args );
            void DocumentPathChanged( const DocumentPathChangedArgs& args );

        private:
            SceneChangeSignature::Event m_SceneAdded;
        public:
            void AddSceneAddedListener( const SceneChangeSignature::Delegate& listener )
            {
                m_SceneAdded.Add( listener );
            }
            void RemoveSceneAddedListener( const SceneChangeSignature::Delegate& listener )
            {
                m_SceneAdded.Remove( listener );
            }

        private:
            SceneChangeSignature::Event m_SceneRemoving;
        public:
            void AddSceneRemovingListener( const SceneChangeSignature::Delegate& listener )
            {
                m_SceneRemoving.Add( listener );
            }
            void RemoveSceneRemovingListener( const SceneChangeSignature::Delegate& listener )
            {
                m_SceneRemoving.Remove( listener );
            }

        private:
            SceneChangeSignature::Event m_CurrentSceneChanging;
        public:
            void AddCurrentSceneChangingListener( const SceneChangeSignature::Delegate& listener )
            {
                m_CurrentSceneChanging.Add( listener );
            }
            void RemoveCurrentSceneChangingListener( const SceneChangeSignature::Delegate& listener )
            {
                m_CurrentSceneChanging.Remove( listener );
            }

        private:
            SceneChangeSignature::Event m_CurrentSceneChanged;
        public:
            void AddCurrentSceneChangedListener( const SceneChangeSignature::Delegate& listener )
            {
                m_CurrentSceneChanged.Add( listener );
            }
            void RemoveCurrentSceneChangedListener( const SceneChangeSignature::Delegate& listener )
            {
                m_CurrentSceneChanged.Remove( listener );
            }
        };
    }
}
