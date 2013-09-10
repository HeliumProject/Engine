#include "FrameworkPch.h"
#include "Framework/SystemDefinition.h"

using namespace Helium;

//////////////////////////////////////////////////////////////////////////
// System Component
HELIUM_IMPLEMENT_ASSET( Helium::SystemComponent, Framework, 0)

DynamicArray< SystemComponent *> SystemComponent:: ms_FinalizeOrder;

void SystemComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

Helium::SystemComponent::SystemComponent()
	: m_Finalized(false)
{

}

//////////////////////////////////////////////////////////////////////////
// SystemDefinition
HELIUM_IMPLEMENT_ASSET( Helium::SystemDefinition, Framework, 0)

void SystemDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &SystemDefinition::m_SystemComponents, "m_SystemComponents" );
	comp.AddField( &SystemDefinition::m_ComponentTypeConfigs, "m_ComponentTypeConfigs" );
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

//////////////////////////////////////////////////////////////////////////
// ComponentTypeConfig

HELIUM_DEFINE_BASE_STRUCT( ComponentTypeConfig );

void ComponentTypeConfig::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &ComponentTypeConfig::m_ComponentTypeName, "m_ComponentTypeName" );
	comp.AddField( &ComponentTypeConfig::m_PoolSize, "m_PoolSize" );
}

bool ComponentTypeConfig::operator==( const ComponentTypeConfig& _rhs ) const
{
	return ( 
		m_ComponentTypeName == _rhs.m_ComponentTypeName &&
		m_PoolSize == _rhs.m_PoolSize
		);
}

bool ComponentTypeConfig::operator!=( const ComponentTypeConfig& _rhs ) const
{
	return !( *this == _rhs );
}
