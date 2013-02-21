#pragma once

#include "Platform/Utility.h"
#include "Foundation/Map.h"
#include "Framework/Slice.h"
#include "Editor/Proxy/WorldProxy.h"
#include "SceneGraph/SceneManager.h"

namespace Helium
{
    namespace Editor
    {
        class EditorEngine : NonCopyable
        {
        public:
            EditorEngine();
            ~EditorEngine();

            bool Initialize( SceneGraph::SceneManager* sceneManager, HWND hwnd );
            void Shutdown();

            void OnViewCanvasPaint();

        private:
            Reflect::ObjectPtr CreateProxyFor( SceneGraph::Scene* scene );
            WorldProxyPtr CreateWorldProxy( SceneGraph::Scene* scene );

            void OnSceneAdded( const SceneGraph::SceneChangeArgs& args );
            void OnSceneRemoving( const SceneGraph::SceneChangeArgs& args );

            void InitRenderer( HWND hwnd );

            SceneGraph::SceneManager* m_SceneManager;

            typedef Helium::Map< SceneGraph::Scene*, Reflect::ObjectPtr > SceneToObjectMap;
            SceneToObjectMap m_SceneToDefinitionMap;
            SceneToObjectMap m_SceneToProxyMap;

            /// Currently the editor support loading a single scene. However, it may be useful to 
            WorldProxyPtr m_PrimaryWorldProxy;
            DynamicArray<WorldProxyPtr> m_WorldProxies;
        };
    }
}

#include "EditorEngine.inl"