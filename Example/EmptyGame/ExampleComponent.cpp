#include "EmptyGamePch.h"

#include "EmptyGame/ExampleComponent.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;
using namespace EmptyGame;

HELIUM_DEFINE_COMPONENT(EmptyGame::ExampleComponent, 16);

void ExampleComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void ExampleComponent::Initialize( const ExampleComponentDefinition &definition )
{

}

HELIUM_IMPLEMENT_ASSET(EmptyGame::ExampleComponentDefinition, Components, 0);

void ExampleComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField(&ExampleComponentDefinition::m_ExampleValue, "m_TestValue");
}
