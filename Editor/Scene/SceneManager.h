#pragma once

#include "Editor/API.h"

#include "Foundation/Automation/Event.h"
#include "Pipeline/Asset/Classes/SceneAsset.h"
#include "Application/Undo/QueueManager.h"

#include "Editor/DocumentManager.h"
#include "Editor/Selection.h"

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
        class SceneEditor;
        class HierarchyNode;
        class Viewport;

        class Scene;
        typedef Helium::SmartPtr< Editor::Scene > ScenePtr;
        typedef std::map< tstring, ScenePtr > M_SceneSmartPtr;
        typedef std::map< Editor::Scene*, i32 > M_AllocScene;

        class SceneDocument;
        typedef Helium::SmartPtr< SceneDocument > SceneDocumentPtr;

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
        // Tracks all the scenes and their undo queues.
        // 
        class EDITOR_SCENE_API SceneManager : public DocumentManager
        {
        private:
            // the root scene (the first one opened)
            ScenePtr m_Root;

            // all loaded scenes by path
            M_SceneSmartPtr m_Scenes;

            // the nested scenes that can be freed
            M_AllocScene m_AllocatedScenes;

            // the current scene
            Editor::Scene* m_CurrentScene;

#ifdef UI_REFACTOR
            SceneEditor* m_Editor;
#endif

            Undo::QueueManager m_UndoManager;

        public:
#ifdef UI_REFACTOR
            SceneManager( SceneEditor* editor );
            SceneEditor* GetEditor();
#else
            SceneManager( MessageSignature::Delegate message );
#endif

            ScenePtr NewScene( Editor::Viewport* viewport, bool isRoot, tstring path = TXT( "" ), bool addDoc = true );
            virtual DocumentPtr OpenPath( const tstring& path, tstring& error ) HELIUM_OVERRIDE;
            ScenePtr OpenScene( Editor::Viewport* viewport, const tstring& path, tstring& error );

        public:
            virtual bool Save( DocumentPtr document, tstring& error ) HELIUM_OVERRIDE;

            void SetRootScene( Editor::Scene* root );
            Editor::Scene* GetRootScene();
            bool IsRoot( Editor::Scene* scene ) const;

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