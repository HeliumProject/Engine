#include "Precompile.h"
#include "HierarchyNodeType.h"

#include "Foundation/Reflect/Object.h" 

#include "Scene.h"
#include "HierarchyNode.h"
#include "Color.h"
#include "Primitive.h"

using namespace Editor;

LUNA_DEFINE_TYPE( Editor::HierarchyNodeType );

void HierarchyNodeType::InitializeType()
{
  Reflect::RegisterClass< Editor::HierarchyNodeType >( TXT( "Editor::HierarchyNodeType" ) );
}

void Editor::HierarchyNodeType::CleanupType()
{
  Reflect::UnregisterClass< Editor::HierarchyNodeType >();
}

HierarchyNodeType::HierarchyNodeType( Editor::Scene* scene, i32 instanceType )
: Editor::SceneNodeType( scene, instanceType )
{
  int index = -1;

  m_Visible = true;
  m_Selectable = true;

  ZeroMemory(&m_WireMaterial, sizeof(m_WireMaterial));
  m_WireMaterial.Ambient = Editor::Color::BLACK;
  m_WireMaterial.Diffuse = Editor::Color::BLACK;
  m_WireMaterial.Specular = Editor::Color::BLACK;

  ZeroMemory(&m_SolidMaterial, sizeof(m_SolidMaterial));
  m_SolidMaterial.Ambient = Editor::Color::DARKGRAY;
  m_SolidMaterial.Diffuse = Editor::Color::DARKGRAY;
  m_SolidMaterial.Specular = Editor::Color::DARKGRAY;
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
