#include "Foundation/Inspect/Canvas.h"
#include "Foundation/Inspect/Interpreter.h"
#include "Foundation/Inspect/Controls.h"

using namespace Helium;
using namespace Helium::Inspect;

Canvas::Canvas ()
{
    m_Canvas = this;
    m_DefaultSize = Math::Point (100, 20);
    m_Border = 4;
    m_Pad = 2;
}
