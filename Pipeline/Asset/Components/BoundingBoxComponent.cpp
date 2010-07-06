#include "BoundingBoxComponent.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( BoundingBoxComponent );

void BoundingBoxComponent::EnumerateClass( Reflect::Compositor< BoundingBoxComponent >& comp )
{
  comp.GetComposite().m_UIName = TXT( "Bounding Box" );

  comp.AddField( &BoundingBoxComponent::m_Extents, "m_Extents" );
  comp.AddField( &BoundingBoxComponent::m_Offset, "m_Offset" );
  comp.AddField( &BoundingBoxComponent::m_Minima, "m_Minima" );
  comp.AddField( &BoundingBoxComponent::m_Maxima, "m_Maxima" );
}

Component::ComponentUsage BoundingBoxComponent::GetComponentUsage() const
{
    return Component::ComponentUsages::Overridable;
}
