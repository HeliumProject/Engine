#include "ExamplePluginPch.h"

#include "ExamplePlugin/ExamplePluginComponent.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;
using namespace ExamplePlugin;

HELIUM_DEFINE_COMPONENT(ExamplePlugin::ExamplePluginComponent, 16);

void ExamplePluginComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

void ExamplePluginComponent::Initialize( const ExamplePluginComponentDefinition &definition )
{

}

HELIUM_DEFINE_CLASS(ExamplePlugin::ExamplePluginComponentDefinition);

void ExamplePluginComponentDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField(&ExamplePluginComponentDefinition::m_ExampleValue, "m_TestValue");
}
