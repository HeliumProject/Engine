#pragma once

#include "Application/Undo/Command.h"
#include "Foundation/Math/Matrix4.h"

namespace Helium
{
    namespace Editor
    {
        class Camera;
        class Viewport;

        /// @class CameraMovedCommand
        /// @breif Used to implement a Undo/Redo queue for camera movement in the scene editor
        class CameraMovedCommand : public Undo::Command
        {
        private:
            Editor::Camera*         m_Camera;
            Editor::Viewport*           m_View;

            Math::Matrix4    m_PreviousTransform;
        public:
            CameraMovedCommand(Editor::Viewport* view, Editor::Camera* cam );
            virtual ~CameraMovedCommand();

        public:
            void Undo();
            void Redo();
        };

    } // namespace Editor
}