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
	class HealthComponentDefinition;
	
	typedef Helium::StrongPtr<HealthComponentDefinition> HealthComponentDefinitionPtr;	
	typedef Helium::StrongPtr<const HealthComponentDefinition> ConstHealthComponentDefinitionPtr;
			
	struct EXAMPLE_GAME_API HealthComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( ExampleGame::HealthComponent, Helium::Component );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );
		
		void Initialize( const HealthComponentDefinition &definition);
		void ApplyDamage( float m_DamageAmount );

		float m_Health;
		float m_MaxHealth;
		bool m_CreatedDeadComponent;
	};
	
	class EXAMPLE_GAME_API HealthComponentDefinition : public Helium::ComponentDefinitionHelper<HealthComponent, HealthComponentDefinition>
	{
		HELIUM_DECLARE_CLASS( ExampleGame::HealthComponentDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		float m_InitialHealth;
		float m_MaxHealth;
	};

	struct EXAMPLE_GAME_API DoDamage : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(DoDamage);
		virtual void DefineContract(Helium::TaskContract &r);
	};

	struct EXAMPLE_GAME_API KillAllWithZeroHealth : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(KillAllWithZeroHealth)
		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}
