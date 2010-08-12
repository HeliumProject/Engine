#pragma once

#include "Application/Undo/Command.h"
#include "Foundation/Math/Matrix4.h"

namespace Helium
{
    namespace Core
    {
        class Camera;
        class Viewport;

        /// @class CameraMovedCommand
        /// @breif Used to implement a Undo/Redo queue for camera movement in the scene editor
        class CameraMovedCommand : public Undo::Command
        {
        private:
            Core::Camera*         m_Camera;
            Core::Viewport*           m_View;

            Math::Matrix4    m_PreviousTransform;
        public:
            CameraMovedCommand(Core::Viewport* view, Core::Camera* cam );
            virtual ~CameraMovedCommand();

        public:
            void Undo();
            void Redo();
        };

    } // namespace Core
}