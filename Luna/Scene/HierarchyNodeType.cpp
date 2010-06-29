#include "Precompile.h"
#include "HierarchyNodeType.h"

#include "Foundation/Reflect/Object.h" 

#include "Scene.h"
#include "HierarchyNode.h"
#include "Color.h"
#include "Primitive.h"

using namespace Luna;

LUNA_DEFINE_TYPE( Luna::HierarchyNodeType );

void HierarchyNodeType::InitializeType()
{
  Reflect::RegisterClass< Luna::HierarchyNodeType >( TXT( "Luna::HierarchyNodeType" ) );
}

void Luna::HierarchyNodeType::CleanupType()
{
  Reflect::UnregisterClass< Luna::HierarchyNodeType >();
}

HierarchyNodeType::HierarchyNodeType( Luna::Scene* scene, i32 instanceType )
: Luna::SceneNodeType( scene, instanceType )
{
  int index = -1;

  m_Visible = true;
  m_Selectable = true;

  ZeroMemory(&m_WireMaterial, sizeof(m_WireMaterial));
  m_WireMaterial.Ambient = Luna::Color::BLACK;
  m_WireMaterial.Diffuse = Luna::Color::BLACK;
  m_WireMaterial.Specular = Luna::Color::BLACK;

  ZeroMemory(&m_SolidMaterial, sizeof(m_SolidMaterial));
  m_SolidMaterial.Ambient = Luna::Color::DARKGRAY;
  m_SolidMaterial.Diffuse = Luna::Color::DARKGRAY;
  m_SolidMaterial.Specular = Luna::Color::DARKGRAY;
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
