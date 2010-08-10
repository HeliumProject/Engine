#include "MeshProcessingComponent.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_CLASS( MeshProcessingComponent );

void MeshProcessingComponent::EnumerateClass( Reflect::Compositor< MeshProcessingComponent >& comp )
{
  comp.GetComposite().m_UIName = TXT( "Mesh Processing" );

  comp.AddField( &MeshProcessingComponent::m_ScalingFactor, "m_ScalingFactor" );
  comp.AddEnumerationField( &MeshProcessingComponent::m_MeshCompressionFactor, "m_MeshCompressionFactor" );
}

Component::ComponentUsage MeshProcessingComponent::GetComponentUsage() const
{
    return Component::ComponentUsages::Class;
}


