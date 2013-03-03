#include "SceneGraphPch.h"
#include "EntityProxy.h"

using namespace Helium;

REFLECT_DEFINE_OBJECT(Helium::EntityProxy);


EntityProxy::EntityProxy()
: m_Entity( NULL )
{

}

EntityProxy::EntityProxy( EntityDefinitionPtr definition )
: m_EntityDefinition( definition )
, m_Entity( NULL )
{
    HELIUM_ASSERT( definition );
}

void EntityProxy::Invalidate()
{
    
}
