/*#include "Precompile.h"*/
#include "CameraMovedCommand.h"

#include "Pipeline/SceneGraph/Camera.h"
#include "Pipeline/SceneGraph/Viewport.h"

using namespace Helium;
using namespace Helium::SceneGraph;

CameraMovedCommand::CameraMovedCommand(SceneGraph::Viewport* view, SceneGraph::Camera* cam )
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
}

void CameraMovedCommand::Redo()
{
    Matrix4 currMat = m_Camera->GetInverseView();
    std::swap( currMat, m_PreviousTransform );
    m_Camera->SetTransform( currMat );

    m_Camera->SetMovementMode( MovementModes::None );

    m_Camera->Update(true);     // camera moved, propogate messages et al.
}
