#include "ExampleGamePch.h"

#include "ExampleGame/Components/GameLogic/PlayerManager.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// PlayerManagerComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::PlayerManagerComponent, EXAMPLE_GAME_MAX_WORLDS);

void PlayerManagerComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void PlayerManagerComponent::Initialize( const PlayerManagerComponentDefinition &definition )
{
	m_Definition.Set( &definition );
}

void PlayerManagerComponent::Tick()
{
	// Create default player when we can
	if ( m_Players.IsEmpty() && GetWorld()->GetRootSlice() )
	{
		Entity *pPlayerEntity = GetWorld()->GetRootSlice()->CreateEntity( m_Definition->m_PlayerEntity );
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

void TickPlayerManager(PlayerManagerComponent *c)
{
	c->Tick();
}

void TickPlayer(PlayerComponent *c)
{
	c->Tick();
}

void TickPlayers(World *pWorld)
{
	QueryComponents< PlayerManagerComponent, TickPlayerManager >( pWorld );
	QueryComponents< PlayerComponent, TickPlayer >( pWorld );
}

HELIUM_DEFINE_TASK(PlayerManagerTick, ( ForEachWorld< TickPlayers > ) )
	
void PlayerManagerTick::DefineContract( TaskContract &rContract )
{
	rContract.ExecuteBefore<Helium::StandardDependencies::ReceiveInput>();
}