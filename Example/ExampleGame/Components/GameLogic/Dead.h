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

namespace ExampleGame
{
	class DeadComponentDefinition;

	typedef Helium::StrongPtr<DeadComponentDefinition> DeadComponentDefinitionPtr;	
	typedef Helium::StrongPtr<const DeadComponentDefinition> ConstDeadComponentDefinitionPtr;

	struct EXAMPLE_GAME_API DeadComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( ExampleGame::DeadComponent, Helium::Component );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		void Initialize( float damageAmount );

		float m_DamageAmount;
	};

	class DespawnOnDeathComponentDefinition;

	struct EXAMPLE_GAME_API DespawnOnDeathComponent : public Helium::EntityComponent
	{
		HELIUM_DECLARE_COMPONENT( ExampleGame::DespawnOnDeathComponent, Helium::EntityComponent );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		void Initialize(const DespawnOnDeathComponentDefinition &definition);
	};

	class EXAMPLE_GAME_API DespawnOnDeathComponentDefinition : public Helium::ComponentDefinitionHelper<DespawnOnDeathComponent, DespawnOnDeathComponentDefinition>
	{
		HELIUM_DECLARE_CLASS( ExampleGame::DespawnOnDeathComponentDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );
	};

	struct EXAMPLE_GAME_API TaskDestroyAllDead : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(TaskDestroyAllDead)
		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}
