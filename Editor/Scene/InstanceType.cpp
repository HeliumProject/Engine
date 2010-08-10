#include "Precompile.h"
#include "InstanceType.h"
#include "Instance.h"

#include "Editor/Scene/Scene.h"

#include "Editor/UI/Viewport.h"
#include "Color.h"
#include "PrimitivePointer.h"

#include "Core/Content/Nodes/Instance.h"
#include "Foundation/Container/Insert.h" 

using Helium::Insert; 

using namespace Helium;
using namespace Helium::Editor;

EDITOR_DEFINE_TYPE(Editor::InstanceType);

void InstanceType::InitializeType()
{
  Reflect::RegisterClass< Editor::InstanceType >( TXT( "Editor::InstanceType" ) );
}

void InstanceType::CleanupType()
{
  Reflect::UnregisterClass< Editor::InstanceType >();
}

InstanceType::InstanceType( Editor::Scene* scene, i32 instanceType )
: Editor::HierarchyNodeType( scene, instanceType )
, m_Pointer( NULL )
{
  ZeroMemory(&m_Material, sizeof(m_WireMaterial));
  m_Material.Ambient = Editor::Color::BLACK;
  m_Material.Diffuse = Editor::Color::BLACK;
  m_Material.Specular = Editor::Color::BLACK;

  m_Pointer = new Editor::PrimitivePointer( scene->GetViewport()->GetResources() );
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

void InstanceType::AddSet(Editor::InstanceSet* set)
{
  Insert<M_InstanceSetSmartPtr>::Result inserted = m_Sets.insert( M_InstanceSetSmartPtr::value_type( set->GetName(), set ) );

  if ( inserted.second )
  {
    m_SetAdded.Raise( InstanceTypeChangeArgs( this, set ) ); 
  }
}

void InstanceType::RemoveSet(Editor::InstanceSet* set)
{
  Helium::SmartPtr< Editor::InstanceSet > keepAlive = set;

  if ( m_Sets.erase(set->GetName()) > 0 )
  {
    m_SetRemoved.Raise( InstanceTypeChangeArgs( this, set ) );
  }
}
