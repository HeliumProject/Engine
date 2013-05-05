#include "ComponentsPch.h"
#include "Components/ExampleComponent.h"
#include "Reflect/TranslatorDeduction.h"

HELIUM_IMPLEMENT_ASSET(Helium::ExampleComponentDefinition, Components, 0);

void Helium::ExampleComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField(&ExampleComponentDefinition::m_TestValue, "m_TestValue");
	comp.AddField(&ExampleComponentDefinition::m_TestAssetReference, "m_TestAssetReference");
}

HELIUM_DEFINE_COMPONENT(Helium::ExampleComponent, 16);

void Helium::ExampleComponent::PopulateStructure( Reflect::Structure& comp )
{

}
