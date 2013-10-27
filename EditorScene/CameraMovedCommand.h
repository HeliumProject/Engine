#pragma once

#include "Application/UndoQueue.h"

#include "Math/Matrix4.h"

namespace Helium
{
	namespace Editor
	{
		class Camera;
		class Viewport;

		/// @class CameraMovedCommand
		/// @brief Used to implement a Undo/Redo queue for camera movement in the scene editor
		class CameraMovedCommand : public UndoCommand
		{
		private:
			Editor::Camera*         m_Camera;
			Editor::Viewport*           m_View;

			Matrix4    m_PreviousTransform;
		public:
			CameraMovedCommand(Editor::Viewport* view, Editor::Camera* cam );
			virtual ~CameraMovedCommand();

		public:
			void Undo();
			void Redo();
		};

	} // namespace Editor
}