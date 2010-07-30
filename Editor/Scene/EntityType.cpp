#include "Precompile.h"
#include "EntityType.h"

#include "EntityAssetSet.h"
#include "Entity.h"
#include "Scene.h"

#include "Viewport.h"
#include "Color.h"
#include "PrimitiveCube.h"
#include "PrimitiveSphere.h"
#include "PrimitiveCapsule.h"
#include "PrimitiveCylinder.h"

#include "Foundation/Component/ComponentHandle.h"
#include "Pipeline/Asset/Manifests/SceneManifest.h"

using namespace Helium;
using namespace Helium::Editor;

EDITOR_DEFINE_TYPE(Editor::EntityType);

void EntityType::InitializeType()
{
    Reflect::RegisterClass< Editor::EntityType >( TXT( "Editor::EntityType" ) );
}

void EntityType::CleanupType()
{
    Reflect::UnregisterClass< Editor::EntityType >();
}

EntityType::EntityType( Editor::Scene* scene, i32 instanceType )
: Editor::InstanceType( scene, instanceType )
{
    ZeroMemory(&m_Material, sizeof(m_WireMaterial));
    m_Material.Ambient = Editor::Color::BLACK;
    m_Material.Diffuse = Editor::Color::BLACK;
    m_Material.Specular = Editor::Color::BLACK;
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
        Editor::EntityAssetSet* set = Reflect::ObjectCast< Editor::EntityAssetSet > (itr->second);
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
        Editor::EntityAssetSet* set = Reflect::ObjectCast< Editor::EntityAssetSet > (itr->second);
        if (set)
        {
            set->Delete();
        }
    }
}

void EntityType::PopulateManifest(Asset::SceneManifest* manifest) const
{
    // iterate over every set in the Editor::Entity type
    M_InstanceSetSmartPtr::const_iterator setItr = m_Sets.begin();
    M_InstanceSetSmartPtr::const_iterator setEnd = m_Sets.end();
    for ( ; setItr != setEnd; ++setItr )
    {
        const Editor::InstanceSet* set = setItr->second;
        const Editor::EntityAssetSet* entitySet = Reflect::ConstObjectCast<Editor::EntityAssetSet>( set );

        // if our set is a class set, insert the class id into the manifest
        if (entitySet)
        {
            manifest->m_Assets.insert( entitySet->GetEntityAssetPath() );
        }
    }

    __super::PopulateManifest( manifest );
}