#include "ExampleGamePch.h"

#include "DamageOnContact.h"
#include "Framework/WorldManager.h"
#include "ExampleGame/Components/GameLogic/Health.h"


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

void ApplyDamage( HasPhysicalContactsComponent *pHasPhysicalContacts, DamageOnContactComponent *pDamageOnContact )
{
	for (Set<Entity *>::Iterator iter = pHasPhysicalContacts->m_EverTouchedThisFrame.Begin();
		iter != pHasPhysicalContacts->m_EverTouchedThisFrame.End(); ++iter)
	{
		Entity *pOtherEntity = *iter;

		HealthComponent *pOtherHealthComponent = pOtherEntity->GetComponents().GetFirst<HealthComponent>();
		if ( pOtherHealthComponent )
		{
			pOtherHealthComponent->ApplyDamage( pDamageOnContact->m_DamageAmount );
		}
	}
}

HELIUM_DEFINE_TASK( ApplyDamageOnContact, (ForEachWorld< QueryComponents< HasPhysicalContactsComponent, DamageOnContactComponent, ApplyDamage > >) )

void ExampleGame::ApplyDamageOnContact::DefineContract( Helium::TaskContract &rContract )
{
	rContract.Fulfills<ExampleGame::DoDamage>();
}
