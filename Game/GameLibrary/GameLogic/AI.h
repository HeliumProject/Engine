
#pragma once

#include "MathSimd/Vector2.h"

namespace GameLibrary
{
	class AIComponentChasePlayerDefinition;
	struct AvatarControllerComponent;

	typedef Helium::StrongPtr<AIComponentChasePlayerDefinition> AIComponentChasePlayerDefinitionPtr;	
	typedef Helium::StrongPtr<const AIComponentChasePlayerDefinition> ConstAIComponentChasePlayerDefinitionPtr;

	struct GAME_LIBRARY_API AIComponentChasePlayer : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( GameLibrary::AIComponentChasePlayer, Helium::Component );

		void Initialize( const AIComponentChasePlayerDefinition &definition);
	};

	class GAME_LIBRARY_API AIComponentChasePlayerDefinition : public Helium::ComponentDefinitionHelper<AIComponentChasePlayer, AIComponentChasePlayerDefinition>
	{
		HELIUM_DECLARE_CLASS( GameLibrary::AIComponentChasePlayerDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );
	};

	struct GAME_LIBRARY_API TaskProcessAI : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(TaskProcessAI)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}