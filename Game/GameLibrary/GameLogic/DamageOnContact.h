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
	class DamageOnContactComponentDefinition;
	
	typedef Helium::StrongPtr<DamageOnContactComponentDefinition> DamageOnContactComponentDefinitionPtr;	
	typedef Helium::StrongPtr<const DamageOnContactComponentDefinition> ConstDamageOnContactComponentDefinitionPtr;
			
	struct GAME_LIBRARY_API DamageOnContactComponent : public Helium::EntityComponent
	{
		HELIUM_DECLARE_COMPONENT( GameLibrary::DamageOnContactComponent, Helium::EntityComponent );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );
		
		void Initialize( const DamageOnContactComponentDefinition &definition);

		float m_DamageAmount;
		bool m_DestroySelfOnContact;
	};
	
	class GAME_LIBRARY_API DamageOnContactComponentDefinition : public Helium::ComponentDefinitionHelper<DamageOnContactComponent, DamageOnContactComponentDefinition>
	{
		HELIUM_DECLARE_CLASS( GameLibrary::DamageOnContactComponentDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		DamageOnContactComponentDefinition();

		float m_DamageAmount;
		bool m_DestroySelfOnContact;
	};


	struct GAME_LIBRARY_API ApplyDamageOnContact : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(ApplyDamageOnContact)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}
