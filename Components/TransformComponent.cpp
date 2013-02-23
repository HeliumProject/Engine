
#include "ComponentsPch.h"
#include "Components/TransformComponent.h"

HELIUM_DEFINE_COMPONENT(Helium::TransformComponent);

void Helium::TransformComponent::PopulateComposite( Reflect::Composite& comp )
{

}

void Helium::TransformComponent::Finalize( const TransformComponentDefinition *pDefinition )
{

}

HELIUM_IMPLEMENT_ASSET(Helium::TransformComponentDefinition, Components, 0);

Helium::TransformComponentDefinition::TransformComponentDefinition()    
: m_Position( 0.0f )
, m_Rotation( Simd::Quat::IDENTITY )
{

}

void Helium::TransformComponentDefinition::PopulateComposite( Reflect::Composite& comp )
{
	comp.AddStructureField(&TransformComponentDefinition::m_Position, "m_Position");
	comp.AddStructureField(&TransformComponentDefinition::m_Rotation, "m_Rotation");
}
