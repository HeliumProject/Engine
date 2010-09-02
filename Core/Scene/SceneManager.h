#pragma once

#include "Foundation/Automation/Event.h"

#include "Foundation/Document/Document.h"
#include "Foundation/Document/DocumentManager.h"

#include "Core/API.h"
#include "Core/Asset/Classes/SceneAsset.h"
#include "Core/Scene/Selection.h"
#include "Core/Scene/Scene.h"
#include "Core/Scene/Pick.h"
#include "Core/Scene/Render.h"

namespace Helium
{
    namespace Asset
    {
        class SceneAsset;
        typedef Helium::SmartPtr< SceneAsset > SceneAssetPtr;
    }

    namespace Core
    {
        // Forwards
        class HierarchyNode;
        class Viewport;

        struct FilePathChangedArgs;
        struct NodeChangeArgs;

        struct SceneChangeArgs
        {
            Core::Scene* m_Scene;

            SceneChangeArgs (Core::Scene* scene)
                : m_Scene (scene)
            {
            }
        };

        typedef Helium::Signature< const SceneChangeArgs& > SceneChangeSignature;

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
            
            void SetScene( Core::Scene* scene )
            {
              HELIUM_ASSERT( m_Scene == NULL );
              m_Scene = scene;
            }
            
            Core::Scene* GetScene() const
            {
              HELIUM_ASSERT( m_Scene != NULL );
              return m_Scene;
            }

            virtual bool Save( tstring& error );

        private:
            Core::Scene* m_Scene;
        };

        typedef Helium::SmartPtr< SceneDocument > SceneDocumentPtr;

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

            // the nested scenes that can be freed
            M_AllocScene m_AllocatedScenes;

            // the current scene
            Core::Scene* m_CurrentScene;

        public:
            SceneManager( MessageSignature::Delegate message, FileDialogSignature::Delegate fileDialog );

            ScenePtr NewScene( Core::Viewport* viewport, tstring path = TXT( "" ) );
            ScenePtr OpenScene( Core::Viewport* viewport, const tstring& path, tstring& error );

            DocumentManager& GetDocumentManager()
            {
                return m_DocumentManager;
            }

            bool AllowChanges( SceneDocument* document )
            {
                return m_DocumentManager.AllowChanges( document );
            }

            void AddScene( Core::Scene* scene );
            void RemoveScene( Core::Scene* scene );
            void RemoveAllScenes();

            const M_SceneSmartPtr& GetScenes() const;
            Core::Scene* GetScene( const tstring& path ) const;

            bool IsNestedScene( Core::Scene* scene ) const;
            Core::Scene* AllocateNestedScene( Core::Viewport* viewport, const tstring& path, Core::Scene* parent );
            void ReleaseNestedScene( Core::Scene*& scene );

            static tstring GetUniqueFileName();

            bool HasCurrentScene() const;
            bool IsCurrentScene( const Core::Scene* sceneToCompare ) const;

            Core::Scene* GetCurrentScene() const;
            void SetCurrentScene( Core::Scene* scene );

            // Undo/redo support
            bool CanUndo();
            bool CanRedo();
            void Undo();
            void Redo();
            void Push( Undo::Queue* queue );

        private:
            Core::Scene* FindFirstNonNestedScene() const;
            void OnSceneEditing( const SceneEditingArgs& args );
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
