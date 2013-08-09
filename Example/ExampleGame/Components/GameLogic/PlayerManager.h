#pragma once

#include "ExampleGame/Components/GameLogic/Player.h"

#define EXAMPLE_GAME_MAX_WORLDS (1)
#define EXAMPLE_GAME_MAX_PLAYERS (4)

namespace ExampleGame
{
	class PlayerManagerComponentDefinition;
	
	typedef Helium::StrongPtr<PlayerManagerComponentDefinition> PlayerManagerComponentDefinitionPtr;
	typedef Helium::StrongPtr<const PlayerManagerComponentDefinition> ConstPlayerManagerComponentDefinitionPtr;
		
	//////////////////////////////////////////////////////////////////////////
	// PlayerManagerComponent
	//
	// - Component on world that manages creation/destruction of player entities
	class EXAMPLE_GAME_API PlayerManagerComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( ExampleGame::PlayerManagerComponent, Helium::Component );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );
		
		void Initialize( const PlayerManagerComponentDefinition &definition);

		void Tick();

		struct PlayerInfo
		{
			Helium::EntityWPtr m_PlayerEntity;
		};
		Helium::DynamicArray<PlayerInfo> m_Players;

		ConstPlayerManagerComponentDefinitionPtr m_Definition;
	};
	
	class EXAMPLE_GAME_API PlayerManagerComponentDefinition : public Helium::ComponentDefinitionHelper<PlayerManagerComponent, PlayerManagerComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( ExampleGame::PlayerManagerComponentDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		Helium::EntityDefinitionPtr m_PlayerEntity;
	};

	//////////////////////////////////////////////////////////////////////////
	// PlayerManagerTick
	//
	// - Ticks all PlayerManagerComponents
	struct EXAMPLE_GAME_API PlayerManagerTick : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(PlayerManagerTick)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}
