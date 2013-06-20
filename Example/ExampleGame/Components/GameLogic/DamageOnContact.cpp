#include "ExampleGamePch.h"

#include "DamageOnContact.h"
#include "Framework/WorldManager.h"


using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// DamageOnContactComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::DamageOnContactComponent, 128);

void DamageOnContactComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void DamageOnContactComponent::Initialize( const DamageOnContactComponentDefinition &definition )
{
	m_DamageAmount = definition.m_DamageAmount;
}

HELIUM_IMPLEMENT_ASSET(ExampleGame::DamageOnContactComponentDefinition, Components, 0);

void DamageOnContactComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField( &DamageOnContactComponentDefinition::m_DamageAmount, "m_DamageAmount" );
}
