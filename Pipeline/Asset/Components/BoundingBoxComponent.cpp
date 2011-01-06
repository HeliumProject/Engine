#include "BoundingBoxComponent.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_CLASS( BoundingBoxComponent );

void BoundingBoxComponent::AcceptCompositeVisitor( Reflect::Composite& comp )
{
  comp.AddField( &BoundingBoxComponent::m_Extents, "m_Extents" );
  comp.AddField( &BoundingBoxComponent::m_Offset, "m_Offset" );
  comp.AddField( &BoundingBoxComponent::m_Minima, "m_Minima" );
  comp.AddField( &BoundingBoxComponent::m_Maxima, "m_Maxima" );
}

Component::ComponentUsage BoundingBoxComponent::GetComponentUsage() const
{
    return Component::ComponentUsages::Overridable;
}
