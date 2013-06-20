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
	class DamageOnContactComponentDefinition;
	
	typedef Helium::StrongPtr<DamageOnContactComponentDefinition> DamageOnContactComponentDefinitionPtr;	
	typedef Helium::StrongPtr<const DamageOnContactComponentDefinition> ConstDamageOnContactComponentDefinitionPtr;
			
	struct EXAMPLE_GAME_API DamageOnContactComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( ExampleGame::DamageOnContactComponent, Helium::Component );
		static void PopulateStructure( Helium::Reflect::Structure& comp );
		
		void Initialize( const DamageOnContactComponentDefinition &definition);

		float m_DamageAmount;
	};
	
	class EXAMPLE_GAME_API DamageOnContactComponentDefinition : public Helium::ComponentDefinitionHelper<DamageOnContactComponent, DamageOnContactComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( ExampleGame::DamageOnContactComponentDefinition, Helium::ComponentDefinition );
		static void PopulateStructure( Helium::Reflect::Structure& comp );

		float m_DamageAmount;
	};
}
