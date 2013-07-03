#pragma once

#include "Reflect/Structure.h"
#include "Engine/Asset.h"
#include "Framework/ComponentDefinition.h"
#include "Foundation/DynamicArray.h"
#include "Framework/TaskScheduler.h"
#include "Framework/Entity.h"
#include "Components/TransformComponent.h"
#include "Bullet/BulletBodyComponent.h"

#include "MathSimd/Vector2.h"

namespace ExampleGame
{
	class AvatarControllerComponentDefinition;
	
	typedef Helium::StrongPtr<AvatarControllerComponentDefinition> AvatarControllerComponentDefinitionPtr;	
	typedef Helium::StrongPtr<const AvatarControllerComponentDefinition> ConstAvatarControllerComponentDefinitionPtr;
		
	struct EXAMPLE_GAME_API AvatarControllerComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( ExampleGame::AvatarControllerComponent, Helium::Component );
		static void PopulateStructure( Helium::Reflect::Structure& comp );
		
		void Finalize( const AvatarControllerComponentDefinition &definition );

		Helium::EntityDefinitionPtr m_BulletDefinition; // TODO: This needs to be put in a weapon, not a controller
		Helium::Simd::Vector3 m_AimDir;
		Helium::Simd::Vector2 m_MoveDir;
		float m_Speed;
		bool m_bShoot;

		Helium::ComponentPtr< Helium::TransformComponent > m_TransformComponent;
		Helium::ComponentPtr< Helium::BulletBodyComponent > m_PhysicsComponent;
	};
	
	class EXAMPLE_GAME_API AvatarControllerComponentDefinition : public Helium::ComponentDefinitionHelperFinalizeOnly<AvatarControllerComponent, AvatarControllerComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( ExampleGame::AvatarControllerComponentDefinition, Helium::ComponentDefinition );
		static void PopulateStructure( Helium::Reflect::Structure& comp );

		Helium::EntityDefinitionPtr m_BulletDefinition;
		float m_Speed;
	};

	// We would potentially have a separate task to make AI pump data into controller
	struct EXAMPLE_GAME_API ApplyPlayerInputToAvatarTask : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(ApplyPlayerInputToAvatarTask)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};

	struct EXAMPLE_GAME_API ControlAvatarTask : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(ControlAvatarTask)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}
