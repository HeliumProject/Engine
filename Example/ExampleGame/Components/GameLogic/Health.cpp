#include "ExampleGamePch.h"

#include "Health.h"
#include "Framework/WorldManager.h"


using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// HealthComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::HealthComponent, 128);

void HealthComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void HealthComponent::Initialize( const HealthComponentDefinition &definition )
{
	m_Health = ( definition.m_InitialHealth < 0.0f) ? definition.m_MaxHealth : definition.m_InitialHealth;
	m_MaxHealth = definition.m_MaxHealth;
}

HELIUM_IMPLEMENT_ASSET(ExampleGame::HealthComponentDefinition, Components, 0);

void HealthComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField( &HealthComponentDefinition::m_InitialHealth, "m_InitialHealth" );
	comp.AddField( &HealthComponentDefinition::m_MaxHealth, "m_MaxHealth" );
}
