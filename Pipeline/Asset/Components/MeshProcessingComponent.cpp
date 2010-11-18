#include "MeshProcessingComponent.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_ENUMERATION( MeshCompressionFactor );
REFLECT_DEFINE_CLASS( MeshProcessingComponent );

void MeshProcessingComponent::EnumerateClass( Reflect::Compositor< MeshProcessingComponent >& comp )
{
    comp.GetComposite().m_UIName = TXT( "Mesh Processing" );

    comp.AddField( &MeshProcessingComponent::a_ScalingFactor, "a_ScalingFactor" );
    comp.AddEnumerationField( &MeshProcessingComponent::a_MeshCompressionFactor, "a_MeshCompressionFactor" );
    comp.AddField( &MeshProcessingComponent::a_FlipWinding, "a_FlipWinding" );
}

Component::ComponentUsage MeshProcessingComponent::GetComponentUsage() const
{
    return Component::ComponentUsages::Class;
}


