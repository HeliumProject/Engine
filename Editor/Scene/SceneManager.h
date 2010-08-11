#pragma once

#include "Editor/API.h"

#include "Scene.h"

#include "Foundation/Automation/Event.h"
#include "Core/Asset/Classes/SceneAsset.h"
#include "Application/Undo/QueueManager.h"

#include "Editor/DocumentManager.h"
#include "Editor/Selection.h"

#include "Pick.h"
#include "Render.h"

namespace Helium
{
    namespace Asset
    {
        class SceneAsset;
        typedef Helium::SmartPtr< SceneAsset > SceneAssetPtr;
    }

    namespace Editor
    {
        // Forwards
        class HierarchyNode;
        class Viewport;

        struct FilePathChangedArgs;
        struct NodeChangeArgs;

        struct SceneChangeArgs
        {
            Editor::Scene* m_Scene;

            SceneChangeArgs (Editor::Scene* scene)
                : m_Scene (scene)
            {
            }
        };

        typedef Helium::Signature< void, const SceneChangeArgs& > SceneChangeSignature;

        /////////////////////////////////////////////////////////////////////////////
        // Wrapper for files edited by the scene editor.  Handles RCS prompts (in the
        // base class) and stores a pointer to the scene that this file is associated
        // with.
        // 
        class SceneDocument : public Document
        {
        public:
            SceneDocument( const tstring& file, const tstring& name = TXT( "" ) )
                : Document( file, name )
                , m_Scene( NULL )
            {

            }
            
            void SetScene( Editor::Scene* scene )
            {
              HELIUM_ASSERT( m_Scene == NULL );
              m_Scene = scene;
            }
            
            Editor::Scene* GetScene() const
            {
              HELIUM_ASSERT( m_Scene != NULL );
              return m_Scene;
            }

            virtual bool Save( tstring& error );

        private:
            Editor::Scene* m_Scene;
        };

        typedef Helium::SmartPtr< SceneDocument > SceneDocumentPtr;

        /////////////////////////////////////////////////////////////////////////////
        // Tracks all the scenes and their undo queues.
        // 
        class EDITOR_SCENE_API SceneManager
        {
        private:
            // manages the documents (scenes)
            DocumentManager m_DocumentManager;

            // all loaded scenes by path
            M_SceneSmartPtr m_Scenes;

            // the nested scenes that can be freed
            M_AllocScene m_AllocatedScenes;

            // the current scene
            Editor::Scene* m_CurrentScene;

        public:
            SceneManager( MessageSignature::Delegate message );

            ScenePtr NewScene( Editor::Viewport* viewport, tstring path = TXT( "" ), bool addDoc = true );
            ScenePtr OpenScene( Editor::Viewport* viewport, const tstring& path, tstring& error );

            DocumentManager& GetDocumentManager()
            {
                return m_DocumentManager;
            }

            bool AllowChanges( SceneDocument* document )
            {
                return m_DocumentManager.AllowChanges( document );
            }

            void AddScene( Editor::Scene* scene );
            void RemoveScene( Editor::Scene* scene );
            void RemoveAllScenes();

            const M_SceneSmartPtr& GetScenes() const;
            Editor::Scene* GetScene( const tstring& path ) const;

            bool IsNestedScene( Editor::Scene* scene ) const;
            Editor::Scene* AllocateNestedScene( Editor::Viewport* viewport, const tstring& path, Editor::Scene* parent );
            void ReleaseNestedScene( Editor::Scene*& scene );

            static tstring GetUniqueFileName();

            bool HasCurrentScene() const;
            bool IsCurrentScene( const Editor::Scene* sceneToCompare ) const;

            Editor::Scene* GetCurrentScene() const;
            void SetCurrentScene( Editor::Scene* scene );

            // Undo/redo support
            bool CanUndo();
            bool CanRedo();
            void Undo();
            void Redo();
            void Push( Undo::Queue* queue );

        private:
            Editor::Scene* FindFirstNonNestedScene() const;
            bool OnSceneEditing( const SceneEditingArgs& args );
            void DocumentPathChanged( const DocumentPathChangedArgs& args );
            void DocumentClosed( const DocumentChangedArgs& args );

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
