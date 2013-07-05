#include "FrameworkPch.h"
#include "Framework/SystemDefinition.h"

using namespace Helium;

HELIUM_IMPLEMENT_ASSET( Helium::SystemComponent, Framework, 0)

DynamicArray< SystemComponent *> SystemComponent:: ms_FinalizeOrder;

void SystemComponent::PopulateStructure( Reflect::Structure& comp )
{

}

Helium::SystemComponent::SystemComponent()
	: m_Finalized(false)
{

}

HELIUM_IMPLEMENT_ASSET( Helium::SystemDefinition, Framework, 0)

void SystemDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField( &SystemDefinition::m_SystemComponents, "m_SystemComponents" );
}

void SystemDefinition::Initialize()
{
	for (DynamicArray< SystemComponentDefinitionPtr >::Iterator iter = m_SystemComponents.Begin();
		iter != m_SystemComponents.End(); ++iter)
	{
		(*iter)->Initialize();
	}

	for (DynamicArray< SystemComponentDefinitionPtr >::Iterator iter = m_SystemComponents.Begin();
		iter != m_SystemComponents.End(); ++iter)
	{
		(*iter)->DoFinalize();
	}
}

void SystemDefinition::Cleanup()
{
	SystemComponent::CleanupAllComponents();

	for (DynamicArray< SystemComponentDefinitionPtr >::Iterator iter = m_SystemComponents.Begin();
		iter != m_SystemComponents.End(); ++iter)
	{
		(*iter)->Destroy();
	}
}