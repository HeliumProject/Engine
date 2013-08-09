#pragma once

#include "Reflect/MetaStruct.h"
#include "Engine/Asset.h"
#include "Framework/ComponentDefinition.h"
#include "Foundation/DynamicArray.h"
#include "Framework/TaskScheduler.h"
#include "Framework/Entity.h"

#include "MathSimd/Vector2.h"

namespace ExampleGame
{
	class PlayerInputComponentDefinition;
	
	typedef Helium::StrongPtr<PlayerInputComponentDefinition> PlayerInputComponentDefinitionPtr;	
	typedef Helium::StrongPtr<const PlayerInputComponentDefinition> ConstPlayerInputComponentDefinitionPtr;
		
	struct EXAMPLE_GAME_API PlayerInputComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( ExampleGame::PlayerInputComponent, Helium::Component );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );
		
		void Initialize( const PlayerInputComponentDefinition &definition);

		// Normalized screen coordinates where mouse is
		Helium::Simd::Vector2 m_ScreenSpaceFocusPosition;
		Helium::Simd::Vector3 m_WorldSpaceFocusPosition;

		// 
		Helium::Simd::Vector2 m_MoveDir;

		bool m_bFirePrimary;
		bool m_bHasScreenSpaceFocus;
		bool m_bHasWorldSpaceFocus;

		// TODO: This could define preferences for what keys are used for certain inputs
		//ConstPlayerInputComponentDefinitionPtr m_Definition;
	};
	
	class EXAMPLE_GAME_API PlayerInputComponentDefinition : public Helium::ComponentDefinitionHelper<PlayerInputComponent, PlayerInputComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( ExampleGame::PlayerInputComponentDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );
	};

	struct EXAMPLE_GAME_API GatherInputForPlayers : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(GatherInputForPlayers)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}
