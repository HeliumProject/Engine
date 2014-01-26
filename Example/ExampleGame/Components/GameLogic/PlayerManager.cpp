#include "ExampleGamePch.h"

#include "ExampleGame/Components/GameLogic/PlayerManager.h"
#include "Reflect/TranslatorDeduction.h"
#include "Framework/World.h"

using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// PlayerManagerComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::PlayerManagerComponent, EXAMPLE_GAME_MAX_WORLDS);

void PlayerManagerComponent::PopulateMetaType( Reflect::MetaStruct& comp )
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

HELIUM_DEFINE_CLASS(ExampleGame::PlayerManagerComponentDefinition);

void PlayerManagerComponentDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
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

HELIUM_DEFINE_TASK(PlayerManagerTick, ( ForEachWorld< TickPlayers > ), TickTypes::Gameplay )
	
void PlayerManagerTick::DefineContract( TaskContract &rContract )
{
	rContract.ExecuteBefore<Helium::StandardDependencies::ReceiveInput>();
}