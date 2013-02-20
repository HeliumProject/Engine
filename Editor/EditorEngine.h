#pragma once

#include "Platform/Utility.h"

#include "Framework/Slice.h"
#include "Editor/Proxy/WorldProxy.h"

namespace Helium
{
    namespace Editor
    {
        class EditorEngine : NonCopyable
        {
        public:
            EditorEngine();
            ~EditorEngine();

            bool Initialize(HWND hwnd);
            void Shutdown();

            void OnViewCanvasPaint();

            WorldProxy *GetPrimaryWorldProxy() const { return m_PrimaryWorldProxy.Get(); }
            World *GetCurrentWorld() const { return m_PrimaryWorldProxy ? m_PrimaryWorldProxy->GetWorld() : 0; }

            void OpenWorld( WorldDefinition *spWorldDefinition );

        private:
            void InitRenderer( HWND hwnd );

            /// Currently the editor support loading a single scene. However, it may be useful to 
            WorldProxyPtr m_PrimaryWorldProxy;
            DynamicArray<WorldProxyPtr> m_WorldProxies;
        };
    }
}

#include "EditorEngine.inl"