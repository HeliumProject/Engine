#pragma once

#include "Editor/API.h"
#include "Editor/Document.h"

namespace Helium
{
    namespace Editor
    {
        // Forwards
        class Scene;

        /////////////////////////////////////////////////////////////////////////////
        // Wrapper for files edited by the scene editor.  Handles RCS prompts (in the
        // base class) and stores a pointer to the scene that this file is associated
        // with.
        // 
        class SceneDocument : public Document
        {
        private:
            Editor::Scene* m_Scene;

            // RTTI
            EDITOR_DECLARE_TYPE( SceneDocument, Document );
            static void InitializeType();
            static void CleanupType();

        public:
            SceneDocument( const tstring& file, const tstring& name = TXT( "" ) );
            virtual ~SceneDocument();
            void SetScene( Editor::Scene* scene );
            Editor::Scene* GetScene() const;
        };

        typedef Helium::SmartPtr< SceneDocument > SceneDocumentPtr;
    }
}