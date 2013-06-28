#include "ExampleGamePch.h"

#include "AvatarController.h"
#include "Ois/OisSystem.h"
#include "Framework/WorldManager.h"
#include "Framework/ParameterSet.h"


#include "ExampleGame/Components/GameLogic/PlayerInput.h"

using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// AvatarControllerComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::AvatarControllerComponent, EXAMPLE_GAME_MAX_PLAYERS * EXAMPLE_GAME_MAX_WORLDS);

void AvatarControllerComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void AvatarControllerComponent::Finalize( const AvatarControllerComponentDefinition &definition )
{
	m_Speed = definition.m_Speed;
	m_BulletDefinition = definition.m_BulletDefinition;

	m_TransformComponent = GetComponentCollection()->GetFirst<TransformComponent>();
	m_PhysicsComponent = GetComponentCollection()->GetFirst<BulletBodyComponent>();
	HELIUM_ASSERT( m_TransformComponent.Get() );
	HELIUM_ASSERT( m_PhysicsComponent.Get() );
}

HELIUM_IMPLEMENT_ASSET(ExampleGame::AvatarControllerComponentDefinition, Components, 0);

void AvatarControllerComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField( &AvatarControllerComponentDefinition::m_Speed, "m_Speed" );
	comp.AddField( &AvatarControllerComponentDefinition::m_BulletDefinition, "m_BulletDefinition" );
}

//////////////////////////////////////////////////////////////////////////

void ApplyPlayerInputToAvatar( PlayerInputComponent *pPlayerInput, AvatarControllerComponent *pController )
{
	if (pPlayerInput->m_bHasWorldSpaceFocus)
	{
		pController->m_AimDir = pPlayerInput->m_WorldSpaceFocusPosition - pController->m_TransformComponent->GetPosition();
		pController->m_AimDir.SetElement(2, 0.0f);
		pController->m_AimDir.Normalize();
	}
	
	pController->m_MoveDir = pPlayerInput->m_MoveDir;
	pController->m_bShoot = pPlayerInput->m_bFirePrimary;
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

#define MOVEMENT_USE_FORCE 1
#define MOVEMENT_USE_VELOCITY 0
#define MOVEMENT_USE_KINEMATIC 0

#if MOVEMENT_USE_FORCE

	// Works perfectly with dynamic character controller but is obviously very indirect
	pController->m_PhysicsComponent->ApplyForce( movement );

#elif MOVEMENT_USE_VELOCITY

	// Velocity approach. Would work decently with dynamic character controller if there wasn't so much jitter against walls. Maybe fixable?
	pController->m_PhysicsComponent->SetVelocity( movement );


#elif MOVEMENT_USE_KINEMATIC

	// Kinematic approach - set position. Requires custom code to not walk through walls, walk up/down stairs, etc. 
	// Still not sure how to deal with dynamic objects trapped between static and kinematic objects
	{
		const Simd::Vector3 pos = pController->m_TransformComponent->GetPosition();
		Simd::Vector3 movement2 = movement * Simd::Vector3(0.008f);
		Simd::Vector3 new_pos = pos + movement2;
		pController->m_TransformComponent->SetPosition(new_pos);
		pController->m_PhysicsComponent->WakeUp();
	}

#else
	int array[-1];
#endif

#undef MOVEMENT_USE_FORCE
#undef MOVEMENT_USE_VELOCITY
#undef MOVEMENT_USE_KINEMATIC

	if ( pController->m_bShoot )
	{
		HELIUM_TRACE(
			TraceLevels::Debug,
			"SHOOT %f %f %f\n",
			pController->m_AimDir.GetElement(0),
			pController->m_AimDir.GetElement(1),
			pController->m_AimDir.GetElement(2));

		Simd::Vector3 bulletOrigin = pController->m_TransformComponent->GetPosition() + pController->m_AimDir * 45.0f;
		Simd::Vector3 bulletVelocity = pController->m_AimDir * 400.0f;

		ParameterSet paramSet;
		paramSet.SetParameter(ParameterSet::ParameterNamePosition, bulletOrigin);
		paramSet.SetParameter(ParameterSet::ParameterNameVelocity, bulletVelocity);
		pController->GetWorld()->GetRootSlice()->CreateEntity(pController->m_BulletDefinition, &paramSet);
	}
}

HELIUM_DEFINE_TASK( ControlAvatarTask, (ForEachWorld< QueryComponents< AvatarControllerComponent, ControlAvatar > >) )

void ExampleGame::ControlAvatarTask::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecuteAfter<ExampleGame::ApplyPlayerInputToAvatarTask>();
	rContract.ExecuteBefore<Helium::StandardDependencies::ProcessPhysics>();
}
