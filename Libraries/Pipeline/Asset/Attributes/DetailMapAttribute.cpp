#include "DetailMapAttribute.h"

using namespace Asset;

REFLECT_DEFINE_ABSTRACT( DetailMapAttribute );

void DetailMapAttribute::EnumerateClass( Reflect::Compositor<DetailMapAttribute>& comp )
{
  comp.GetComposite().m_UIName = "Detail Map (Base)";
}


///////////////////////////////////////////////////////////////////////////////
// All classes that derive from DetailMapAttribute will occupy the same slot
// within an attribute collection.
// 
i32 DetailMapAttribute::GetSlot() const
{
  return Reflect::GetType< DetailMapAttribute >();
}