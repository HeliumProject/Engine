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
	class HealthComponentDefinition;
	
	typedef Helium::StrongPtr<HealthComponentDefinition> HealthComponentDefinitionPtr;	
	typedef Helium::StrongPtr<const HealthComponentDefinition> ConstHealthComponentDefinitionPtr;
			
	struct EXAMPLE_GAME_API HealthComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( ExampleGame::HealthComponent, Helium::Component );
		static void PopulateStructure( Helium::Reflect::Structure& comp );
		
		void Initialize( const HealthComponentDefinition &definition);

		float m_Health;
		float m_MaxHealth;
	};
	
	class EXAMPLE_GAME_API HealthComponentDefinition : public Helium::ComponentDefinitionHelper<HealthComponent, HealthComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( ExampleGame::HealthComponentDefinition, Helium::ComponentDefinition );
		static void PopulateStructure( Helium::Reflect::Structure& comp );

		float m_InitialHealth;
		float m_MaxHealth;
	};
}
