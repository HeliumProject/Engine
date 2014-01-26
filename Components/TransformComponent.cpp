#include "ComponentsPch.h"
#include "Components/TransformComponent.h"
#include "Reflect/TranslatorDeduction.h"

HELIUM_DEFINE_COMPONENT(Helium::TransformComponent, 128);

void Helium::TransformComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

void Helium::TransformComponent::Initialize( const TransformComponentDefinition &definition )
{
	m_Position = definition.m_Position;
	m_Rotation = definition.m_Rotation;
	m_bDirty = true;
}

HELIUM_DEFINE_CLASS(Helium::TransformComponentDefinition);

Helium::TransformComponentDefinition::TransformComponentDefinition()
: m_Position( 0.0f )
, m_Rotation( Simd::Quat::IDENTITY )
{

}

void Helium::TransformComponentDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField(&TransformComponentDefinition::m_Position, "m_Position");
	comp.AddField(&TransformComponentDefinition::m_Rotation, "m_Rotation");
}
