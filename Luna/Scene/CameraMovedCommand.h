#pragma once

#include "Application/Undo/Command.h"
#include "Foundation/Math/Matrix4.h"

namespace Luna
{
  class Camera;
  class Viewport;

/// @class CameraMovedCommand
/// @breif Used to implement a Undo/Redo queue for camera movement in the scene editor
class CameraMovedCommand : public Undo::Command
{
private:
  Luna::Camera*         m_Camera;
  Luna::Viewport*           m_View;

  Math::Matrix4    m_PreviousTransform;
public:
  CameraMovedCommand(Luna::Viewport* view, Luna::Camera* cam );
  virtual ~CameraMovedCommand();

public:
  void Undo();
  void Redo();
};

} // namespace Luna