#include "FrameworkPch.h"
#include "Framework/WorldDefinition.h"
#include "Framework/World.h"

#include "Framework/ParameterSet.h"

HELIUM_IMPLEMENT_ASSET( Helium::WorldDefinition, Framework, 0 );

using namespace Helium;

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
    
    if (m_ComponentDefinitions.Get())
    {
        ParameterSet parameterSet;        
        Components::DeployComponents(*spWorld, *m_ComponentDefinitions, parameterSet);
    }

    return spWorld;
}
