#include "Precompile.h"

#include "GameLibrary/GameLogic/PlayerManager.h"
#include "Reflect/TranslatorDeduction.h"
#include "Framework/World.h"

using namespace Helium;
using namespace GameLibrary;

//////////////////////////////////////////////////////////////////////////
// PlayerManagerComponent

HELIUM_DEFINE_COMPONENT(GameLibrary::PlayerManagerComponent, EXAMPLE_GAME_MAX_WORLDS);

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

HELIUM_DEFINE_CLASS(GameLibrary::PlayerManagerComponentDefinition);

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