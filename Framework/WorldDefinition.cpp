#include "Precompile.h"
#include "Framework/WorldDefinition.h"
#include "Framework/World.h"

#include "Framework/ParameterSet.h"

using namespace Helium;

HELIUM_IMPLEMENT_ASSET( Helium::WorldDefinition, Framework, 0 );

void Helium::WorldDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &WorldDefinition::m_ComponentSet, "m_ComponentSet" );
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
	m_ComponentSet.AddComponentDefinition(name, pComponentDefinition);
}

Helium::WorldPtr WorldDefinition::CreateWorld() const
{
	WorldPtr spWorld = new World();
	
	if ( !spWorld->Initialize() )
	{
		return NULL;
	}
	
	Components::DeployComponents(*spWorld, m_Components);
	Components::DeployComponents(*spWorld, m_ComponentSet);

	return spWorld;
}
