#include "Precompile.h"
#include "CameraMovedCommand.h"

#include "Camera.h"
#include "View.h"

using namespace Math;

namespace Luna
{

CameraMovedCommand::CameraMovedCommand(Luna::View* view, Luna::Camera* cam )
: m_Camera( cam ),
  m_View( view )
{
  m_PreviousTransform = cam->GetInverseView();
}

CameraMovedCommand::~CameraMovedCommand(void)
{
}

void CameraMovedCommand::Undo()
{
  Matrix4 currMat = m_Camera->GetInverseView();
  std::swap( currMat, m_PreviousTransform );
  m_Camera->SetTransform( currMat );

  m_Camera->SetMovementMode( MovementModes::None );

  
  m_Camera->Update(true);     // camera moved, propogate messages et al.
  m_View->Update();       // redraw scene
}

void CameraMovedCommand::Redo()
{
  Matrix4 currMat = m_Camera->GetInverseView();
  std::swap( currMat, m_PreviousTransform );
  m_Camera->SetTransform( currMat );

  m_Camera->SetMovementMode( MovementModes::None );

  m_Camera->Update(true);     // camera moved, propogate messages et al.
  m_View->Update();     // redraw scene
}

} // namespace Luna