#pragma once

#include "Engine/Asset.h"
#include "Framework/ComponentDefinition.h"
#include "Framework/TaskScheduler.h"
#include "Framework/StateMachine.h"

namespace ExampleGame
{
	class ShapeShooterComponentDefinition;

	typedef Helium::StrongPtr<ShapeShooterComponentDefinition> ShapeShooterComponentDefinitionPtr;	
	typedef Helium::StrongPtr<const ShapeShooterComponentDefinition> ConstShapeShooterComponentDefinitionPtr;

	struct ShapeShooterComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( ExampleGame::ShapeShooterComponent, Helium::Component );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		void Initialize( const ShapeShooterComponentDefinition &definition);
		void Tick();

		Helium::StateMachineInstance m_StateMachine;
	};

	class ShapeShooterComponentDefinition : public Helium::ComponentDefinitionHelper<ShapeShooterComponent, ShapeShooterComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( ExampleGame::ShapeShooterComponentDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		Helium::StateMachineDefinitionPtr m_StateMachine;
	};

	struct TickShapeShooter : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(TickShapeShooter)
		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}
