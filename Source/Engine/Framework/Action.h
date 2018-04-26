#pragma once

#include "Framework/Framework.h"
#include "Framework/ParameterSet.h"
#include "Framework/World.h"

#include "Reflect/Object.h"

namespace Helium
{
	class HELIUM_FRAMEWORK_API Action : public Asset
	{
	public:
		HELIUM_DECLARE_ASSET( Action, Asset );
		virtual void PerformAction( World &pWorld, ParameterSet *pParamSet ) { HELIUM_ASSERT(0); }
	};
	typedef Helium::StrongPtr< Action > ActionPtr;
}
