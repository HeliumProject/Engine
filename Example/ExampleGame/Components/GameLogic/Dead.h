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
	class DeadComponentDefinition;

	typedef Helium::StrongPtr<DeadComponentDefinition> DeadComponentDefinitionPtr;	
	typedef Helium::StrongPtr<const DeadComponentDefinition> ConstDeadComponentDefinitionPtr;

	struct EXAMPLE_GAME_API DeadComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( ExampleGame::DeadComponent, Helium::Component );
		static void PopulateStructure( Helium::Reflect::Structure& comp );

		void Initialize( float damageAmount );

		float m_DamageAmount;
	};
}
