
#pragma once

#include "MathSimd/Vector2.h"

namespace ExampleGame
{
	class AIComponentChasePlayerDefinition;
	struct AvatarControllerComponent;

	typedef Helium::StrongPtr<AIComponentChasePlayerDefinition> AIComponentChasePlayerDefinitionPtr;	
	typedef Helium::StrongPtr<const AIComponentChasePlayerDefinition> ConstAIComponentChasePlayerDefinitionPtr;

	struct EXAMPLE_GAME_API AIComponentChasePlayer : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( ExampleGame::AIComponentChasePlayer, Helium::Component );

		void Initialize( const AIComponentChasePlayerDefinition &definition);
	};

	class EXAMPLE_GAME_API AIComponentChasePlayerDefinition : public Helium::ComponentDefinitionHelper<AIComponentChasePlayer, AIComponentChasePlayerDefinition>
	{
		HELIUM_DECLARE_CLASS( ExampleGame::AIComponentChasePlayerDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );
	};

	struct EXAMPLE_GAME_API TaskProcessAI : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(TaskProcessAI)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}