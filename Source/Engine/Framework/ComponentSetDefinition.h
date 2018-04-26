
#pragma once

#include "Reflect/MetaStruct.h"

#include "Framework/ComponentSet.h"
#include "Framework/ComponentDefinition.h"

namespace Helium
{
	class ComponentSet;
	class ParameterSet;
	class ComponentDefinition;

	// Holds a set of definitions and allows them to construct and wire up together. Parameters can be provided, and the components themselves
	// can be components
	class HELIUM_FRAMEWORK_API ComponentSetDefinition : public Helium::Asset
	{
	public:
		HELIUM_DECLARE_ASSET(Helium::ComponentSetDefinition, Helium::Asset);
		static void PopulateMetaType( Reflect::MetaStruct& comp );
	private:

		ComponentSet m_Set;
	};
	typedef Helium::StrongPtr<ComponentSetDefinition> ComponentSetDefinitionPtr;
}
