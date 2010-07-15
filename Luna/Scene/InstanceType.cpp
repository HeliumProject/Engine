#include "Precompile.h"
#include "InstanceType.h"
#include "Instance.h"

#include "Scene.h"

#include "Viewport.h"
#include "Color.h"
#include "PrimitivePointer.h"

#include "Pipeline/Content/Nodes/Instance.h"
#include "Foundation/Container/Insert.h" 

using Nocturnal::Insert; 
using namespace Luna;

LUNA_DEFINE_TYPE(Luna::InstanceType);

void InstanceType::InitializeType()
{
  Reflect::RegisterClass< Luna::InstanceType >( TXT( "Luna::InstanceType" ) );
}

void InstanceType::CleanupType()
{
  Reflect::UnregisterClass< Luna::InstanceType >();
}

InstanceType::InstanceType( Luna::Scene* scene, i32 instanceType )
: Luna::HierarchyNodeType( scene, instanceType )
, m_Pointer( NULL )
{
  ZeroMemory(&m_Material, sizeof(m_WireMaterial));
  m_Material.Ambient = Luna::Color::BLACK;
  m_Material.Diffuse = Luna::Color::BLACK;
  m_Material.Specular = Luna::Color::BLACK;

  m_Pointer = new Luna::PrimitivePointer( scene->GetViewport()->GetResources() );
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

void InstanceType::AddSet(Luna::InstanceSet* set)
{
  Insert<M_InstanceSetSmartPtr>::Result inserted = m_Sets.insert( M_InstanceSetSmartPtr::value_type( set->GetName(), set ) );

  if ( inserted.second )
  {
    m_SetAdded.Raise( InstanceTypeChangeArgs( this, set ) ); 
  }
}

void InstanceType::RemoveSet(Luna::InstanceSet* set)
{
  Nocturnal::SmartPtr< Luna::InstanceSet > keepAlive = set;

  if ( m_Sets.erase(set->GetName()) > 0 )
  {
    m_SetRemoved.Raise( InstanceTypeChangeArgs( this, set ) );
  }
}
