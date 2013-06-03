#include "ExampleGamePch.h"

#include "ExampleGame/Components/GameLogic/Player.h"
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
	m_Definition.Set( pDefinition );
}

void ExampleGame::PlayerComponent::Tick()
{
	if ( !m_Avatar.ReferencesObject() )
	{
		if ( m_RespawnDelay > 0.0f )
		{
			m_RespawnDelay -= 0.01f;
		}
		else
		{
			m_RespawnDelay = 0.0f;
			Respawn();
		}
	}
}

void ExampleGame::PlayerComponent::Respawn()
{
	// TODO: Do something here
}

HELIUM_IMPLEMENT_ASSET(ExampleGame::PlayerComponentDefinition, Components, 0);

void PlayerComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField( &PlayerComponentDefinition::m_RespawnTimeDelay, "m_RespawnTimeDelay" );
}
