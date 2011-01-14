#include "MeshProcessingComponent.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_ENUMERATION( MeshCompressionFactor );
REFLECT_DEFINE_OBJECT( MeshProcessingComponent );

void MeshProcessingComponent::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    comp.AddField( &MeshProcessingComponent::a_ScalingFactor, TXT( "a_ScalingFactor" ) );
    comp.AddEnumerationField( &MeshProcessingComponent::a_MeshCompressionFactor, TXT( "a_MeshCompressionFactor" ) );
    comp.AddField( &MeshProcessingComponent::a_FlipWinding, TXT( "a_FlipWinding" ) );
}

Component::ComponentUsage MeshProcessingComponent::GetComponentUsage() const
{
    return Component::ComponentUsages::Class;
}


