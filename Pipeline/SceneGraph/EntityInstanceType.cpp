/*#include "Precompile.h"*/
#include "EntityInstanceType.h"

#include "Foundation/Component/ComponentHandle.h"

#include "Pipeline/Asset/Manifests/SceneManifest.h"
#include "Pipeline/SceneGraph/Color.h"
#include "Pipeline/SceneGraph/EntitySet.h"
#include "Pipeline/SceneGraph/EntityInstance.h"
#include "Pipeline/SceneGraph/PrimitiveCube.h"
#include "Pipeline/SceneGraph/PrimitiveSphere.h"
#include "Pipeline/SceneGraph/PrimitiveCapsule.h"
#include "Pipeline/SceneGraph/PrimitiveCylinder.h"
#include "Pipeline/SceneGraph/Scene.h"
#include "Pipeline/SceneGraph/Viewport.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT(SceneGraph::EntityInstanceType);

void EntityInstanceType::InitializeType()
{
    Reflect::RegisterClassType< SceneGraph::EntityInstanceType >( TXT( "SceneGraph::EntityInstanceType" ) );
}

void EntityInstanceType::CleanupType()
{
    Reflect::UnregisterClassType< SceneGraph::EntityInstanceType >();
}

EntityInstanceType::EntityInstanceType( SceneGraph::Scene* scene, const Reflect::Class* instanceClass )
: SceneGraph::InstanceType( scene, instanceClass )
{

}

EntityInstanceType::~EntityInstanceType()
{

}

void EntityInstanceType::Reset()
{
    Base::Reset();
}

void EntityInstanceType::Create()
{
    Base::Create();

    M_InstanceSetSmartPtr::const_iterator itr = m_Sets.begin();
    M_InstanceSetSmartPtr::const_iterator end = m_Sets.end();
    for ( ; itr != end; ++itr )
    {
        SceneGraph::EntitySet* set = Reflect::SafeCast< SceneGraph::EntitySet > (itr->second);
        if (set)
        {
            set->Create();
        }
    }
}

void EntityInstanceType::Delete()
{
    Base::Delete();

    M_InstanceSetSmartPtr::const_iterator itr = m_Sets.begin();
    M_InstanceSetSmartPtr::const_iterator end = m_Sets.end();
    for ( ; itr != end; ++itr )
    {
        SceneGraph::EntitySet* set = Reflect::SafeCast< SceneGraph::EntitySet > (itr->second);
        if (set)
        {
            set->Delete();
        }
    }
}

void EntityInstanceType::AddInstance(SceneNodePtr n)
{
    Base::AddInstance( n );

    EntityInstance* entityInstance = Reflect::AssertCast< EntityInstance >( n );
    entityInstance->CheckSets();
}

void EntityInstanceType::PopulateManifest(Asset::SceneManifest* manifest) const
{
    // iterate over every set in the SceneGraph::EntityInstance type
    M_InstanceSetSmartPtr::const_iterator setItr = m_Sets.begin();
    M_InstanceSetSmartPtr::const_iterator setEnd = m_Sets.end();
    for ( ; setItr != setEnd; ++setItr )
    {
        const SceneGraph::InstanceSet* set = setItr->second;
        const SceneGraph::EntitySet* entitySet = Reflect::SafeCast<SceneGraph::EntitySet>( set );

        // if our set is a class set, insert the class id into the manifest
        if (entitySet)
        {
            manifest->m_Assets.insert( entitySet->GetEntityAssetPath() );
        }
    }

    Base::PopulateManifest( manifest );
}