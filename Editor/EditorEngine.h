#pragma once

#include "Platform/Utility.h"

#include "Framework/Slice.h"
#include "Framework/World.h"

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

            void Update();

            World* GetEditorWorld() const { return m_EditorWorld; }

        private:
            void InitRenderer( HWND hwnd );
            void CreateEditorWorld();

            PackagePtr m_EditorPackage;
            WorldPtr m_EditorWorld;
        };
    }
}