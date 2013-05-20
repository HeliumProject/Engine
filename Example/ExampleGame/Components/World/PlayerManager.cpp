#include "ExampleGamePch.h"

#include "ExampleGame/Components/World/PlayerManager.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// PlayerComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::PlayerComponent, EXAMPLE_GAME_MAX_PLAYERS * EXAMPLE_GAME_MAX_WORLDS);

void PlayerComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void PlayerComponent::Finalize( const PlayerComponentDefinition *pDefinition )
{

}

void ExampleGame::PlayerComponent::Tick()
{
	for (DynamicArray<PlayerInfo>::Iterator iter = m_Players.Begin();
		iter != m_Players.End(); ++iter)
	{
		//if ( !iter->m_Avatar.ReferencesObject() )
		//{
		//	if ( iter->m_RespawnDelay > 0.0f )
		//	{
		//		//iter->m_RespawnDelay -= GameSystem::Get
		//	}
		//	else
		//	{

		//	}
		//}
		
		//if ( !iter->m_Avatar.ReferencesObject() )
		//{
		//	if ( iter->m_RespawnDelay > 0.0f )
		//	{
		//		//iter->m_RespawnDelay -= GameSystem::Get
		//	}
		//	else
		//	{

		//	}
		//}


	}
}

HELIUM_IMPLEMENT_ASSET(ExampleGame::PlayerComponentDefinition, Components, 0);

void PlayerComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{

}


//////////////////////////////////////////////////////////////////////////
// PlayerManagerComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::PlayerManagerComponent, EXAMPLE_GAME_MAX_WORLDS);

void PlayerManagerComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void PlayerManagerComponent::Finalize( const PlayerManagerComponentDefinition *pDefinition )
{
	m_Definition.Set( pDefinition );
}

void ExampleGame::PlayerManagerComponent::Tick()
{
	// Create default player when we can
	if ( m_Players.IsEmpty() && m_OwningSet->GetWorld()->GetRootSlice() )
	{
		Entity *pPlayerEntity = m_OwningSet->GetWorld()->GetRootSlice()->CreateEntity( m_Definition->m_PlayerEntity );
		PlayerInfo &rPlayer = *m_Players.New();
		rPlayer.m_PlayerEntity = pPlayerEntity;
	}
}

HELIUM_IMPLEMENT_ASSET(ExampleGame::PlayerManagerComponentDefinition, Components, 0);

void PlayerManagerComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField( &PlayerManagerComponentDefinition::m_PlayerEntity, "m_PlayerEntity" );
}

//////////////////////////////////////////////////////////////////////////
// PlayerManagerTick

void TickPlayers()
{
	{
		DynamicArray<PlayerManagerComponent *> components;
		Helium::Components::GetAllComponents<PlayerManagerComponent>( components );

		for (DynamicArray<PlayerManagerComponent *>::Iterator iter = components.Begin();
			iter != components.End(); ++iter)
		{
			(*iter)->Tick();
		}
	}
		
	{
		DynamicArray<PlayerComponent *> components;
		Helium::Components::GetAllComponents<PlayerComponent>( components );

		for (DynamicArray<PlayerComponent *>::Iterator iter = components.Begin();
			iter != components.End(); ++iter)
		{
			(*iter)->Tick();
		}
	}
}

HELIUM_DEFINE_TASK(PlayerManagerTick, TickPlayers)
	
void PlayerManagerTick::DefineContract( TaskContract &rContract )
{
	rContract.ExecuteBefore<Helium::StandardDependencies::ReceiveInput>();
}