#include "Precompile.h"
#include "EntityType.h"

#include "EntityAssetSet.h"
#include "Entity.h"
#include "Scene.h"

#include "View.h"
#include "Color.h"
#include "PrimitiveCube.h"
#include "PrimitiveSphere.h"
#include "PrimitiveCapsule.h"
#include "PrimitiveCylinder.h"

#include "Pipeline/Component/ComponentHandle.h"
#include "Pipeline/Asset/Manifests/SceneManifest.h"

using namespace Luna;

LUNA_DEFINE_TYPE(Luna::EntityType);

void EntityType::InitializeType()
{
    Reflect::RegisterClass< Luna::EntityType >( "Luna::EntityType" );
}

void EntityType::CleanupType()
{
    Reflect::UnregisterClass< Luna::EntityType >();
}

EntityType::EntityType( Luna::Scene* scene, i32 instanceType )
: Luna::InstanceType( scene, instanceType )
{
    ZeroMemory(&m_Material, sizeof(m_WireMaterial));
    m_Material.Ambient = Luna::Color::BLACK;
    m_Material.Diffuse = Luna::Color::BLACK;
    m_Material.Specular = Luna::Color::BLACK;
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
    M_InstanceSetSmartPtr::const_iterator itr = m_Sets.begin();
    M_InstanceSetSmartPtr::const_iterator end = m_Sets.end();
    for ( ; itr != end; ++itr )
    {
        Luna::EntityAssetSet* set = Reflect::ObjectCast< Luna::EntityAssetSet > (itr->second);
        if (set)
        {
            set->Create();
        }
    }
}

void EntityType::Delete()
{
    M_InstanceSetSmartPtr::const_iterator itr = m_Sets.begin();
    M_InstanceSetSmartPtr::const_iterator end = m_Sets.end();
    for ( ; itr != end; ++itr )
    {
        Luna::EntityAssetSet* set = Reflect::ObjectCast< Luna::EntityAssetSet > (itr->second);
        if (set)
        {
            set->Delete();
        }
    }
}

void EntityType::PopulateManifest(Asset::SceneManifest* manifest) const
{
    // iterate over every set in the Luna::Entity type
    M_InstanceSetSmartPtr::const_iterator setItr = m_Sets.begin();
    M_InstanceSetSmartPtr::const_iterator setEnd = m_Sets.end();
    for ( ; setItr != setEnd; ++setItr )
    {
        const Luna::InstanceSet* set = setItr->second;
        const Luna::EntityAssetSet* entitySet = Reflect::ConstObjectCast<Luna::EntityAssetSet>( set );

        // if our set is a class set, insert the class id into the manifest
        if (entitySet)
        {
            manifest->m_Assets.insert( entitySet->GetEntityAssetPath() );
        }
    }

    __super::PopulateManifest( manifest );
}