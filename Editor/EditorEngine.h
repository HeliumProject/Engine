#pragma once

#include "Platform/Utility.h"
#include "Foundation/Map.h"
#include "Framework/Slice.h"
#include "Editor/Proxy/SceneProxy.h"
#include "SceneGraph/SceneManager.h"
#include "Application/TimerThread.h"

namespace Helium
{
    namespace Editor
    {
		class EditorEngine;

		class EngineTickTimer : public wxTimer
		{
		public:
			EngineTickTimer( EditorEngine &engine );
			~EngineTickTimer();

			void Notify();

		private:
			EditorEngine &m_Engine;
		};

        class EditorEngine : NonCopyable
        {
        public:
            EditorEngine();
            ~EditorEngine();

#if HELIUM_OS_WIN
            bool Initialize( SceneGraph::SceneManager* sceneManager, HWND hwnd );
#else
            bool Initialize( SceneGraph::SceneManager* sceneManager, void* hwnd );
#endif
            void Shutdown();

            void Tick();

        private:
            bool CreateRuntimeForScene( SceneGraph::Scene* scene );
            bool ReleaseRuntimeForScene( SceneGraph::Scene* scene );

            void OnSceneAdded( const SceneGraph::SceneChangeArgs& args );
            void OnSceneRemoving( const SceneGraph::SceneChangeArgs& args );

#if HELIUM_OS_WIN
            void InitRenderer( HWND hwnd );
#else
            void InitRenderer( void* hwnd );
#endif
            SceneGraph::SceneManager* m_SceneManager;

            typedef Helium::Map< SceneGraph::Scene*, Reflect::ObjectPtr > SceneProxyToRuntimeMap;
            SceneProxyToRuntimeMap m_SceneProxyToRuntimeMap;

		private:
			EngineTickTimer *m_pEngineTickTimer;
        };
    }
}

#include "EditorEngine.inl"