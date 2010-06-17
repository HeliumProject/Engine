#include "NormalMapComponent.h"

using namespace Asset;

REFLECT_DEFINE_ABSTRACT( NormalMapComponent );

void NormalMapComponent::EnumerateClass( Reflect::Compositor<NormalMapComponent>& comp )
{
  comp.GetComposite().m_UIName = "Normal Map (Base)";
}


///////////////////////////////////////////////////////////////////////////////
// All classes that derive from NormalMapComponent will occupy the same slot
// within an attribute collection.
// 
i32 NormalMapComponent::GetSlot() const
{
  return Reflect::GetType< NormalMapComponent >();
}