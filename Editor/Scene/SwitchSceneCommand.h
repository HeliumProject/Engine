#pragma once

#include "Editor/API.h"
#include "Application/Undo/PropertyCommand.h"

namespace Helium
{
    namespace Editor
    {
        // Forwards
        class Scene;
        class SceneManager;
        struct SceneChangeArgs;

        /////////////////////////////////////////////////////////////////////////////
        // Undo command for switching the current scene.
        // 
        class SwitchSceneCommand : public Undo::PropertyCommand< Editor::Scene* >
        {
        private:
            Editor::SceneManager* m_SceneManager;
            Editor::Scene* m_OldScene;
            Editor::Scene* m_NewScene;
            bool m_IsValid;

        public:
            SwitchSceneCommand( Editor::SceneManager* manager, Editor::Scene* newScene );
            virtual ~SwitchSceneCommand();
            virtual bool IsSignificant() const HELIUM_OVERRIDE;
            virtual void Undo() HELIUM_OVERRIDE;
            virtual void Redo() HELIUM_OVERRIDE;

        private:
            void SceneRemoving( const SceneChangeArgs& args );
        };
    }
}