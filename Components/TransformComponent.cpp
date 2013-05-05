#include "ComponentsPch.h"
#include "Components/TransformComponent.h"
#include "Reflect/TranslatorDeduction.h"

HELIUM_DEFINE_COMPONENT(Helium::TransformComponent, 128);

void Helium::TransformComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void Helium::TransformComponent::Finalize( const TransformComponentDefinition *pDefinition )
{
	m_Position = pDefinition->m_Position;
	m_Rotation = pDefinition->m_Rotation;
	m_bDirty = true;
}

HELIUM_IMPLEMENT_ASSET(Helium::TransformComponentDefinition, Components, 0);

Helium::TransformComponentDefinition::TransformComponentDefinition()    
: m_Position( 0.0f )
, m_Rotation( Simd::Quat::IDENTITY )
{

}

void Helium::TransformComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField(&TransformComponentDefinition::m_Position, "m_Position");
	comp.AddField(&TransformComponentDefinition::m_Rotation, "m_Rotation");
}
