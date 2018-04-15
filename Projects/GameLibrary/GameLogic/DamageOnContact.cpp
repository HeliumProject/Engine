#include "Precompile.h"

#include "DamageOnContact.h"
#include "Framework/WorldManager.h"
#include "GameLibrary/GameLogic/Health.h"
#include "Reflect/TranslatorDeduction.h"


using namespace Helium;
using namespace GameLibrary;

//////////////////////////////////////////////////////////////////////////
// DamageOnContactComponent

HELIUM_DEFINE_COMPONENT(GameLibrary::DamageOnContactComponent, 128);

void DamageOnContactComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

void DamageOnContactComponent::Initialize( const DamageOnContactComponentDefinition &definition )
{
	m_DamageAmount = definition.m_DamageAmount;
	m_DestroySelfOnContact = definition.m_DestroySelfOnContact;
}

HELIUM_DEFINE_CLASS(GameLibrary::DamageOnContactComponentDefinition);

GameLibrary::DamageOnContactComponentDefinition::DamageOnContactComponentDefinition()
	: m_DamageAmount(0.0f)
	, m_DestroySelfOnContact(true)
{
	
}

void DamageOnContactComponentDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &DamageOnContactComponentDefinition::m_DamageAmount, "m_DamageAmount" );
	comp.AddField( &DamageOnContactComponentDefinition::m_DestroySelfOnContact, "m_DestroySelfOnContact" );
}

void ApplyDamage( HasPhysicalContactsComponent *pHasPhysicalContacts, DamageOnContactComponent *pDamageOnContact )
{
	for (Set<EntityWPtr>::Iterator iter = pHasPhysicalContacts->m_EverTouchedThisFrame.Begin();
		iter != pHasPhysicalContacts->m_EverTouchedThisFrame.End(); ++iter)
	{
		Entity *pOtherEntity = *iter;

		if (!pOtherEntity)
		{
			continue;
		}

		HealthComponent *pOtherHealthComponent = pOtherEntity->GetComponents().GetFirst<HealthComponent>();
		if ( pOtherHealthComponent )
		{
			pOtherHealthComponent->ApplyDamage( pDamageOnContact->m_DamageAmount );
		}

		if ( pDamageOnContact->m_DestroySelfOnContact )
		{
			pDamageOnContact->GetEntity()->DeferredDestroy();
		}
	}
}

HELIUM_DEFINE_TASK( ApplyDamageOnContact, (ForEachWorld< QueryComponents< HasPhysicalContactsComponent, DamageOnContactComponent, ApplyDamage > >), TickTypes::Gameplay )

void GameLibrary::ApplyDamageOnContact::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecutesWithin<GameLibrary::DoDamage>();
}
