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
	class DamagedComponentDefinition;

	typedef Helium::StrongPtr<DamagedComponentDefinition> DamagedComponentDefinitionPtr;	
	typedef Helium::StrongPtr<const DamagedComponentDefinition> ConstDamagedComponentDefinitionPtr;

	struct EXAMPLE_GAME_API DamagedComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( ExampleGame::DamagedComponent, Helium::Component );
		static void PopulateStructure( Helium::Reflect::Structure& comp );

		void Initialize( float damageAmount );

		float m_DamageAmount;
	};
}
