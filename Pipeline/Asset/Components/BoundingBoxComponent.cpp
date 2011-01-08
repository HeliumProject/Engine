#include "BoundingBoxComponent.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_CLASS( BoundingBoxComponent );

void BoundingBoxComponent::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    comp.AddField( &BoundingBoxComponent::m_Extents, TXT( "m_Extents" ) );
    comp.AddField( &BoundingBoxComponent::m_Offset, TXT( "m_Offset" ) );
    comp.AddField( &BoundingBoxComponent::m_Minima, TXT( "m_Minima" ) );
    comp.AddField( &BoundingBoxComponent::m_Maxima, TXT( "m_Maxima" ) );
}

Component::ComponentUsage BoundingBoxComponent::GetComponentUsage() const
{
    return Component::ComponentUsages::Overridable;
}
