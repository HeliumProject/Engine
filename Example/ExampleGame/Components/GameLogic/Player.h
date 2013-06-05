#pragma once

#include "Reflect/Structure.h"
#include "Engine/Asset.h"
#include "Engine/AssetType.h"
#include "Framework/ComponentDefinition.h"
#include "Foundation/DynamicArray.h"
#include "Framework/TaskScheduler.h"
#include "Framework/Entity.h"

#define EXAMPLE_GAME_MAX_WORLDS (1)
#define EXAMPLE_GAME_MAX_PLAYERS (4)

namespace ExampleGame
{
	class PlayerComponentDefinition;
	
    typedef Helium::StrongPtr<PlayerComponentDefinition> PlayerComponentDefinitionPtr;	
    typedef Helium::StrongPtr<const PlayerComponentDefinition> ConstPlayerComponentDefinition;
		
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
		void Respawn();

		//TODO: Respawning could be done in its own component to allow gameplay rules for selecting a spawn point
		ConstPlayerComponentDefinition m_Definition;
		Helium::EntityPtr m_Avatar;
		float m_RespawnDelay;
	};
	
	class EXAMPLE_GAME_API PlayerComponentDefinition : public Helium::ComponentDefinitionHelper<PlayerComponent, PlayerComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( ExampleGame::PlayerComponentDefinition, Helium::ComponentDefinition );
        static void PopulateStructure( Helium::Reflect::Structure& comp );

		float m_RespawnTimeDelay;
	};
}
