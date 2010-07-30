#include "Precompile.h"
#include "CameraMovedCommand.h"

#include "Camera.h"
#include "Viewport.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Editor;

CameraMovedCommand::CameraMovedCommand(Editor::Viewport* view, Editor::Camera* cam )
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
