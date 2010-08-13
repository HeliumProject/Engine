/*#include "Precompile.h"*/
#include "InstanceType.h"
#include "Instance.h"

#include "Core/Scene/Scene.h"

#include "Core/Scene/Viewport.h"
#include "Color.h"
#include "PrimitivePointer.h"

#include "Core/Content/Nodes/ContentInstance.h"
#include "Foundation/Container/Insert.h" 

using Helium::Insert; 

using namespace Helium;
using namespace Helium::Core;

SCENE_DEFINE_TYPE(Core::InstanceType);

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
  m_Pointer->Create();

  __super::Create();
}

void InstanceType::Delete()
{
  m_Pointer->Delete();

  __super::Delete();
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
