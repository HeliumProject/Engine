/*#include "Precompile.h"*/
#include "EntityInstanceType.h"

#include "EntitySet.h"
#include "EntityInstance.h"
#include "Core/Scene/Scene.h"

#include "Core/Scene/Viewport.h"
#include "Color.h"
#include "PrimitiveCube.h"
#include "PrimitiveSphere.h"
#include "PrimitiveCapsule.h"
#include "PrimitiveCylinder.h"

#include "Foundation/Component/ComponentHandle.h"
#include "Core/Asset/Manifests/SceneManifest.h"

using namespace Helium;
using namespace Helium::Core;

REFLECT_DEFINE_ABSTRACT(Core::EntityType);

void EntityType::InitializeType()
{
    Reflect::RegisterClassType< Core::EntityType >( TXT( "Core::EntityType" ) );
}

void EntityType::CleanupType()
{
    Reflect::UnregisterClassType< Core::EntityType >();
}

EntityType::EntityType( Core::Scene* scene, i32 instanceType )
: Core::InstanceType( scene, instanceType )
{
    ZeroMemory(&m_Material, sizeof(m_WireMaterial));
    m_Material.Ambient = Core::Color::BLACK;
    m_Material.Diffuse = Core::Color::BLACK;
    m_Material.Specular = Core::Color::BLACK;
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
        Core::EntitySet* set = Reflect::ObjectCast< Core::EntitySet > (itr->second);
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
        Core::EntitySet* set = Reflect::ObjectCast< Core::EntitySet > (itr->second);
        if (set)
        {
            set->Delete();
        }
    }
}

void EntityType::PopulateManifest(Asset::SceneManifest* manifest) const
{
    // iterate over every set in the Core::EntityInstance type
    M_InstanceSetSmartPtr::const_iterator setItr = m_Sets.begin();
    M_InstanceSetSmartPtr::const_iterator setEnd = m_Sets.end();
    for ( ; setItr != setEnd; ++setItr )
    {
        const Core::InstanceSet* set = setItr->second;
        const Core::EntitySet* entitySet = Reflect::ConstObjectCast<Core::EntitySet>( set );

        // if our set is a class set, insert the class id into the manifest
        if (entitySet)
        {
            manifest->m_Assets.insert( entitySet->GetEntityAssetPath() );
        }
    }

    __super::PopulateManifest( manifest );
}