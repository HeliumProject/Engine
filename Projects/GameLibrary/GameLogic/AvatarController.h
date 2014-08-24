#pragma once

#include "Reflect/MetaStruct.h"
#include "Engine/Asset.h"
#include "Framework/ComponentDefinition.h"
#include "Foundation/DynamicArray.h"
#include "Framework/TaskScheduler.h"
#include "Framework/Entity.h"
#include "Components/TransformComponent.h"
#include "Bullet/BulletBodyComponent.h"

#include "MathSimd/Vector2.h"

namespace GameLibrary
{
	class AvatarControllerComponentDefinition;
	
	typedef Helium::StrongPtr<AvatarControllerComponentDefinition> AvatarControllerComponentDefinitionPtr;	
	typedef Helium::StrongPtr<const AvatarControllerComponentDefinition> ConstAvatarControllerComponentDefinitionPtr;
		
	struct GAME_LIBRARY_API AvatarControllerComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( GameLibrary::AvatarControllerComponent, Helium::Component );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );
		
		void Finalize( const AvatarControllerComponentDefinition &definition );

		Helium::StrongPtr< const AvatarControllerComponentDefinition > m_Definition;

		Helium::Simd::Vector3 m_AimDir;
		Helium::Simd::Vector2 m_MoveDir;
		bool m_bShoot;

		Helium::ComponentPtr< Helium::TransformComponent > m_TransformComponent;
		Helium::ComponentPtr< Helium::BulletBodyComponent > m_PhysicsComponent;

		float m_ShootCooldown;
	};
	
	class GAME_LIBRARY_API AvatarControllerComponentDefinition : public Helium::ComponentDefinitionHelperFinalizeOnly<AvatarControllerComponent, AvatarControllerComponentDefinition>
	{
		HELIUM_DECLARE_CLASS( GameLibrary::AvatarControllerComponentDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		Helium::EntityDefinitionPtr m_BulletDefinition;
		float m_Speed;
		float m_FireRepeatDelay;
	};

	// We would potentially have a separate task to make AI pump data into controller
	struct GAME_LIBRARY_API ApplyPlayerInputToAvatarTask : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(ApplyPlayerInputToAvatarTask)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};

	struct GAME_LIBRARY_API ControlAvatarTask : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(ControlAvatarTask)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}
