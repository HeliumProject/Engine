#include "FrameworkPch.h"
#include "Framework/WorldDefinition.h"
#include "Framework/World.h"

#include "Framework/ParameterSet.h"

using namespace Helium;

HELIUM_IMPLEMENT_ASSET( Helium::WorldDefinition, Framework, 0 );

void Helium::WorldDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField( &WorldDefinition::m_ComponentDefinitionSet, "m_ComponentDefinitionSet" );
}


/// Constructor.
WorldDefinition::WorldDefinition()
{
}

/// Destructor.
WorldDefinition::~WorldDefinition()
{
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

    return spWorld;
}
