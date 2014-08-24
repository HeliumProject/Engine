#pragma once

#include "Reflect/MetaStruct.h"
#include "Engine/Asset.h"
#include "Framework/ComponentDefinition.h"
#include "Foundation/DynamicArray.h"
#include "Framework/TaskScheduler.h"
#include "Framework/Entity.h"

#define EXAMPLE_GAME_MAX_WORLDS (1)
#define EXAMPLE_GAME_MAX_PLAYERS (4)

namespace GameLibrary
{
	class PlayerComponentDefinition;
	
	typedef Helium::StrongPtr<PlayerComponentDefinition> PlayerComponentDefinitionPtr;	
	typedef Helium::StrongPtr<const PlayerComponentDefinition> ConstPlayerComponentDefinition;
		
	//////////////////////////////////////////////////////////////////////////
	// PlayerComponent
	//
	// - Every player will have a world entity on which components may be attached.
	// - This component serves as a basic implementation of player state
	class GAME_LIBRARY_API PlayerComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( GameLibrary::PlayerComponent, Helium::Component );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );
		
		void Initialize( const PlayerComponentDefinition &definition);

		void Tick();
		void Respawn();

		//TODO: Respawning could be done in its own component to allow gameplay rules for selecting a spawn point
		ConstPlayerComponentDefinition m_Definition;
		Helium::EntityPtr m_Avatar;
		float m_RespawnDelay;
	};
	
	class GAME_LIBRARY_API PlayerComponentDefinition : public Helium::ComponentDefinitionHelper<PlayerComponent, PlayerComponentDefinition>
	{
		HELIUM_DECLARE_CLASS( GameLibrary::PlayerComponentDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		Helium::EntityDefinitionPtr m_AvatarEntity;
		float m_RespawnTimeDelay;
	};
}
