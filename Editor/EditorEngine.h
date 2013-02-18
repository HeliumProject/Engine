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

            bool Initialize();
            void InitRenderer( HWND hwnd );
            void Shutdown();

            WorldPtr GetCurrentWorld() const { return m_EditorWorld; }
            //SlicePtr GetEditorSlice() const { return m_EditorSlice; }

        private:
            //void CreateEditorWorld();

            //PackagePtr m_EditorPackage;
            WorldPtr m_EditorWorld;
            //SlicePtr m_EditorSlice; // the slice that holds transient editor-only entities
        };
    }
}