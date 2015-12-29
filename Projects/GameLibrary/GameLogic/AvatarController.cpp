#include "GameLibraryPch.h"

#include "AvatarController.h"
#include "Ois/OisSystem.h"
#include "Framework/WorldManager.h"
#include "Framework/ParameterSet.h"


#include "GameLibrary/GameLogic/PlayerInput.h"


// TEMP
#include "GameLibrary/GameLogic/EnemyWaveManager.h"

using namespace Helium;
using namespace GameLibrary;

//////////////////////////////////////////////////////////////////////////
// AvatarControllerComponent

HELIUM_DEFINE_COMPONENT(GameLibrary::AvatarControllerComponent, EXAMPLE_GAME_MAX_PLAYERS * EXAMPLE_GAME_MAX_WORLDS);

void AvatarControllerComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

void AvatarControllerComponent::Finalize( const AvatarControllerComponentDefinition &definition )
{
	m_Definition = &definition;
	m_bShoot = false;
	m_AimDir = Simd::Vector3::Zero;
	m_MoveDir = Simd::Vector2::Zero;

	m_TransformComponent = GetComponentCollection()->GetFirst<TransformComponent>();
	m_PhysicsComponent = GetComponentCollection()->GetFirst<BulletBodyComponent>();
	HELIUM_ASSERT( m_TransformComponent.Get() );
	HELIUM_ASSERT( m_PhysicsComponent.Get() );
}

HELIUM_DEFINE_CLASS(GameLibrary::AvatarControllerComponentDefinition);

void AvatarControllerComponentDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &AvatarControllerComponentDefinition::m_Speed, "m_Speed" );
	comp.AddField( &AvatarControllerComponentDefinition::m_FireRepeatDelay, "m_FireRepeatDelay" );
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

HELIUM_DEFINE_TASK( ApplyPlayerInputToAvatarTask, (ForEachWorld< QueryComponents< PlayerInputComponent, AvatarControllerComponent, ApplyPlayerInputToAvatar > >), TickTypes::Gameplay )

void GameLibrary::ApplyPlayerInputToAvatarTask::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecuteAfter<GameLibrary::GatherInputForPlayers>();
}

//////////////////////////////////////////////////////////////////////////

void ControlAvatar( AvatarControllerComponent *pController )
{
	HELIUM_ASSERT( pController->m_TransformComponent.Get() );
	const Simd::Vector3 &currentPosition = pController->m_TransformComponent->GetPosition();

	Simd::Vector3 movement(
		pController->m_MoveDir.GetX() * pController->m_Definition->m_Speed, 
		pController->m_MoveDir.GetY() * pController->m_Definition->m_Speed, 
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

	if ( pController->m_ShootCooldown > 0.0f )
	{
		pController->m_ShootCooldown -= WorldManager::GetInstance().GetFrameDeltaSeconds();
	}
	else if ( pController->m_bShoot )
	{
		EntityDefinition* shotDefinition = pController->m_Definition->m_BulletDefinition;
		if ( shotDefinition )
		{
			Simd::Vector3 bulletOrigin = pController->m_TransformComponent->GetPosition() + pController->m_AimDir * 45.0f;
			Simd::Vector3 bulletVelocity = pController->m_AimDir * 400.0f;

			ParameterSetBuilder builder;
			ParameterSet_InitLocated *pInitLocated = builder.AddParameterSet<ParameterSet_InitLocated>();
			pInitLocated->m_Position = bulletOrigin;

			ParameterSet_InitPhysical *pInitPhysical = builder.AddParameterSet<ParameterSet_InitPhysical>();
			pInitPhysical->m_Velocity = bulletVelocity;

			pController->GetWorld()->GetRootSlice()->CreateEntity(shotDefinition, builder.GetSet());

			pController->m_ShootCooldown = pController->m_Definition->m_FireRepeatDelay;
		}
	}
}

HELIUM_DEFINE_TASK( ControlAvatarTask, (ForEachWorld< QueryComponents< AvatarControllerComponent, ControlAvatar > >), TickTypes::Gameplay )

void GameLibrary::ControlAvatarTask::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecuteAfter<GameLibrary::ApplyPlayerInputToAvatarTask>();
	rContract.ExecuteBefore<Helium::StandardDependencies::ProcessPhysics>();
}
