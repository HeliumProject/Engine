#include "Locator.h"

using namespace Content;

REFLECT_DEFINE_CLASS(Locator);

void Locator::EnumerateClass( Reflect::Compositor<Locator>& comp )
{
  Reflect::EnumerationField* enumShape = comp.AddEnumerationField( &Locator::m_Shape, "m_Shape" );
}