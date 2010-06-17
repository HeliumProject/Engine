#include "DetailMapComponent.h"

using namespace Asset;

REFLECT_DEFINE_ABSTRACT( DetailMapComponent );

void DetailMapComponent::EnumerateClass( Reflect::Compositor<DetailMapComponent>& comp )
{
  comp.GetComposite().m_UIName = "Detail Map (Base)";
}


///////////////////////////////////////////////////////////////////////////////
// All classes that derive from DetailMapComponent will occupy the same slot
// within an attribute collection.
// 
i32 DetailMapComponent::GetSlot() const
{
  return Reflect::GetType< DetailMapComponent >();
}