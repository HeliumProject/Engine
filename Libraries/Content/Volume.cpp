#include "Volume.h"

using namespace Content;

REFLECT_DEFINE_CLASS(Volume);

void Volume::EnumerateClass( Reflect::Compositor<Volume>& comp )
{
  Reflect::EnumerationField* enumShape = comp.AddEnumerationField( &Volume::m_Shape, "m_Shape" );
}