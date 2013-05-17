#include "ExampleGamePch.h"

#include "ExampleGame/ExampleComponent.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;
using namespace ExampleGame;

HELIUM_IMPLEMENT_ASSET(ExampleGame::ExampleComponentDefinition, Components, 0);

void ExampleComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField(&ExampleComponentDefinition::m_TestValue, "m_TestValue");
	comp.AddField(&ExampleComponentDefinition::m_TestAssetReference, "m_TestAssetReference");
}

HELIUM_DEFINE_COMPONENT(ExampleGame::ExampleComponent, 16);

void ExampleComponent::PopulateStructure( Reflect::Structure& comp )
{

}
