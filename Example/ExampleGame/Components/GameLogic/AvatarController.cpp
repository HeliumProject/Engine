#include "ExampleGamePch.h"

#include "AvatarController.h"
#include "Ois/OisSystem.h"
#include "Framework/WorldManager.h"


#include "ExampleGame/Components/GameLogic/PlayerInput.h"

using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// AvatarControllerComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::AvatarControllerComponent, EXAMPLE_GAME_MAX_PLAYERS * EXAMPLE_GAME_MAX_WORLDS);

void AvatarControllerComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void AvatarControllerComponent::Initialize( const AvatarControllerComponentDefinition &definition )
{
	//m_Definition.Set( definition );
	m_TransformComponent = GetComponentCollection()->GetFirst<TransformComponent>();
	m_PhysicsComponent = GetComponentCollection()->GetFirst<BulletBodyComponent>();
	m_Speed = definition.m_Speed;
	HELIUM_ASSERT( m_TransformComponent.Get() );
	HELIUM_ASSERT( m_PhysicsComponent.Get() );
}

HELIUM_IMPLEMENT_ASSET(ExampleGame::AvatarControllerComponentDefinition, Components, 0);

void AvatarControllerComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField( &AvatarControllerComponentDefinition::m_Speed, "m_Speed" );
}

//////////////////////////////////////////////////////////////////////////

void ApplyPlayerInputToAvatar( PlayerInputComponent *pPlayerInput, AvatarControllerComponent *pController )
{
	pController->m_AimDir = pPlayerInput->m_AimDir;
	pController->m_MoveDir = pPlayerInput->m_MoveDir;
}

HELIUM_DEFINE_TASK( ApplyPlayerInputToAvatarTask, (ForEachWorld< QueryComponents< PlayerInputComponent, AvatarControllerComponent, ApplyPlayerInputToAvatar > >) )

void ExampleGame::ApplyPlayerInputToAvatarTask::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecuteAfter<ExampleGame::GatherInputForPlayers>();
}

//////////////////////////////////////////////////////////////////////////

void ControlAvatar( AvatarControllerComponent *pController )
{
	HELIUM_ASSERT( pController->m_TransformComponent.Get() );
	const Simd::Vector3 &currentPosition = pController->m_TransformComponent->GetPosition();

	Simd::Vector3 movement(
		pController->m_MoveDir.GetX() * pController->m_Speed, 
		pController->m_MoveDir.GetY() * pController->m_Speed, 
		0.0f);

	//pController->m_PhysicsComponent.Impulse()
	if (movement.GetMagnitudeSquared() > 0.0f)
	{
		pController->m_PhysicsComponent->Impulse();
	}

	Simd::Vector3( WorldManager::GetStaticInstance().GetFrameDeltaSeconds() );

	pController->m_TransformComponent->SetPosition( currentPosition + movement );
}

HELIUM_DEFINE_TASK( ControlAvatarTask, (ForEachWorld< QueryComponents< AvatarControllerComponent, ControlAvatar > >) )

void ExampleGame::ControlAvatarTask::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecuteAfter<ExampleGame::ApplyPlayerInputToAvatarTask>();
}
