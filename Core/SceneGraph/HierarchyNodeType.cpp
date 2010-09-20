/*#include "Precompile.h"*/
#include "HierarchyNodeType.h"

#include "Foundation/Reflect/Object.h" 

#include "Core/SceneGraph/Scene.h"
#include "HierarchyNode.h"
#include "Color.h"
#include "Primitive.h"

using namespace Helium;
using namespace Helium::Core;

REFLECT_DEFINE_ABSTRACT( Core::HierarchyNodeType );

void HierarchyNodeType::InitializeType()
{
  Reflect::RegisterClassType< Core::HierarchyNodeType >( TXT( "Core::HierarchyNodeType" ) );
}

void Core::HierarchyNodeType::CleanupType()
{
  Reflect::UnregisterClassType< Core::HierarchyNodeType >();
}

HierarchyNodeType::HierarchyNodeType( Core::Scene* scene, i32 instanceType )
: Core::SceneNodeType( scene, instanceType )
{
  int index = -1;

  m_Visible = true;
  m_Selectable = true;

  ZeroMemory(&m_WireMaterial, sizeof(m_WireMaterial));
  m_WireMaterial.Ambient = Core::Color::BLACK;
  m_WireMaterial.Diffuse = Core::Color::BLACK;
  m_WireMaterial.Specular = Core::Color::BLACK;

  ZeroMemory(&m_SolidMaterial, sizeof(m_SolidMaterial));
  m_SolidMaterial.Ambient = Core::Color::DARKGRAY;
  m_SolidMaterial.Diffuse = Core::Color::DARKGRAY;
  m_SolidMaterial.Specular = Core::Color::DARKGRAY;
}

void HierarchyNodeType::Create()
{

}

void HierarchyNodeType::Delete()
{

}

bool HierarchyNodeType::IsVisible() const
{
  return m_Visible;
}

void HierarchyNodeType::SetVisible(bool value)
{
  m_Visible = value;

  HM_SceneNodeSmartPtr::const_iterator itr = m_Instances.begin();
  HM_SceneNodeSmartPtr::const_iterator end = m_Instances.end();
  for ( ; itr != end; ++itr )
  {
    itr->second->Dirty();
  }
}

bool HierarchyNodeType::IsSelectable() const
{
  return m_Selectable;
}

void HierarchyNodeType::SetSelectable(bool value)
{
  m_Selectable = value;

  HM_SceneNodeSmartPtr::const_iterator itr = m_Instances.begin();
  HM_SceneNodeSmartPtr::const_iterator end = m_Instances.end();
  for ( ; itr != end; ++itr )
  {
    itr->second->Dirty();
  }
}

bool HierarchyNodeType::IsTransparent()
{
  return false;
}
