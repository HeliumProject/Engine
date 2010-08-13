#include "ContentVolume.h"

using namespace Helium;
using namespace Helium::Content;

REFLECT_DEFINE_CLASS(Volume);

void Volume::EnumerateClass( Reflect::Compositor<Volume>& comp )
{
  Reflect::EnumerationField* enumShape = comp.AddEnumerationField( &Volume::m_Shape, "m_Shape" );
}