#include "Application/Inspect/Controls/InspectCanvas.h"
#include "Application/Inspect/InspectInterpreter.h"
#include "Application/Inspect/InspectControls.h"

using namespace Helium;
using namespace Helium::Inspect;

Canvas::Canvas ()
{
    m_Canvas = this;
    m_StdSize = Math::Point (100, 20);
    m_Border = 4;
    m_Pad = 2;
}

Canvas::~Canvas()
{

}

void Canvas::RealizeControl(Control* control, Control* parent)
{

}
