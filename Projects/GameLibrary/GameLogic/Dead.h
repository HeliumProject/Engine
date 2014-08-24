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
	class DeadComponentDefinition;

	typedef Helium::StrongPtr<DeadComponentDefinition> DeadComponentDefinitionPtr;	
	typedef Helium::StrongPtr<const DeadComponentDefinition> ConstDeadComponentDefinitionPtr;

	struct GAME_LIBRARY_API DeadComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( GameLibrary::DeadComponent, Helium::Component );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		void Initialize( float damageAmount );

		float m_DamageAmount;
	};

	class DespawnOnDeathComponentDefinition;

	struct GAME_LIBRARY_API DespawnOnDeathComponent : public Helium::EntityComponent
	{
		HELIUM_DECLARE_COMPONENT( GameLibrary::DespawnOnDeathComponent, Helium::EntityComponent );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		void Initialize(const DespawnOnDeathComponentDefinition &definition);
	};

	class GAME_LIBRARY_API DespawnOnDeathComponentDefinition : public Helium::ComponentDefinitionHelper<DespawnOnDeathComponent, DespawnOnDeathComponentDefinition>
	{
		HELIUM_DECLARE_CLASS( GameLibrary::DespawnOnDeathComponentDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );
	};

	struct GAME_LIBRARY_API TaskDestroyAllDead : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(TaskDestroyAllDead)
		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}
