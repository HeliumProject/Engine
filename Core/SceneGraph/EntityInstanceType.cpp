/*#include "Precompile.h"*/
#include "EntityInstanceType.h"

#include "EntitySet.h"
#include "EntityInstance.h"
#include "Core/SceneGraph/Scene.h"

#include "Core/SceneGraph/Viewport.h"
#include "Color.h"
#include "PrimitiveCube.h"
#include "PrimitiveSphere.h"
#include "PrimitiveCapsule.h"
#include "PrimitiveCylinder.h"

#include "Foundation/Component/ComponentHandle.h"
#include "Core/Asset/Manifests/SceneManifest.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT(SceneGraph::EntityType);

void EntityType::InitializeType()
{
    Reflect::RegisterClassType< SceneGraph::EntityType >( TXT( "SceneGraph::EntityType" ) );
}

void EntityType::CleanupType()
{
    Reflect::UnregisterClassType< SceneGraph::EntityType >();
}

EntityType::EntityType( SceneGraph::Scene* scene, i32 instanceType )
: SceneGraph::InstanceType( scene, instanceType )
{
    ZeroMemory(&m_Material, sizeof(m_WireMaterial));
    m_Material.Ambient = SceneGraph::Color::BLACK;
    m_Material.Diffuse = SceneGraph::Color::BLACK;
    m_Material.Specular = SceneGraph::Color::BLACK;
}

EntityType::~EntityType()
{

}

void EntityType::Reset()
{
    __super::Reset();
}

void EntityType::Create()
{
    __super::Create();

    M_InstanceSetSmartPtr::const_iterator itr = m_Sets.begin();
    M_InstanceSetSmartPtr::const_iterator end = m_Sets.end();
    for ( ; itr != end; ++itr )
    {
        SceneGraph::EntitySet* set = Reflect::ObjectCast< SceneGraph::EntitySet > (itr->second);
        if (set)
        {
            set->Create();
        }
    }
}

void EntityType::Delete()
{
    __super::Delete();

    M_InstanceSetSmartPtr::const_iterator itr = m_Sets.begin();
    M_InstanceSetSmartPtr::const_iterator end = m_Sets.end();
    for ( ; itr != end; ++itr )
    {
        SceneGraph::EntitySet* set = Reflect::ObjectCast< SceneGraph::EntitySet > (itr->second);
        if (set)
        {
            set->Delete();
        }
    }
}

void EntityType::PopulateManifest(Asset::SceneManifest* manifest) const
{
    // iterate over every set in the SceneGraph::EntityInstance type
    M_InstanceSetSmartPtr::const_iterator setItr = m_Sets.begin();
    M_InstanceSetSmartPtr::const_iterator setEnd = m_Sets.end();
    for ( ; setItr != setEnd; ++setItr )
    {
        const SceneGraph::InstanceSet* set = setItr->second;
        const SceneGraph::EntitySet* entitySet = Reflect::ConstObjectCast<SceneGraph::EntitySet>( set );

        // if our set is a class set, insert the class id into the manifest
        if (entitySet)
        {
            manifest->m_Assets.insert( entitySet->GetEntityAssetPath() );
        }
    }

    __super::PopulateManifest( manifest );
}