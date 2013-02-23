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
    EntityPtr spEntity = Reflect::AssertCast<Entity>(Entity::CreateObject());

    if (m_ComponentDefinitions.Get())
    {
        ParameterSet parameterSet;
        spEntity->DeployComponents(*m_ComponentDefinitions, parameterSet);
    }

    return spEntity;
}
