#include "GameLibraryPch.h"

#include "Game/GameLibrary/GameLogic/Player.h"
#include "Reflect/TranslatorDeduction.h"
#include "Framework/World.h"

#include "Game/GameLibrary/GameLogic/PlayerInput.h"

using namespace Helium;
using namespace GameLibrary;

//////////////////////////////////////////////////////////////////////////
// PlayerComponent

HELIUM_DEFINE_COMPONENT(GameLibrary::PlayerComponent, EXAMPLE_GAME_MAX_PLAYERS * EXAMPLE_GAME_MAX_WORLDS);

void PlayerComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

void PlayerComponent::Initialize( const PlayerComponentDefinition &definition )
{
	m_Definition.Set( &definition );
}

void PlayerComponent::Tick()
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

void PlayerComponent::Respawn()
{
	if ( GetWorld()->GetRootSlice() )
	{
		m_Avatar = GetWorld()->GetRootSlice()->CreateEntity( m_Definition->m_AvatarEntity );
		m_Avatar->Allocate<PlayerInputComponent>();
	}
}

HELIUM_DEFINE_CLASS(GameLibrary::PlayerComponentDefinition);

void PlayerComponentDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &PlayerComponentDefinition::m_RespawnTimeDelay, "m_RespawnTimeDelay" );
	comp.AddField( &PlayerComponentDefinition::m_AvatarEntity, "m_AvatarEntity" );
}
