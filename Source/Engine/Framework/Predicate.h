#pragma once

#include "Framework/Framework.h"

namespace Helium
{
	class World;
	class ParameterSet;

	class HELIUM_FRAMEWORK_API Predicate : public Asset
	{
		HELIUM_DECLARE_ASSET( Predicate, Asset );
		virtual bool Evaluate( World &pWorld, ParameterSet *parameters ) { HELIUM_ASSERT( 0 ); return false; }
	};
	typedef Helium::StrongPtr< Predicate > PredicatePtr;
}

#include "Framework/Predicate.inl"
