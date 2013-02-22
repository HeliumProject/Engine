
#pragma once

namespace Helium
{
	class ExampleComponentDefinition : public Helium::Asset
	{
		HELIUM_DECLARE_ASSET( Helium::ExampleComponentDefinition, Helium::Asset );
        static void PopulateComposite( Reflect::Composite& comp );

		float m_TestValue;
		Helium::StrongPtr<ExampleComponentDefinition> m_TestAssetReference;
	};

	class ExampleComponent : public Component
	{
		HELIUM_DECLARE_COMPONENT( Helium::ExampleComponent, Helium::Component );
        static void PopulateComposite( Reflect::Composite& comp );
	};
}