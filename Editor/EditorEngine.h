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

            bool Initialize( HWND hwnd );
            void Shutdown();

            WorldProxyPtr GetCurrentWorldProxy() const { return m_WorldProxy; }
            WorldPtr GetCurrentWorld() const { return m_WorldProxy ? m_WorldProxy->GetWorld() : 0; }

            void Update();

        private:
            void InitRenderer( HWND hwnd );

            //PackagePtr m_EditorPackage;
            //SlicePtr m_EditorSlice; // the slice that holds transient editor-only entities

            
            //WorldPtr m_EditorWorld;

            //DynamicArray<WorldProxyPtr> m_WorldProxies;
            WorldProxyPtr m_WorldProxy;
        };
    }
}