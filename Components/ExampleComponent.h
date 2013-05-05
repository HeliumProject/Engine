
#pragma once

namespace Helium
{
	class HELIUM_COMPONENTS_API ExampleComponentDefinition : public Helium::Asset
	{
		HELIUM_DECLARE_ASSET( Helium::ExampleComponentDefinition, Helium::Asset );
        static void PopulateStructure( Reflect::Structure& comp );

		float m_TestValue;
		Helium::StrongPtr<ExampleComponentDefinition> m_TestAssetReference;
	};
    typedef StrongPtr<ExampleComponentDefinition> ExampleComponentDefinitionPtr;

	class HELIUM_COMPONENTS_API ExampleComponent : public Component
	{
		HELIUM_DECLARE_COMPONENT( Helium::ExampleComponent, Helium::Component );
        static void PopulateStructure( Reflect::Structure& comp );
	};
}