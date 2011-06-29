#include "FoundationPch.h"
#include "Foundation/Inspect/Canvas.h"
#include "Foundation/Inspect/Interpreter.h"
#include "Foundation/Inspect/Controls.h"

using namespace Helium;
using namespace Helium::Inspect;

REFLECT_DEFINE_OBJECT( Inspect::Canvas );

Canvas::Canvas ()
{
    m_Canvas = this;
    m_DefaultSize = Point (20, 20);
    m_Border = 4;
    m_Pad = 2;
}

Canvas::~Canvas()
{
    // you *must* clear before destroying the canvas (since the canvas
    //  must still be constructed when clearing its children because
    //  Realize/Unrealize is pure virtual)
    HELIUM_ASSERT( m_Children.empty() );
}
