#pragma once

#include "Platform/Utility.h"
#include "Foundation/Map.h"
#include "Framework/Slice.h"
#include "Editor/Proxy/SceneProxy.h"
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
            bool CreateRuntimeForScene( SceneGraph::Scene* scene );
            bool ReleaseRuntimeForScene( SceneGraph::Scene* scene );

            void OnSceneAdded( const SceneGraph::SceneChangeArgs& args );
            void OnSceneRemoving( const SceneGraph::SceneChangeArgs& args );

            void InitRenderer( HWND hwnd );

            SceneGraph::SceneManager* m_SceneManager;

            typedef Helium::Map< SceneGraph::Scene*, Reflect::ObjectPtr > SceneProxyToRuntimeMap;
            SceneProxyToRuntimeMap m_SceneProxyToRuntimeMap;
        };
    }
}

#include "EditorEngine.inl"