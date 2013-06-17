#include "ExampleGamePch.h"

#include "PlayerInput.h"
#include "Ois/OisSystem.h"

using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// PlayerInputComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::PlayerInputComponent, EXAMPLE_GAME_MAX_PLAYERS * EXAMPLE_GAME_MAX_WORLDS);

void PlayerInputComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void PlayerInputComponent::Initialize( const PlayerInputComponentDefinition &definition )
{
	//m_Definition.Set( definition );
}

HELIUM_IMPLEMENT_ASSET(ExampleGame::PlayerInputComponentDefinition, Components, 0);

void PlayerInputComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	
}

void GatherInput( PlayerInputComponent *pPlayerInput )
{
	float x = 0.0f;
	float y = 0.0f;

	if ( Helium::Input::IsKeyDown(Input::KeyCodes::KC_LEFT) )
	{
		x -= 1.0f;
	}
	
	if ( Helium::Input::IsKeyDown(Input::KeyCodes::KC_RIGHT) )
	{
		x += 1.0f;
	}

	if ( Helium::Input::IsKeyDown(Input::KeyCodes::KC_UP) )
	{
		y += 1.0f;
	}

	if ( Helium::Input::IsKeyDown(Input::KeyCodes::KC_DOWN) )
	{
		y -= 1.0f;
	}
	
	pPlayerInput->m_AimDir = Helium::Input::GetMousePosNormalized();
	pPlayerInput->m_AimDir.Normalize();

	pPlayerInput->m_MoveDir = Simd::Vector2(x, y);
	pPlayerInput->m_MoveDir.NormalizeOrZero();
}

HELIUM_DEFINE_TASK( GatherInputForPlayers, (ForEachWorld< QueryComponents< PlayerInputComponent, GatherInput > >) )

void ExampleGame::GatherInputForPlayers::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecuteAfter<Helium::StandardDependencies::ReceiveInput>();
}
