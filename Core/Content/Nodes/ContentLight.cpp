#include "ContentLight.h"

using namespace Helium;
using namespace Helium::Content;

REFLECT_DEFINE_ABSTRACT(Light)

void Light::EnumerateClass( Reflect::Compositor<Light>& comp )
{
    comp.AddField( &Light::m_Color, "m_Color" );
}
