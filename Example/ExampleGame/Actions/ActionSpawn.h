#pragma once

#include "ExampleGame/ExampleGame.h"
#include "Framework/Action.h"

namespace ExampleGame
{
	class EXAMPLE_GAME_API ActionSpawn : public Helium::Action
	{
	public:
		REFLECT_DECLARE_CLASS(ExampleGame::ActionSpawn, Helium::Action);
		virtual void PerformAction(Helium::World *pWorld, Helium::ParameterSet *pParamSet);

		//class 
	};
}
