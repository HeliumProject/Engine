#include "FrameworkPch.h"
#include "Framework/WorldDefinition.h"
#include "Framework/World.h"

#include "Framework/ParameterSet.h"

using namespace Helium;

HELIUM_IMPLEMENT_ASSET( Helium::WorldDefinition, Framework, 0 );

void Helium::WorldDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &WorldDefinition::m_ComponentDefinitionSet, "m_ComponentDefinitionSet" );
	comp.AddField( &WorldDefinition::m_Components, "m_Components" );
}

/// Constructor.
WorldDefinition::WorldDefinition()
{
}

/// Destructor.
WorldDefinition::~WorldDefinition()
{
}

void WorldDefinition::AddComponentDefinition( Helium::Name name, Helium::ComponentDefinition *pComponentDefinition )
{
	if (!m_ComponentDefinitionSet)
	{
		InitComponentDefinitionSet();
	}

	m_ComponentDefinitionSet->AddComponentDefinition(name, pComponentDefinition);
}

Helium::WorldPtr WorldDefinition::CreateWorld() const
{
    WorldPtr spWorld(new World());
	
	if ( !spWorld->Initialize() )
	{
		return NULL;
	}
    
    if (m_ComponentDefinitionSet.Get())
	{
		HELIUM_TRACE(
			TraceLevels::Debug,
			"WorldDefinition::CreateWorld - Deploying components onto world from set %x to %x\n", m_ComponentDefinitionSet.Ptr(), this);

        ParameterSet parameterSet;
        Components::DeployComponents(*spWorld, *m_ComponentDefinitionSet, parameterSet);
    }
	else
	{
		Components::DeployComponents(*spWorld, m_Components);
	}

    return spWorld;
}
