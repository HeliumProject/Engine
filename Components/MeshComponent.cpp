
#include "ComponentsPch.h"
#include "Components/MeshComponent.h"

HELIUM_DEFINE_COMPONENT(Helium::MeshComponent);

void Helium::MeshComponent::PopulateComposite( Reflect::Composite& comp )
{

}

void Helium::MeshComponent::Finalize( const Helium::MeshComponentDefinition* pDefinition )
{
    m_Mesh = pDefinition->m_Mesh;
}

HELIUM_IMPLEMENT_ASSET(Helium::MeshComponentDefinition, Components, 0);

void Helium::MeshComponentDefinition::PopulateComposite( Reflect::Composite& comp )
{
	comp.AddField(&MeshComponentDefinition::m_Mesh, "m_Mesh");
}