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

            WorldProxy *GetCurrentWorldProxy() const { return m_WorldProxy.Get(); }
            World *GetCurrentWorld() const { return m_WorldProxy ? m_WorldProxy->GetWorld() : 0; }

        private:
            void InitRenderer( HWND hwnd );

            WorldProxyPtr m_WorldProxy;
        };
    }
}

#include "EditorEngine.inl"