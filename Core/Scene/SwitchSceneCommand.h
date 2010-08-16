#pragma once

#include "Foundation/Undo/PropertyCommand.h"

#include "Core/API.h"

namespace Helium
{
    namespace Core
    {
        // Forwards
        class Scene;
        class SceneManager;
        struct SceneChangeArgs;

        /////////////////////////////////////////////////////////////////////////////
        // Undo command for switching the current scene.
        // 
        class CORE_API SwitchSceneCommand : public Undo::PropertyCommand< Core::Scene* >
        {
        private:
            Core::SceneManager* m_SceneManager;
            Core::Scene* m_OldScene;
            Core::Scene* m_NewScene;
            bool m_IsValid;

        public:
            SwitchSceneCommand( Core::SceneManager* manager, Core::Scene* newScene );
            virtual ~SwitchSceneCommand();
            virtual bool IsSignificant() const HELIUM_OVERRIDE;
            virtual void Undo() HELIUM_OVERRIDE;
            virtual void Redo() HELIUM_OVERRIDE;

        private:
            void SceneRemoving( const SceneChangeArgs& args );
        };
    }
}