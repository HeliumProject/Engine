#include "SceneGraphPch.h"
#include "EntityProxy.h"

#include "SceneGraph/Scene.h"

using namespace Helium;

REFLECT_DEFINE_OBJECT(Helium::EntityProxy);


EntityProxy::EntityProxy()
: m_Entity( NULL )
{

}

EntityProxy::EntityProxy( EntityDefinitionPtr definition )
: m_EntityDefinition( definition )
, m_Scene( NULL )
, m_Entity( NULL )
{
    HELIUM_ASSERT( definition );
}

bool EntityProxy::AttachToScene( SceneGraph::Scene* scene )
{
    HELIUM_ASSERT( scene );
    HELIUM_ASSERT( m_Scene == NULL );
    HELIUM_ASSERT( m_Entity == NULL );

    if ( scene == NULL || m_Scene != NULL )
        return false;

    Slice* slice = scene->GetSlice();
    HELIUM_ASSERT( slice );
    if ( slice == NULL )
        return false;

    m_Scene = scene;
    m_Entity = slice->CreateEntity( m_EntityDefinition );
    HELIUM_ASSERT( m_Entity );

    return (m_Entity != NULL);
}

bool EntityProxy::DetachFromScene()
{
    HELIUM_ASSERT( m_Scene != NULL );
    HELIUM_ASSERT( m_Entity != NULL );

    if ( m_Scene == NULL )
        return false;

    Slice* slice = m_Scene->GetSlice();
    HELIUM_ASSERT( slice );
    if ( slice == NULL )
        return false;

    bool successfullyDestroyed = slice->DestroyEntity( m_Entity );
    HELIUM_ASSERT( successfullyDestroyed );

    m_Entity = NULL;
    m_Scene = NULL;

    return successfullyDestroyed;
}
