#pragma once

#include "Foundation/Undo/UndoCommand.h"

#include "SceneGraph/API.h"

namespace Helium
{
    namespace SceneGraph
    {
        // Forwards
        class Scene;
        class SceneManager;
        struct SceneChangeArgs;

        /////////////////////////////////////////////////////////////////////////////
        // Undo command for switching the current scene.
        // 
        class HELIUM_SCENE_GRAPH_API SwitchSceneCommand : public PropertyUndoCommand< SceneGraph::Scene* >
        {
        public:
            typedef PropertyUndoCommand< SceneGraph::Scene* > Base;

        private:
            SceneGraph::SceneManager* m_SceneManager;
            SceneGraph::Scene* m_OldScene;
            SceneGraph::Scene* m_NewScene;
            bool m_IsValid;

        public:
            SwitchSceneCommand( SceneGraph::SceneManager* manager, SceneGraph::Scene* newScene );
            virtual ~SwitchSceneCommand();
            virtual bool IsSignificant() const HELIUM_OVERRIDE;
            virtual void Undo() HELIUM_OVERRIDE;
            virtual void Redo() HELIUM_OVERRIDE;

        private:
            void SceneRemoving( const SceneChangeArgs& args );
        };
    }
}