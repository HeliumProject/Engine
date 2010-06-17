#include "NormalMapAttribute.h"

using namespace Asset;

REFLECT_DEFINE_ABSTRACT( NormalMapAttribute );

void NormalMapAttribute::EnumerateClass( Reflect::Compositor<NormalMapAttribute>& comp )
{
  comp.GetComposite().m_UIName = "Normal Map (Base)";
}


///////////////////////////////////////////////////////////////////////////////
// All classes that derive from NormalMapAttribute will occupy the same slot
// within an attribute collection.
// 
i32 NormalMapAttribute::GetSlot() const
{
  return Reflect::GetType< NormalMapAttribute >();
}