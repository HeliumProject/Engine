#include "FrameworkPch.h"
#include "Framework/EntityDefinition.h"

#include "Framework/Slice.h"
#include "Framework/FrameworkDataDeduction.h"
#include "Framework/ParameterSet.h"

HELIUM_IMPLEMENT_ASSET( Helium::EntityDefinition, Framework, 0 );

using namespace Helium;

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

    if (m_ComponentDefinitions.Get())
    {
        ParameterSet parameterSet;
        pEntity->DeployComponents(*m_ComponentDefinitions, parameterSet);
    }
}