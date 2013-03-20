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

            SlicePtr GetEditorSlice() const { return m_EditorSlice; }

        private:
            void InitRenderer( HWND hwnd );
            void CreateEditorWorld();

            PackagePtr m_EditorPackage;
            WorldPtr m_EditorWorld;
            SlicePtr m_EditorSlice; // the slice that holds transient editor-only entities
        };
    }
}