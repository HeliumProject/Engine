#pragma once

#include "Reflect/MetaStruct.h"
#include "Engine/Asset.h"
#include "Math/Vector3.h"
#include "Framework/ComponentDefinition.h"

namespace ExamplePlugin
{
	class ExamplePluginComponentDefinition;

	class EXAMPLE_PLUGIN_API ExamplePluginComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( ExamplePlugin::ExamplePluginComponent, Helium::Component );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );
		
		void Initialize( const ExamplePluginComponentDefinition &definition);
	};
	
	class EXAMPLE_PLUGIN_API ExamplePluginComponentDefinition : public Helium::ComponentDefinitionHelper<ExamplePluginComponent, ExamplePluginComponentDefinition>
	{
		HELIUM_DECLARE_CLASS( ExamplePlugin::ExamplePluginComponentDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		float m_ExampleValue;
	};
	typedef Helium::StrongPtr<ExamplePluginComponentDefinition> ExamplePluginComponentDefinitionPtr;
}