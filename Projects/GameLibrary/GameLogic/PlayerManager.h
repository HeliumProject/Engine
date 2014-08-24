#pragma once

#include "GameLibrary/GameLogic/Player.h"

#define EXAMPLE_GAME_MAX_WORLDS (1)
#define EXAMPLE_GAME_MAX_PLAYERS (4)

namespace GameLibrary
{
	class PlayerManagerComponentDefinition;
	
	typedef Helium::StrongPtr<PlayerManagerComponentDefinition> PlayerManagerComponentDefinitionPtr;
	typedef Helium::StrongPtr<const PlayerManagerComponentDefinition> ConstPlayerManagerComponentDefinitionPtr;
		
	struct PlayerInfo
	{
		Helium::EntityWPtr m_PlayerEntity;
	};

	//////////////////////////////////////////////////////////////////////////
	// PlayerManagerComponent
	//
	// - Component on world that manages creation/destruction of player entities
	class GAME_LIBRARY_API PlayerManagerComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( GameLibrary::PlayerManagerComponent, Helium::Component );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );
		
		void Initialize( const PlayerManagerComponentDefinition &definition);

		void Tick();

		PlayerInfo &GetPlayerInfo(int playerIndex) { return m_Players[playerIndex]; }

		Helium::DynamicArray<PlayerInfo> m_Players;

		ConstPlayerManagerComponentDefinitionPtr m_Definition;
	};
	
	class GAME_LIBRARY_API PlayerManagerComponentDefinition : public Helium::ComponentDefinitionHelper<PlayerManagerComponent, PlayerManagerComponentDefinition>
	{
		HELIUM_DECLARE_CLASS( GameLibrary::PlayerManagerComponentDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		Helium::EntityDefinitionPtr m_PlayerEntity;
	};

	//////////////////////////////////////////////////////////////////////////
	// PlayerManagerTick
	//
	// - Ticks all PlayerManagerComponents
	struct GAME_LIBRARY_API PlayerManagerTick : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(PlayerManagerTick)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}
