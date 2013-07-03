#pragma once

#include "Framework/Framework.h"
#include "Framework/ParameterSet.h"
#include "Framework/World.h"

#include "Reflect/Object.h"

namespace Helium
{
	class HELIUM_FRAMEWORK_API Action : public Reflect::Object
	{
	public:
		REFLECT_DECLARE_ABSTRACT(Helium::Action, Reflect::Object);
		virtual void PerformAction(World *pWorld, ParameterSet *pParamSet) = 0;
	};
}
