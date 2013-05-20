#pragma once

#include "Reflect/Structure.h"
#include "Engine/Asset.h"
#include "Engine/AssetType.h"
#include "Framework/ComponentDefinition.h"
#include "Foundation/DynamicArray.h"
#include "Engine/TaskScheduler.h"
#include "Framework/Entity.h"

#define EXAMPLE_GAME_MAX_WORLDS (1)
#define EXAMPLE_GAME_MAX_PLAYERS (4)

namespace ExampleGame
{
	class PlayerComponentDefinition;
	class PlayerManagerComponentDefinition;
	
    typedef Helium::StrongPtr<PlayerComponentDefinition> PlayerComponentDefinitionPtr;
    typedef Helium::StrongPtr<PlayerManagerComponentDefinition> PlayerManagerComponentDefinitionPtr;
	
    typedef Helium::StrongPtr<const PlayerManagerComponentDefinition> ConstPlayerManagerComponentDefinitionPtr;
		
	//////////////////////////////////////////////////////////////////////////
	// PlayerComponent
	//
	// - Every player will have a world entity on which components may be attached.
	// - This component serves as a basic implementation of player state
	class EXAMPLE_GAME_API PlayerComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( ExampleGame::PlayerComponent, Helium::Component );
        static void PopulateStructure( Helium::Reflect::Structure& comp );
		
        void Finalize( const PlayerComponentDefinition *pDefinition);

		void Tick();

		struct PlayerInfo
		{
			Helium::EntityPtr m_PlayerEntity;
		};
		Helium::DynamicArray<PlayerInfo> m_Players;
	};
	
	class EXAMPLE_GAME_API PlayerComponentDefinition : public Helium::ComponentDefinitionHelper<PlayerComponent, PlayerComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( ExampleGame::PlayerComponentDefinition, Helium::ComponentDefinition );
        static void PopulateStructure( Helium::Reflect::Structure& comp );
	};

	//////////////////////////////////////////////////////////////////////////
	// PlayerManagerComponent
	//
	// - Component on world that manages creation/destruction of player entities
	class EXAMPLE_GAME_API PlayerManagerComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( ExampleGame::PlayerManagerComponent, Helium::Component );
        static void PopulateStructure( Helium::Reflect::Structure& comp );
		
        void Finalize( const PlayerManagerComponentDefinition *pDefinition);

		void Tick();

		struct PlayerInfo
		{
			Helium::EntityPtr m_PlayerEntity;
		};
		Helium::DynamicArray<PlayerInfo> m_Players;

		ConstPlayerManagerComponentDefinitionPtr m_Definition;
	};
	
	class EXAMPLE_GAME_API PlayerManagerComponentDefinition : public Helium::ComponentDefinitionHelper<PlayerManagerComponent, PlayerManagerComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( ExampleGame::PlayerManagerComponentDefinition, Helium::ComponentDefinition );
        static void PopulateStructure( Helium::Reflect::Structure& comp );

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
