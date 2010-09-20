/*#include "Precompile.h"*/
#include "InstanceType.h"

#include "Foundation/Container/Insert.h" 

#include "Core/SceneGraph/Instance.h"
#include "Core/SceneGraph/Scene.h"
#include "Core/SceneGraph/Viewport.h"
#include "Core/SceneGraph/Color.h"
#include "Core/SceneGraph/PrimitivePointer.h"

using Helium::Insert; 

using namespace Helium;
using namespace Helium::Core;

REFLECT_DEFINE_ABSTRACT(Core::InstanceType);

void InstanceType::InitializeType()
{
    Reflect::RegisterClassType< Core::InstanceType >( TXT( "Core::InstanceType" ) );
}

void InstanceType::CleanupType()
{
    Reflect::UnregisterClassType< Core::InstanceType >();
}

InstanceType::InstanceType( Core::Scene* scene, i32 instanceType )
: Core::HierarchyNodeType( scene, instanceType )
, m_Pointer( NULL )
{
    ZeroMemory(&m_Material, sizeof(m_WireMaterial));
    m_Material.Ambient = Core::Color::BLACK;
    m_Material.Diffuse = Core::Color::BLACK;
    m_Material.Specular = Core::Color::BLACK;

    m_Pointer = new Core::PrimitivePointer( scene->GetViewport()->GetResources() );
    m_Pointer->Update();
}

InstanceType::~InstanceType()
{
    delete m_Pointer;
}

void InstanceType::Create()
{
    __super::Create();

    m_Pointer->Create();
}

void InstanceType::Delete()
{
    __super::Delete();

    m_Pointer->Delete();
}

bool InstanceType::IsTransparent()
{
    return false;
}

const D3DMATERIAL9& InstanceType::GetMaterial() const
{
    return m_Material;
}

void InstanceType::AddSet(Core::InstanceSet* set)
{
    Insert<M_InstanceSetSmartPtr>::Result inserted = m_Sets.insert( M_InstanceSetSmartPtr::value_type( set->GetName(), set ) );

    if ( inserted.second )
    {
        m_SetAdded.Raise( InstanceTypeChangeArgs( this, set ) ); 
    }
}

void InstanceType::RemoveSet(Core::InstanceSet* set)
{
    Helium::SmartPtr< Core::InstanceSet > keepAlive = set;

    if ( m_Sets.erase(set->GetName()) > 0 )
    {
        m_SetRemoved.Raise( InstanceTypeChangeArgs( this, set ) );
    }
}
