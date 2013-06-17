#include "ExampleGamePch.h"

#include "ExampleGame/Components/GameLogic/Player.h"
#include "Reflect/TranslatorDeduction.h"

#include "ExampleGame/Components/GameLogic/PlayerInput.h"

using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// PlayerComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::PlayerComponent, EXAMPLE_GAME_MAX_PLAYERS * EXAMPLE_GAME_MAX_WORLDS);

void PlayerComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void PlayerComponent::Initialize( const PlayerComponentDefinition &definition )
{
	m_Definition.Set( &definition );
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
	if ( GetWorld()->GetRootSlice() )
	{
		m_Avatar = GetWorld()->GetRootSlice()->CreateEntity( m_Definition->m_AvatarEntity );
		m_Avatar->Allocate<PlayerInputComponent>();
	}
}

HELIUM_IMPLEMENT_ASSET(ExampleGame::PlayerComponentDefinition, Components, 0);

void PlayerComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField( &PlayerComponentDefinition::m_RespawnTimeDelay, "m_RespawnTimeDelay" );
	comp.AddField( &PlayerComponentDefinition::m_AvatarEntity, "m_AvatarEntity" );
}
