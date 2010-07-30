#include "Point.h"

using namespace Helium;
using namespace Helium::Content;

REFLECT_DEFINE_CLASS(Point);

void Point::EnumerateClass( Reflect::Compositor<Point>& comp )
{
  Reflect::Field* fieldPosition = comp.AddField( &Point::m_Position, "m_Position" );
}
