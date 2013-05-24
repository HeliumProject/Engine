#include "FrameworkPch.h"
#include "Framework/EntityDefinition.h"

#include "Framework/Slice.h"
#include "Framework/Entity.h"
#include "Framework/ParameterSet.h"

using namespace Helium;

HELIUM_IMPLEMENT_ASSET( Helium::EntityDefinition, Framework, 0 );

void Helium::EntityDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField( &EntityDefinition::m_ComponentDefinitionSet, "m_ComponentDefinitionSet" );
}

/// Constructor.
EntityDefinition::EntityDefinition()
{
}

/// Destructor.
EntityDefinition::~EntityDefinition()
{
}

Helium::EntityPtr Helium::EntityDefinition::CreateEntity()
{
    return Reflect::AssertCast<Entity>(Entity::CreateObject());
}

void Helium::EntityDefinition::FinalizeEntity( Entity *pEntity )
{
    HELIUM_ASSERT(pEntity);

    if (m_ComponentDefinitionSet.Get())
    {
        ParameterSet parameterSet;
        pEntity->DeployComponents(*m_ComponentDefinitionSet, parameterSet);
    }
}